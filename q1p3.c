#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int q1p3() {
    char output[] = "A simple program output.\n";
    int fileDesc = open("redirect.txt", O_CREAT | O_WRONLY | O_APPEND, S_IRUSR | S_IWUSR);
    write(fileDesc, output, strlen(output));
    return 0;
}