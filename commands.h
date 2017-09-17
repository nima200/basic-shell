#ifndef ASSIGNMENT1_HELPERS_H
#define ASSIGNMENT1_HELPERS_H
#define FOREACH_COMMAND(command) \
        command(ls), \
        command(cd), \
        command(cat), \
        command(cp), \
        command(fg), \
        command(jobs), \
        command(EXIT), \
        command(external) \

#define GENERATE_ENUM(ENUM) ENUM
#define GENERATE_STRING(STRING) #STRING

typedef enum { FOREACH_COMMAND(GENERATE_ENUM) } command;
static const char *COMMAND_STRING[] = {FOREACH_COMMAND(GENERATE_STRING)};
command checkCommand(char *command);
void listDirectory();
void concatenate(char *fileName);
void copy(char *source, char* destination);
__off_t fSize(const char *fileName);
#endif //ASSIGNMENT1_HELPERS_H
