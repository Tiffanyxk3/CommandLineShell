/**
 * @file shell
 *
 */

#include <fcntl.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "builtin.h"
#include "job.h"

int main(void) {
    init_ui();
    hist_init(100);
    job_init();

    signal(SIGINT, SIG_IGN);
    signal(SIGCHLD, sigchld_handler);

    while (true) {
        char *command = read_command();
        if (command == NULL) {
            hist_destroy();
            job_destroy();
            close(fileno(stdin));
            close(fileno(stdout));
            close(fileno(stderr));
            free(command);
            return 0;
        }
        int execution = command_driver(command, true);
        if (execution == EXIT_FAILURE) {
            hist_destroy();
            job_destroy();
            close(fileno(stdin));
            close(fileno(stdout));
            close(fileno(stderr));
            free(command);
            exit(0);
            return EXIT_FAILURE;
        }
        free(command);
    }

    return 0;
}