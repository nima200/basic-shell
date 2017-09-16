#include <memory.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "commands.h"
#define READ_MODE "r"
command checkCommand(char *command) {
    if (strcmp(command, "cd") == 0) return CD;
    if (strcmp(command, "ls") == 0) return LS;
    if (strcmp(command, "cat") == 0) return CAT;
    if (strcmp(command, "cp") == 0) return CP;
    if (strcmp(command, "fg") == 0) return FG;
    if (strcmp(command, "jobs") == 0) return JOBS;
    if (strcmp(command, "exit") == 0) return EXIT;
    return EXTERNAL;
}

void listDirectory() {
    DIR *dir;
    struct dirent *entry;
    dir = opendir("./");
    if (dir != NULL) {
        while ((entry = readdir(dir))) {
            printf("\t\t%s\n", entry->d_name);
        }
        closedir(dir);
    } else {
        perror("ERROR: Could not open the directory\n");
    }
}

void concatenate(char* fileName) {
    FILE *filePtr;
    char character;
    filePtr = fopen(fileName, READ_MODE);
    if (filePtr == NULL) {
        printf("ERROR: Cannot open file %s\n", fileName);
        return;
    }
    character = (char) fgetc(filePtr);
    while (character != EOF) {
        printf("%c", character);
        character = (char) fgetc(filePtr);
    }
    fclose(filePtr);
}

void copy(char* source, char *destination) {
    int sourceDesc, targetDesc;
    char *buffer;
    sourceDesc = open(source, O_RDONLY, S_IRUSR);
    targetDesc = open(destination, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (sourceDesc == -1) {
        perror("ERROR: Invalid source file");
        return;
    }
    if (targetDesc == -1) {
        perror("ERROR: Invalid target file");
        return;
    }
    __off_t sourceSize;
    if ((sourceSize = fSize(source)) != -1) {
        buffer = malloc((size_t) sourceSize);
        while (read(sourceDesc, buffer, (size_t) sourceSize) > 0) {
            write(targetDesc, buffer, (size_t) sourceSize);
        }
        if (read(sourceDesc, buffer, (size_t) sourceSize) < 0) {
           perror("ERROR: Something went wrong while reading the file.");
        } else {
            close(sourceDesc);
            close(targetDesc);
            free(buffer);
        }
    }
}

__off_t fSize(const char *fileName) {
    struct stat st;
    return stat(fileName, &st) == 0 ? st.st_size : -1;
}