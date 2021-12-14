/**
 * @file util
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "builtin.h"
#include "job.h"

/**
 * Retrieves the next token from a string.
 *
 * @param str_ptr maintains context in the string
 * @param delim command the set of characters to use as delimiters
 *
 * @return char pointer to the next token in the string
 */
char *next_token(char **str_ptr, const char *delim) {
    if (*str_ptr == NULL) {
        return NULL;
    }

    size_t tok_start = strspn(*str_ptr, delim);
    size_t tok_end = strcspn(*str_ptr + tok_start, delim);

    // Zero length token. We must be finished.
    if (tok_end  == 0) {
        *str_ptr = NULL;
        return NULL;
    }

    // Take note of the start of the current token. We'll return it later.
    char *current_ptr = *str_ptr + tok_start;

    // Shift pointer forward (to the end of the current token)
    *str_ptr += tok_start + tok_end;

    if (**str_ptr == '\0') {
        // If the end of the current token is also the end of the string, we must be at the last token.
        *str_ptr = NULL;
    } else {
        // Replace the matching delimiter with a NUL character to terminate the token string.
        **str_ptr = '\0';

        // Shift forward one character over the newly-placed NUL so that next_pointer now points at the first character of the next token.
        (*str_ptr)++;
    }

    return current_ptr;
}

/**
 * Creates and executes pipeline
 *
 * @param cmds command line
 */
void pipeline(struct command_line *cmds) {
    while (cmds->stdout_pipe) {
        int fd[2];
        if (pipe(fd) == -1) {
            perror("pipe");
            return;
        }
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            return;
        }
        else if (pid == 0) {
            dup2(fd[1], STDOUT_FILENO);
            execvp(cmds->tokens[0], cmds->tokens);
            close(fd[0]);
        }
        else {
            dup2(fd[0], STDIN_FILENO);
            close(fd[1]);
        }
        cmds++;
    }

    if (cmds->stdout_file != NULL) {
        int out;
        if (cmds->stdout_append) {
            out = open(cmds->stdout_file, O_WRONLY | O_APPEND);
        }
        else {
            out = open(cmds->stdout_file, O_RDWR | O_CREAT | O_TRUNC);
        }
        if (out == -1) {
            perror("open");
            return;
        }
        dup2(out, STDOUT_FILENO);
        execvp(cmds->tokens[0], cmds->tokens);
        close (out);
        return;
    }
    else {
        execvp(cmds->tokens[0], cmds->tokens);
    }
}

