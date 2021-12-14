/**
 * @file job
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


int jobs_num = 0;

/** job structure */
struct job_st {
    char *job;
    int pid;
};

struct job_st *jobs;

/** Initializes job entries */
void job_init() {
	jobs = (struct job_st *) malloc(10 * sizeof(struct job_st));
}

/** Destroys job entries */
void job_destroy() {
	for (int i = 0; i < jobs_num; i++) {
		free(jobs[i].job);
	}
	free(jobs);
}

/** Prints the job list */
void show_jobs() {
	for (int i = 0; i < jobs_num; i++) {
		if (jobs[i].job == NULL) {
			continue;
		}
		printf("%s\n", jobs[i].job);
		fflush(stdout);
	}
}

/** Adds a job */
void job_add(char *command, pid_t child) {
	jobs[jobs_num].job = strdup(command);
	jobs[jobs_num].pid = child;
	jobs_num++;
}

/** Removs a job */
void job_remove(pid_t child) {
	for (int i = 0; i < jobs_num; i++) {
		if (jobs[i].pid == child) {
			jobs[i].job = NULL;
		}
	}
}