#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

int q1p3() {
    int fileDesc = open("redirect.txt", O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    dup2(fileDesc, STDOUT_FILENO);
    printf("A simple program output.\n");
    return 0;
}