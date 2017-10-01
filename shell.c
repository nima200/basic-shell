#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include "process/management.h"
#include "shell.h"

struct process* jobsList = NULL;
int main(void) {
    time_t now;
    srand((unsigned int) (time(&now)));
    int background;
    int redirect;
    if (signal(SIGINT, (__sighandler_t) Handle_SIGINT) == SIG_ERR) {
        printf("ERROR: Could not bind signal handler for SIGINT. Exiting.\n");
        exit(0);
    }
    if (signal(SIGTSTP, (__sighandler_t) Handle_SIGTSTP) == SIG_ERR) {
        printf("ERROR: Could not bind signal handler for SIGTSTP. Exiting.\n");
        exit(0);
    }
    if (signal(SIGUSR2, (__sighandler_t) Handle_SIGNLINE) == SIG_ERR) {
        printf("ERROR: Could not bind signal handler for SIGNLINE. Exiting.\n");
        exit(0);
    }
    // TODO: Implement CWD prompt
    while (1) {
        char *args[20];
        initNullArr(args, 20);
        background = 0;
        int argCount = getCmd("\u03BB ", args, &background, &redirect);
        raise(SIGUSR2); // SIGUSER2 == New command entered
        executeCmd(args, &background, &redirect, &jobsList);
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
int getCmd(char *prompt, char **args, int *background, int *redirect) {
    char *ampLoc, *redirectLoc;
    size_t lineSize = strlen(prompt);
    char *line = NULL;
    __ssize_t length = 0;
    // Show prompt to user and take in input

    printf("%s(PID: %d) > ", prompt, getpid());
    length = getline(&line, &lineSize, stdin);
    // Should never happen
    if (length <= 0) {
        perror("A fatal error has occurred. Exiting.");
        exit(1);
    }
    // Check for background task flag: '&'
    if ((ampLoc = index(line, '&')) != NULL) {
        *background = 1;
        *ampLoc = ' ';
    } else {
        *background = 0;
    }
    // Output redirection
    if ((redirectLoc = index(line, '>')) != NULL) {
        *redirect = 1;
        *redirectLoc = ' ';
    } else {
        *redirect = 0;
    }
    char *line_copy = line;
    int argCount = parseCmd(args, &line_copy);
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

/**
 * Creates a child process
 * @param stringCommand The character array that represents the command
 * @param params The parameters of the command
 * @param background Background flag
 * @param redirect Redirection flag
 * @param jobList Pointer to the head of the jobs linked list
 */
void createChildProcess(char *stringCommand, char **params, const int *background, const int *redirect, process **jobList) {
    pid_t PID = fork();
    const command command= checkCommand(stringCommand);
    // Redirection handling
    switch (PID) {
        case 0:
            /* IMPORTANT NOTE: As this is required per the assignment extension specifications, it must be mentioned that
             * such a slowdown is also creating delays for specific time-constraint commands such as "sleep 5s" which renders
             * the results as inaccurate because the outcome of such would now be sleep 5s + (random seconds caused by slowDown()) */
            slowDown();
            if (*redirect == 1) {
                int fd = open(params[1] != NULL ? params[1] : params[0], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
                dup2(fd, STDOUT_FILENO);
            }
            switch (command) {
                    case ls:
                    {
                        // If any flags are passed to ls, pass the command to UNIX and allow the OS to take care of its
                        // execution as opposed to using the internal implementation since there are many different
                        // flags that this command supports.
                        if (params[1] != NULL) {
                            execvp(stringCommand, params);
                            exit(0);
                        } else {
                            listDirectory();
                            exit(0);
                        }
                    }
                    // CD is handled in the parent program, hence not forked.
                    case cd:break;
                    case cat:
                    {
                        concatenate(params[0]);
                        exit(0);
                    }
                    case cp:
                    {
                        copy(params[0], params[1]);
                        exit(0);
                    }
                    // FG, JOBS, and EXIT are all handled by a parent program, hence not forked.
                    case fg:break;
                    case jobs:break;
                    case EXIT:break;
                    // Pass external commands to the OS to execute, using execvp()
                    case external:
                        execvp(stringCommand, params);
                        exit(0);
                }
            break;
        default:
            switch(*background) {
                case 0:
                    // Parent waits for child to complete
                    waitpid(PID, NULL, 0);
                    break;
                default:
                    // If the job is to be sent to background, the parent a reference to it onto the jobs list and continues.
                    pushJob(jobList, PID, stringCommand);
                    return;
            }
            break;
    }


}

/**
 * Executes a given command
 * @param args The arguments to pass to the command (this includes the command name itself as well in args[0])
 * @param background Flag to indicate if command is to be run in the background
 * @param redirect Flag to indicate if output is to be redirected
 * @param jobList Pointer to the head of the jobs linked-list
 */
void executeCmd(char **args, const int *background, const int *redirect, process **jobList) {
    // Find out whether the command is a system call 'internal' command or to be executed externally
    if (args[0] == NULL)
        return;
    command command = checkCommand(args[0]);
    switch (command) {
        case ls:
            createChildProcess(args[0], args, background, redirect, jobList);
            break;
        case cat:
        case cp:
            createChildProcess(args[0], args + 1, background, redirect, jobList);
            break;
        case cd: {
            // If no args were passed to cd, take the user to Home. (Default UNIX behavior)
            if (args[1] == NULL) {
                chdir(getenv("HOME"));
                return;
            }
            // Attempt to change the current working directory to that of the first argument
            // If the attempt fails because of invalid dir, return.
            if (chdir(args[1]) == -1) {
                printf("ERROR: Invalid directory: %s\n", args[1]);
                return;
            }
            break;
        }
        case fg:{
            // Only invoke functionality if job index was provided
            if (args[1] == NULL) {
                return;
            } else {
                // Find the job with the given index
                pid_t pid = getJob(*jobList, atoi(args[1]));
                if (pid == -1) {
                    printf("ERROR: Invalid job index. No job found with index %s\n", args[1]);
                } else {
                    // If the index was valid, take it out of the list of background jobs and wait on it
                    foreground(jobList, atoi(args[1]));
                    waitpid(pid, NULL, 0);
                    flush(jobList);
                }
                return;
            }
        }
        case jobs:{
            // Copy the STDOUT file descriptor to reopen it later
            int out = dup(STDOUT_FILENO);
            // If redirection flag was set, attempt to open the file into which redirection was requested, and replace the
            // file descriptor of STDOUT with that of the file. Once the command is executed, re-open STDOUT by copying back
            // its descriptor in its original place.
            if (*redirect == 1) {
                int fd = open(args[2] != NULL ? args[2] : args[1], O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
                dup2(fd, STDOUT_FILENO);
                showJobs(*jobList);
                flush(jobList);
                dup2(out, STDOUT_FILENO);
                close(out);
            } else {
                flush(jobList);
                showJobs(*jobList);
            }
            break;
        }
        case EXIT:
            exit(0);
        case external:
            createChildProcess(args[0], args, background, redirect, jobList);
            break;
    }
}


/**
 * Handle stop signals differently: do nothing.
 */
void Handle_SIGTSTP() {}

/**
 * Handler for user defined signal that rises every time a new-line symbol is passed as input. If there are any background
 * jobs that the system was running, show the report of their completion. This is an attempt at mimicing the same feature
 * in Ubuntu systems.
 */
void Handle_SIGNLINE() {
    showFinishedJobs(jobsList);
}
/**
 * Signal handler function to take care interrupt signals.
 */
void Handle_SIGINT() {
    printf("\n");
    exit(0);
}

/**
 * Initialize a character array by setting in null to all its values.
 * @param arr Pointer to the array
 * @param size Size of the array
 */
void initNullArr(char *arr[], int size) {
    for (int i = 0; i < size; ++i) {
        arr[i] = NULL;
    }
}


