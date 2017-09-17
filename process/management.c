#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include <errno.h>
#include "management.h"

void pushJob(process **job, pid_t PID, const char *command) {
    process* newJob = (process*) malloc(sizeof(process));
    if (newJob == NULL) {
        errno = 12;
        perror("ERROR: Could not allocate enough memory to the job");
        return;
    }
    newJob->id = PID;
    newJob->command = command;
    newJob->next = (*job);
    newJob->index = *job == NULL ? 1 : (*job)->index + 1;
    (*job) = newJob;
    printf("[%d] %d\n", newJob->index, newJob->id);
}

void flush(process **head) {
    process *temp = *head, *prev = NULL;
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

void foreground(process **head, int index) {
    process *temp = *head, *prev = NULL;
    if (temp != NULL && temp->index == index) {
        *head = temp->next;
        free(temp);
        return;
    }
    while (temp != NULL) {
        while (temp != NULL && temp->index != index) {
            prev = temp;
            temp = temp->next;
        }
        if (temp == NULL) return;
        prev->next = temp->next;
        free(temp);
        return;
    }
}

void showJobs(process *jobs) {
    int i = 0;
    while (jobs != NULL) {
        i++;
        char *status = waitpid(jobs->id, NULL, WNOHANG) > 0 ? "Done" :
            waitpid(jobs->id, NULL, WNOHANG) == -1 ? "Error" : "Running";
        printf("\t[%d] (PID:%d) %s\t\t\t%s\n", jobs->index, jobs->id, status, jobs->command);
        jobs = jobs->next;
    }
}

pid_t getJob(process *jobs, int index) {
    while (jobs != NULL) {
        if (jobs->index == index) {
            return jobs->id;
        }
        jobs = jobs->next;
    }
    return -1;
}

