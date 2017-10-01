#include <stdlib.h>
#include <stdio.h>
#include <wait.h>
#include "management.h"
#include "../shell.h"

/**
 * Pushes a job onto a background jobs linked list.
 * @param job Pointer to the job process
 * @param PID The PID of the process
 * @param command The command name
 */
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

/**
 * Removes all zombie jobs from the background jobs linked list by iterating through them and finding which jobs
 * have had a change in status. By removing the completed jobs from the list, it also adjusts the remaining jobs such
 * that there are no empty spots in the list and the proper chain is kept through the background jobs linked list.
 * @param head Pointer to the head of the background jobs linked list.
 */
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

/**
 * Attempts to remove a specific job from the background jobs linked list and free the memory allocated to storing its
 * metadata. Reorganizes the linked list chain as well.
 * @param head Pointer to the head of the background jobs linked list.
 * @param index Index of the specific job in the list to remove.
 */
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

/**
 * Attempts to print a status report of the current background jobs. Provides information as to whether the job is still
 * running or has encountered a specific issue as well.
 * @param jobs The head of the background jobs linked list to iterate through.
 */
void showJobs(process *jobs) {
    int i = 0;
    while (jobs != NULL) {
        i++;
        char *status = waitpid(jobs->id, NULL, WNOHANG) == -1 ? "Error" : "Running";
        printf("[%d]\t\t(PID:%d) %s\t\t\t%s\n", jobs->index, jobs->id, status, jobs->command);
        jobs = jobs->next;
    }
}

/**
 * Attempts to find out which jobs have finished running and prints a report to STDOUT including the job index, pid,
 * and the completed status.
 * @param jobs The head of the background jobs linked list to iterate through.
 */
void showFinishedJobs(process *jobs) {
    int i = 0;
    while (jobs != NULL) {
        i++;
        if (waitpid(jobs->id, NULL, WNOHANG) > 0) {
            printf("[%d]\t\t(PID:%d) %s\t\t\t %s\n", jobs->index, jobs->id, "Done", jobs->command);
        }
        jobs = jobs->next;
    }
}

/**
 * Searches through the background jobs linked list to find the job with the given index.
 * @param jobs Head of the background jobs linked list to iterate through
 * @param index Index of the specific job the user is looking for
 * @return PID of the job with the given index
 */
pid_t getJob(process *jobs, int index) {
    while (jobs != NULL) {
        if (jobs->index == index) {
            return jobs->id;
        }
        jobs = jobs->next;
    }
    return -1;
}

/**
 * Utility function used to slow down the execution of a specific task. This must be called before making the desired
 * function call that you wish to slow down.
 */
void slowDown() {
    int w, rem;
    w = rand() % 10;
    rem = sleep((unsigned int) w);
    if (signal(SIGINT, (__sighandler_t) Handle_SIGINT) == SIG_ERR) {
        printf("ERROR: Could not bind signal handler for SIGINT. Exiting.\n");
        exit(0);
    }
    while(rem!= 0) rem = sleep(rem);
}




