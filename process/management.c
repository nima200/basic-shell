#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include "management.h"

void pushJob(struct process **job, pid_t PID, const char *command) {
    struct process* newJob = (struct process*) malloc(sizeof(struct process));
    newJob->id = PID;
    newJob->command = command;
    newJob->next = (*job);
    (*job) = newJob;
}

void flush(struct process **head) {
    struct process *temp = *head, *prev;
    while (temp != NULL && (waitpid(temp->id, NULL, WNOHANG)) != 0) {
        *head = temp->next;
        free(temp);
        temp = *head;
    }
    while (temp != NULL) {
        while (temp != NULL && (waitpid(temp->id, NULL, WNOHANG)) == 0) {
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL) return;
        prev->next = temp->next;
        free(temp);
        temp = prev->next;
    }
}

void showJobs(struct process *job) {
    int i = 0;
    while (job != NULL) {
        i++;
        char *status = waitpid(job->id, NULL, WNOHANG) > 0 ? "Done" :
            waitpid(job->id, NULL, WNOHANG) == -1 ? "Error" : "Running";
        printf("\t[%d] (PID:%d) %s\t\t\t%s\n", i, job->id, status, job->command);
        job = job->next;
    }
}

