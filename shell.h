#ifndef ASSIGNMENT1_SHELL_H
#define ASSIGNMENT1_SHELL_H
int getCmd(char *prompt, char **args, int *background);
int parseCmd(char **args, char **line);
void executeCmd(char **args, const int *background, struct process *jobs);
void createChildProcess(const command *typeOfCommand, char **params, const int *background, struct process *jobs);
#endif //ASSIGNMENT1_SHELL_H
