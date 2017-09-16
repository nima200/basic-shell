#ifndef ASSIGNMENT1_HELPERS_H
#define ASSIGNMENT1_HELPERS_H
typedef enum {LS, CD, CAT, CP, FG, JOBS, EXIT, EXTERNAL} command;
command checkCommand(char *command);
void listDirectory();
void concatenate(char *fileName);
void copy(char *source, char* destination);
__off_t fSize(const char *fileName);
#endif //ASSIGNMENT1_HELPERS_H
