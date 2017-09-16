#ifndef ASSIGNMENT1_SHELL_H
#define ASSIGNMENT1_SHELL_H
int getCmd(char *prompt, char **args, int *background);
int parseCmd(char **args, char **line);
void executeCmd(char **args, const int *background);
void createChildProcess(const command *typeOfCommand, char **params, const int *background);
#endif //ASSIGNMENT1_SHELL_H
