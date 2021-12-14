/**
 * @file
 *
 * Contains shell job data structures and retrieval functions.
 */

#ifndef _JOB_H_
#define _JOB_H_

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern int jobs_num;

void job_init();
void job_destroy();
void show_jobs();
void job_add(char *command, pid_t child);
void job_remove(pid_t child);

#endif
