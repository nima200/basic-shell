#ifndef ASSIGNMENT1_MANAGEMENT_H
#define ASSIGNMENT1_MANAGEMENT_H
#define MAX_PROCESS_COUNT 1000
#include <unistd.h>
typedef struct process {
    pid_t id;
    struct process* next;
    struct process* previous;
} process;
process* createNewJob();
void pushJob(struct process *job, pid_t PID);
void flush(struct process *job);
#endif //ASSIGNMENT1_MANAGEMENT_H
