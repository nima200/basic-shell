#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include "management.h"

process* createNewJob() {
    struct process *job = malloc(sizeof(process));
    job->id = 0;
}

void pushJob(struct process *job, pid_t PID) {
    struct process* newJob = createNewJob();
    newJob->id = PID;
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
    struct process *currentJob = job;
    int i = 0;
    while (currentJob->next != NULL) {
        int status = waitpid(currentJob->id, NULL, WNOHANG | WUNTRACED);
        if (status != 0) {
            currentJob->previous->next = currentJob->next;
            currentJob->next->previous = currentJob->previous;
            currentJob = currentJob->next;
            free(currentJob);
            i++;
        } else {
            currentJob = currentJob->next;
        }
    }
}

