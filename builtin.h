/**
 * @file
 *
 * Contains built-in commands of my shell.
 */

#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdbool.h>

extern int status;
extern int pipe_num;
extern int redir_num;
extern int backg_num;

int command_driver(char *command, bool add);
void sigchld_handler(int signo);
void execute_pipeline(char *args[]);
int cd(char *args[]);
int show_history();
int execute_last();
int execute_number(int number);
int execute_prefix(char *prefix);
void execute_history(char *arg);
void show_jobs();
void job_add(char *command, pid_t child);
void job_remove(pid_t child);

#endif
