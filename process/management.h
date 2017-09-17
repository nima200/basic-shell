#ifndef ASSIGNMENT1_MANAGEMENT_H
#define ASSIGNMENT1_MANAGEMENT_H

#include <unistd.h>
#include "../commands.h"

typedef struct process {
    pid_t id;
    const char* command;
    struct process* next;
//    struct process* previous;
} process;
process* createNewJob();
void pushJob(struct process **job, pid_t PID, const char *command);
void flush(struct process **head);
void showJobs(struct process *job);
#endif //ASSIGNMENT1_MANAGEMENT_H
