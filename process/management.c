#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include "management.h"

void pushJob(process **job, pid_t PID, const char *command) {
    process* newJob = (process*) malloc(sizeof(process));
    if (newJob == NULL) {
        printf("ERROR: Could not allocate enough memory to the job\n");
        return;
    }
    newJob->id = PID;
    newJob->command = command;
    newJob->next = (*job);
    newJob->index = *job == NULL ? 1 : (*job)->index + 1;
    (*job) = newJob;
    printf("[%d] %d\n", newJob->index, newJob->id);
}

// TODO: Implement killing the process actually.
void flush(process **head) {
    process *temp = *head, *prev = NULL;
    // Case that child is still running
    while (temp != NULL && (waitpid(temp->id, NULL, WNOHANG)) != 0) {
        *head = temp->next;
        free(temp);
        temp = *head;
    }
    while (temp != NULL) {
        // Case that we visit zombie child(ren)
        while (temp != NULL && (waitpid(temp->id, NULL, WNOHANG)) == 0) {
            kill(temp->id, SIGKILL);
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
        char *status = waitpid(jobs->id, NULL, WNOHANG) == -1 ? "Error" : "Running";
        printf("\t[%d] (PID:%d) %s\t\t\t%s\n", jobs->index, jobs->id, status, jobs->command);
        jobs = jobs->next;
    }
}

void showFinishedJobs(process *jobs) {
    int i = 0;
    while (jobs != NULL) {
        i++;
        if (waitpid(jobs->id, NULL, WNOHANG) > 0) {
            printf("\t[%d] (PID:%d) %s\t\t\t %s\n", jobs->index, jobs->id, "Done", jobs->command);
        }
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

