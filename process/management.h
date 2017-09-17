#ifndef ASSIGNMENT1_MANAGEMENT_H
#define ASSIGNMENT1_MANAGEMENT_H

#include <unistd.h>
#include "../commands.h"

typedef struct process {
    pid_t id;
    const char* command;
    struct process* next;
    int index;
//    struct process* previous;
} process;
void pushJob(struct process **job, pid_t PID, const char *command);
void flush(struct process **head);
void showJobs(struct process *jobs);
pid_t getJob(process *jobs, int index);
void foreground(process **head, int index);
#endif //ASSIGNMENT1_MANAGEMENT_H
