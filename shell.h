#ifndef ASSIGNMENT1_SHELL_H
#define ASSIGNMENT1_SHELL_H
int getCmd(char *prompt, char **args, int *background, int *redirect);
int parseCmd(char **args, char **line);
void executeCmd(char **args, const int *background, const int *redirect, process **jobList);
static void Handle_SIGINT();
static void Handle_SIGTSTP();
static void Handle_SIGNLINE();
void
createChildProcess(char *stringCommand, char **params, const int *background, const int *redirect, process **jobList);
#endif //ASSIGNMENT1_SHELL_H
