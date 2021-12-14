/**
 * @file builtin
 *
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "history.h"
#include "logger.h"
#include "ui.h"
#include "util.h"
#include "builtin.h"
#include "job.h"

int status;
int pipe_num = 0;
int redir_num = 0;
int backg_num = 0;


/**
 * Executes the command
 *
 * @param command command line
 * @param add whether to add it to history list
 * 
 * @return number
 */
int command_driver(char *command, bool add) {
	
	char *command1 = strdup(command);
	if (strncmp(command1, "!", 1) != 0 && add) {
		hist_add(command1);
	}
	char *args[100] = { 0 };
	int tokens = 0;
	char *next_tok = command;
	char *curr_tok;
	while ((curr_tok = next_token(&next_tok, " \t\n\r")) != NULL) {
		if (strncmp(curr_tok, "#", 1) == 0) {
			break;
		}
		if (strncmp(curr_tok, "|", 1) == 0) {
			pipe_num++;
		}
		if (strncmp(curr_tok, ">", 1) == 0) {
			redir_num++;
		}
		if (strncmp(curr_tok, "&", 1) == 0) {
			backg_num++;
			break;
		}
		args[tokens++] = curr_tok;
	}
	args[tokens] = (char *) NULL;

	if (args[0] == NULL) {
		free(command1);
		return 0;
	}

	if (strncmp(args[0], "cd", 2) == 0) {
		cd(args);
	}
	else if (strncmp(args[0], "history", 7) == 0) {
		show_history();
	}
	else if (strncmp(args[0], "!", 1) == 0) {
		execute_history(args[0]);
		hist_add(command1);
	}
	else if (strncmp(args[0], "jobs", 4) == 0) {
		show_jobs();
	}
	else if (strncmp(args[0], "exit", 4) == 0) {
		free(command1);
		return EXIT_FAILURE;
	}
	else {
		pid_t child = fork();
		if (backg_num > 0) {
			job_add(command1, child);
		}

		if (child == -1) {
			perror("fork");
			free(command1);
			return 0;
		}
		else if (child == 0) {
			/* child process*/
			if (pipe_num > 0 || redir_num > 0) {
				execute_pipeline(args);
			}
			else {
				int ret = execvp(args[0], args);
				if (ret == -1) {
					perror("execvp");
					close(fileno(stdin));
					close(fileno(stdout));
					close(fileno(stderr));
					free(command1);
					return EXIT_FAILURE;
				}
			}
		}
		else {
			// parent process
			if (backg_num == 0) {
				waitpid(child, &status, 0);
			}
		}
	}

	backg_num = 0;
	free(command1);
	return 0;
}

/**
 * Handler for SIGCHLD
 *
 * @param signo signo
 */
void sigchld_handler(int signo) {
	int s;
	pid_t child = waitpid(-1, &s, WNOHANG);
	job_remove(child);
}

/**
 * Executes the pipeline
 *
 * @param args command line
 */
void execute_pipeline(char *args[]) {
	struct command_line *cmds = malloc(128 * sizeof(struct command_line));
	int i = 0, j = 0, cmd_index = 0;
	cmds[0].tokens = malloc(128 * sizeof(char *));
	while (args[i] != NULL) {
		if (strncmp(args[i], "|", 1) == 0 || strncmp(args[i], ">", 1) == 0) {
			if (strncmp(args[i], "|", 1) == 0) {
				cmds[cmd_index].tokens[j] = NULL;
				cmds[cmd_index].stdout_pipe = true;
				cmd_index++;
				j = 0;
				cmds[cmd_index].tokens = malloc(128 * sizeof(char *));
			}
			else if (strncmp(args[i], ">>", 2) == 0) {
				cmds[cmd_index].stdout_file = args[i + 1];
				cmds[cmd_index].stdout_append = true;
				break;
			}
			else if (strncmp(args[i], ">", 1) == 0) {
				cmds[cmd_index].stdout_file = args[i + 1];
				break;
			}
		}
		else {
			cmds[cmd_index].tokens[j++] = args[i];
			cmds[cmd_index].stdout_pipe = false;
		}
		i++;
	}

	pipeline(cmds);
}


/**
 * Configureing 'cd' command
 *
 * @param args command line
 *
 * @return number
 */
int cd(char *args[]) {
	if (args[1] == NULL || strncmp(args[1], "~", 1) == 0) {
		return chdir(getenv("HOME"));
	}
	return chdir(args[1]);

}

/**
 * Prints history
 *
 * @return number
 */
int show_history() {
	hist_print();
	return 0;
}

/**
 * Executes the most recent command
 *
 * @return number
 */
int execute_last() {
	int number = hist_last_cnum();
	execute_number(number);
	return 0;
}

/**
 * Executes the command with command 'number'
 *
 * @param number command number
 *
 * @return number
 */
int execute_number(int number) {
	char *command = hist_search_cnum(number);
	if (command == NULL) {
		return 0;
	}
	command_driver(command, false);
	return 0;
}

/**
 * Executes the command with 'prefix'
 *
 * @param prefix command prefix
 *
 * @return number
 */
int execute_prefix(char *prefix) {
	char *command = hist_search_prefix(prefix);
	if (command == NULL) {
		return 0;
	}
	command_driver(command, false);
	return 0;
}

/**
 * Configuring 'history' command
 *
 * @param arg command line
 *
 * @return number
 */
void execute_history(char *arg) {
	char exe[16];
	for (int i = 1; i < strlen(arg); i++) {
		exe[i - 1] = *(arg + i);
	}
	exe[strlen(arg) - 1] = '\0';

	if (exe[0] == '!') {
		execute_last();
	}
	else {
		bool number = true;
		for (int i = 0; i < strlen(exe); i++) {
			if (*(exe + i) < '0' || *(exe + i) > '9') {
				number = false;
			}
		}
		if (number) {
			execute_number(atoi(exe));
		}
		else {
			execute_prefix(exe);
		}
	}
}
