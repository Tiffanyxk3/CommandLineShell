/**
 * @file ui
 *
 */

#include <stdio.h>
#include <readline/readline.h>
#include <locale.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "builtin.h"
#include "job.h"

static int readline_init(void);
static char prompt_str[80] = "--[enter a command]--> ";
static bool scripting = false;
static int hist_index = 0;

/**
 * Initializes ui
 */
void init_ui(void) {
    LOGP("Initializing UI...\n");

    char *locale = setlocale(LC_ALL, "en_US.UTF-8");
    LOG("Setting locale: %s\n", (locale != NULL) ? locale : "could not set locale!");

    if (isatty(STDIN_FILENO)) {
        LOGP("stdin is a TTY; entering interactive mode\n");
    }
    else {
        LOGP("data piped in on stdin; entering script mode\n");
        scripting = true;
    }

    rl_startup_hook = readline_init;
}

/**
 * Prompts the line
 *
 * @return the prompted line
 */
char *prompt_line(void) {
    char *username = getenv("USER");

    char hostname[64];
    gethostname(hostname, 64);

    char *path = getcwd(NULL, 0);
    char pth[128];
    if (strstr(path, getenv("HOME")) != NULL) {
        if (strncmp(path, getenv("HOME"), strlen(getenv("HOME"))) == 0) {
            pth[0] = '~';
            int start = 1;
            for (int i = strlen(getenv("HOME")); i < strlen(path); i++) {
                pth[start] = *(path + i);
                start++;
            }
            pth[strlen(path) - strlen(getenv("HOME")) + 1] = '\0';
            path = pth;
        }
        
    }

    char *status_emoji;
    if (status == 0) {
        status_emoji = ":)";
    }
    else {
        status_emoji = ":(";
    }

    sprintf(prompt_str, "[%s]-[%d]-[%s@%s:%s]$ ", status_emoji, numbers_cmd + 1, username, hostname, path);

    // free(path);
    return prompt_str;
}

/**
 * Reads the command
 *
 * @return the command line
 */
char *read_command(void) {
    if (scripting == true) {
        char *line = NULL;
        size_t line_sz = 0;
        size_t read_sz = getline(&line, &line_sz, stdin);
        if (read_sz == -1) {
            free(line);
            return NULL;
        }
        line[read_sz - 1] = '\0';
        char *ln = strdup(line);
        free(line);
        return ln;
    }
    return readline(prompt_line());
}

/**
 * Read line
 *
 * @return 0
 */
int readline_init(void) {
    hist_index = numbers_cmd;
    rl_bind_keyseq("\\e[A", key_up);
    rl_bind_keyseq("\\e[B", key_down);
    rl_variable_bind("show-all-if-ambiguous", "on");
    rl_variable_bind("colored-completion-prefix", "on");
    rl_attempted_completion_function = command_completion;
    return 0;
}

/**
 * Configuring key up
 *
 * @return 0
 */
int key_up(int count, int key) {
    if (hist_index == 0 || hist_index > numbers_cmd) {
        return 0;
    }
    hist_index--;
    char *hist_search = hist_search_cnum(hist_index + 1);
    rl_replace_line(hist_search, 1);

    rl_point = rl_end;
    return 0;
}

/**
 * Configuring key down
 *
 * @return 0
 */
int key_down(int count, int key) {
    if (hist_index > numbers_cmd) {
        return 0;
    }
    hist_index++;
    char *hist_search = hist_search_cnum(hist_index + 1);
    if (hist_search != NULL) {
        rl_replace_line(hist_search, 1);
    }
    
    rl_point = rl_end;
    return 0;
}

/**
 * Completing command
 *
 * @param text string
 * @param start index
 * @param end index
 *
 * @return matches
 */
char **command_completion(const char *text, int start, int end) {
    // Tell readline that if we don't find a suitable completion, it should fall back on its built-in filename completion.
    rl_attempted_completion_over = 0;

    return rl_completion_matches(text, command_generator);
}

/**
 * Generates command:
 * This function is called repeatedly by the readline library to build a list of
 * possible completions. It returns one match per function call. Once there are
 * no more completions available, it returns NULL.
 *
 * @param text string
 * @param state 
 *
 * @return null
 */
char *command_generator(const char *text, int state) {
    // Finds potential matching completions for 'text.'

    if (text == NULL) {
        return NULL;
    }
    char *path_def = getenv("PATH");

    int start = 0;
    while (true) {
        char path[256];
        int i = 0;
        while (true) {
            if (*(path_def + start) == ':' || start == strlen(path_def)) {
                break;
            }
            path[i] = *(path_def + start);
            i++;
            start++;
        }
        path[i] = '\0';
        start+=2;
        if (i == 0) {
            break;
        }
        DIR *directory = opendir(path);
        if (directory == NULL) {
            break;
        }
        struct dirent *file;
        while ((file = readdir(directory)) != NULL) {
            if (access(file->d_name, X_OK)) {
                closedir(directory);
                return file->d_name;
            }
        }
        closedir(directory);
    }

    int len = strlen(text);
    if (strncmp(text, "cd", len) == 0) {
        rl_attempted_completion_over = 1;
        return "cd";
    }
    if (strncmp(text, "history", len) == 0) {
        return "history";
    }
    if (strncmp(text, "jobs", len) == 0) {
        return "jobs";
    }
    if (strncmp(text, "exit", len) == 0) {
        return "exit";
    }

    return NULL;
}
