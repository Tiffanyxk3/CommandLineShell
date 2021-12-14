/**
 * @file history
 *
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "builtin.h"
#include "job.h"

struct hist_st *history;
int numbers_cmd = 0;
int max;

/**
 * history structure
 */
struct hist_st {
    char command[256];
    int number;
};

/**
 * Set up history data structures, with 'limit' being the maximum number of entries maintained.
 *
 * @param limit number of maximum entries
 */
void hist_init(unsigned int limit) {
    history = (struct hist_st *) malloc(limit * sizeof(struct hist_st));
    max = limit;

}

/**
 * Destroys history entries
 */
void hist_destroy(void) {
    free(history);
}

/**
 * Adding entry
 *
 * @param cmd command line to be added
 */
void hist_add(const char *cmd) {
    int index = numbers_cmd % max;
    memset(history[index].command, 0, sizeof(history[index].command));

    for (int i = 0; i < strlen(cmd); i++) {
        history[index].command[i] = cmd[i];
    }
    history[index].number = numbers_cmd + 1;

    numbers_cmd++;
}

/**
 * Prints history entries
 */
void hist_print(void) {
    int start = 0;
    if (numbers_cmd > max) {
        start = (numbers_cmd - max) % max;
    }
    int end = numbers_cmd;
    if (numbers_cmd > max) {
        end = max;
    }

    for (int i = 0; i < end; i++) {
        if (history[start].command == NULL) {
            break;
        }
        printf("%d %s\n", history[start].number, history[start].command);
        fflush(stdout);
        start++;
        if (start == max) {
            start = 0;
        }
    }
}

/**
 * Retrieves the most recent command starting with 'prefix'
 *
 * @param prefix prefix to be searched on
 *
 * @return entry that matches, NULL if no match
 */
char *hist_search_prefix(char *prefix) {
    if (prefix == NULL) {
        return NULL;
    }

    int start = (numbers_cmd % max) - 1;
    for (int i = start; i >= 0; i--) {
        if (strncmp(prefix, history[i].command, strlen(prefix)) == 0) {
            return history[i].command;
        }
    }
    if (numbers_cmd < max) {
        return NULL;
    }
    for (int i = max - 1; i > start; i--) {
        if (strncmp(prefix, history[i].command, strlen(prefix)) == 0) {
            return history[i].command;
        }
    }
    return NULL;
}

/**
 * Retrieves a particular command number
 *
 * @param command_number number to be searched on
 *
 * @return entry that matches, NULL if no match
 */
char *hist_search_cnum(int command_number) {
    int index = command_number % max;
    if (index == 0) {
        index = max;
    }

    if (command_number == history[index - 1].number) {
        return history[index - 1].command;
    }
    return NULL;

}

/**
 * Retrieve the most recent command number
 *
 * @return command number
 */
unsigned int hist_last_cnum(void) {
    return history[numbers_cmd - 1].number;
}

