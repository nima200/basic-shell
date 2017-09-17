#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <wait.h>
#include <sys/stat.h>
#include <syscall.h>
#include "process/management.h"
#include "commands.h"
#include "shell.h"

int main(void) {
    char **args = malloc(20 * sizeof(char*));
    struct process* jobs = createNewJob();
    int background;
    // TODO: Implement CWD prompt
    while (1) {
        background = 0;
        int argCount = getCmd("\u03BB\t", args, &background);
        executeCmd(args, &background, jobs);

    }
}
/**
 * Prints a prompt message to the user, takes user's input, and finally calls a parser to extract the
 * arguments sent inside an array. If after parsing it is recognized that the user wants to run the
 * command in background, the background param is set to 1; else, it is set to 0.
 * @note                    The returned value includes the command to be executed + the number of flags
 * @param prompt            The prompt message to be printed to the user
 * @param args              Pointer to the array to store the extracted arguments in
 * @param background        Flag to set to 0 or 1 based on whether it was detected that the command is fg or bg
 * @return                  The number of arguments in user's command
 */
int getCmd(char *prompt, char **args, int *background) {
    char *loc;
    char *line = NULL;
    size_t lineSize = strlen(prompt);
    size_t length = 0;
    // Allocate enough memory to the buffer for storing the prompt
    line = malloc(lineSize * sizeof(char));
    // Handle out of memory
    if (line == NULL) {
        perror("OUT OF MEMORY: Unable to allocate enough memory to buffer. Exiting");
        exit(1);
    }
    // Show prompt to user and take in input

    printf("%s(PID: %d) >\t", prompt, getpid());
    length = (size_t) getline(&line, &lineSize, stdin);
    // Should never happen
    if (length <= 0) {
        perror("A fatal error has occurred. Exiting.");
        exit(1);
    }
    // Check for background task flag: '&'
    if ((loc = index(line, '&')) != NULL) {
        *background = 1;
        *loc = ' ';
    } else {
        *background = 0;
    }
    int argCount = parseCmd(args, &line);
    // De-allocate the memory space previously allocated to the line buffer
    free(line);
    // Return the number of arguments in the command
    return argCount;
}

/**
 * Parses a command line to a set of arguments, looking for 'SPACE' or other non-text ASCII characters as
 * the delimiter for argument separation.
 * @note                    The returned value includes the command to be executed + the number flags
 * @param args              Pointer to the array to store the extracted arguments in
 * @param line              Pointer to the character array that contains the command line
 * @return                  The number of arguments in the user's command
 */
int parseCmd(char **args, char **line) {
    char *token;
    int i = 0;
    while ((token = strsep(line, " \t\n")) != NULL) {
        // Find the first non standard ASCII Text chars and/or 'SPACE' and end the token there
        for (int j = 0; j < strlen(token); j++) {
            if (token[j] <= 32) { // 32 = ASCII SPACE
                token[j] = '\0';
            }
        }
        // If the token is not empty, add to set of args and increment i for the next token
        if (strlen(token) > 0) {
            args[i++] = token;
        }
    }
    return i;
}

void createChildProcess(const command *typeOfCommand, char **params, const int *background, process *jobs) {
    pid_t PID = fork();
    switch (PID) {
        case 0:
            switch (*typeOfCommand) {
                    case LS:{
                        listDirectory();
                        exit(0);
                    }
                    case CD:break;

                    case CAT:
                        concatenate(params[0]);
                        exit(0);
                    case CP:
                        copy(params[0], params[1]);
                        exit(0);
                    case FG:break;
                    case JOBS:break;
                    case EXIT:break;
                    case EXTERNAL:break;
                }
            break;
        default:
            switch(*background) {
                case 0:
                    // Parent waits for child to complete
                    waitpid(PID, NULL, 0);
                    break;
                default:
                    pushJob(jobs, PID);
                    // Parent does nothing and continues running
                    printf("hi");
                    return;
            }
            break;
    }


}

void executeCmd(char **args, const int *background, process *jobs) {
    // Find whether the command is a system call 'internal' command or to be executed externally
    command command = checkCommand(args[0]);
    switch (command) {
        case LS:
        case CAT:
        case CP:
            createChildProcess(&command, args + 1, background, jobs);
            break;
        case CD: {
            if (args[1] == NULL) {
                chdir(getenv("HOME"));
                return;
            }
            if (chdir(args[1]) == -1) {
                printf("ERROR: Invalid directory: %s\n", args[1]);
            }
            break;
        }
        case FG:
            break;
        case JOBS:
            break;
        case EXIT:
            free(args);
            exit(0);
        case EXTERNAL:
            break;
    }
}

