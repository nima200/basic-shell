#ifndef ASSIGNMENT1_SHELL_H
#define ASSIGNMENT1_SHELL_H
int getCmd(char *prompt, char **args, int *background);
int parseCmd(char **args, char **line);
void executeCmd(char **args, const int *background, process **jobList);
void createChildProcess(char *stringCommand, char **params, const int *background, process **jobList);
#endif //ASSIGNMENT1_SHELL_H
