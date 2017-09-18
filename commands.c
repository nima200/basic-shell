#include <memory.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "commands.h"

#define CWD "./"
command checkCommand(char *command) {
    if (strcmp(command, COMMAND_STRING[cd]) == 0) return cd;
    if (strcmp(command, COMMAND_STRING[ls]) == 0) return ls;
    if (strcmp(command, COMMAND_STRING[cat]) == 0) return cat;
    if (strcmp(command, COMMAND_STRING[cp]) == 0) return cp;
    if (strcmp(command, COMMAND_STRING[fg]) == 0) return fg;
    if (strcmp(command, COMMAND_STRING[jobs]) == 0) return jobs;
    if (strcmp(command, "exit") == 0) return EXIT;
    return external;
}

void listDirectory() {
    DIR *dir;
    struct dirent *entry;
    dir = opendir(CWD);
    if (dir != NULL) {

        while ((entry = readdir(dir))) {
            printf("\t\t%s\n", entry->d_name);
        }
        closedir(dir);
    } else {
        printf("ERROR: Could not open the directory\n");
    }
}

void concatenate(char* fileName) {
    int fileDesc;
    char *buffer;
    fileDesc = open(fileName, O_RDONLY, S_IRUSR);
    if (fileDesc == -1) {
        printf("ERROR: Invalid file\n");
        return;
    }
    __off_t fileSize;
    if ((fileSize = fSize(fileName)) != -1) {
        buffer = malloc((size_t) fileSize);
        if (buffer == NULL) {
            printf("ERROR: Could not allocate enough memory to the buffer\n");
            return;
        }
        while (read(fileDesc, buffer, (size_t) fileSize) > 0) {
            write(1, buffer, (size_t) fileSize);
        }
        if (read(fileDesc, buffer, (size_t) fileSize) < 0) {
            printf("ERROR: Something went wrong while reading the file.\n");
        } else {
            close(fileDesc);
            free(buffer);
        }
    } else {
        printf("ERROR: Could not determine file size\n");
    }
}

void copy(char* source, char *destination) {
    int sourceDesc, targetDesc;
    char *buffer;
    sourceDesc = open(source, O_RDONLY, S_IRUSR);
    targetDesc = open(destination, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (sourceDesc == -1) {
        printf("ERROR: Invalid source file\n");
        return;
    }
    if (targetDesc == -1) {
        printf("ERROR: Invalid target file\n");
        return;
    }
    __off_t sourceSize;
    if ((sourceSize = fSize(source)) != -1) {
        buffer = malloc((size_t) sourceSize);
        if (buffer == NULL) {
            printf("ERROR: Could not allocate enough memory to buffer\n");
            return;
        }
        while (read(sourceDesc, buffer, (size_t) sourceSize) > 0) {
            write(targetDesc, buffer, (size_t) sourceSize);
        }
        if (read(sourceDesc, buffer, (size_t) sourceSize) < 0) {
            printf("ERROR: Something went wrong while reading the file.\n");
        } else {
            close(sourceDesc);
            close(targetDesc);
            free(buffer);
        }
    } else {
        printf("ERROR: Could not determine file size.\n");
    }
}

__off_t fSize(const char *fileName) {
    struct stat st;
    return stat(fileName, &st) == 0 ? st.st_size : -1;
}