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



    return 0;
}

