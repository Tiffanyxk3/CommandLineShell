/**
 * @file
 *
 * Utility functions.
 */

#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdbool.h>

/**
 * command line structure
 */
struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
    char *stdin_file;
    bool stdout_append;
};

char *next_token(char **str_ptr, const char *delim);
void pipeline(struct command_line *cmds);

#endif

