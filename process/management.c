#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <errno.h>
#include "../commands.h"
#include "management.h"

process* createNewJob() {
    struct process *job = malloc(sizeof(process));
    if (job == NULL) {
        errno = 12;
        perror("ERROR: An error occurred while trying to allocate memory to job");
        return NULL;
    }
    job->id = 0;
    job->previous = NULL;
    job->next = NULL;
    job->command = NULL;
    return job;
}

void pushJob(struct process *job, pid_t PID, const command *command) {
    struct process* newJob = createNewJob();
    newJob->id = PID;
    newJob->command = COMMAND_STRING[*command];
    struct process* p = job;
    while (1) {
        if (p->next == NULL) {
            p->next = newJob;
            newJob->previous = p;
            break;
        } else {
            p = p->next;
        }
    }
}

void flush(struct process *job) {
    struct process **currentJob = &job;
    while (*currentJob != NULL) {
        if ((*currentJob)->id != 0) {
            int status = waitpid((*currentJob)->id, NULL, WNOHANG | WUNTRACED);
            if (status != 0) {
                // REMOVE FROM END
                if ((*currentJob)->next == NULL) {
                    (*currentJob)->previous->next = NULL;
                    free(*currentJob);
                    break;
                } else {
                    // REMOVE FROM MIDDLE
                    (*currentJob)->previous->next = (*currentJob)->next;
                    (*currentJob)->next->previous = (*currentJob)->previous;
                    free(*currentJob);
                    currentJob = &((*currentJob)->next);
                }
            }
        } else {
            currentJob = &((*currentJob)->next);
        }
    }
}

void showJobs(struct process *job) {

}

