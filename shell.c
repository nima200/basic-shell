#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

int getcmd(char *prompt, char *args[], int *background);

int main(void) {
    char *args[20];
    int bg;
    getcmd("\n>>\t", args, &bg);
//    while (1) {
//        bg = 0;
//        int cnt = getcmd("\n>>", args, &bg);
//    }
}

int getcmd(char *prompt, char *args[], int *background) {
    int i = 0;
    char *token, *loc;
    char *line = NULL;
    size_t lineSize = strlen(prompt);
    size_t length = 0;
    // Allocate enough memory to the buffer for storing the prompt
    line = (char *) malloc(lineSize * sizeof(char));
    // Handle out of memory
    if (line == NULL) {
        perror("OUT OF MEMORY: Unable to allocate enough memory to buffer. Exiting");
        exit(1);
    }
    // Show prompt to user and take in input
    printf("%s", prompt);
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

    while ((token = strsep(&line, " \t\n")) != NULL) {
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
    // Return the number of arguments in the command
    return i;
}

