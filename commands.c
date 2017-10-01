#include <memory.h>
#include <dirent.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include "commands.h"

#define CWD "./"
/**
 * Attempts to check a character array forming a command name against a set of built in commands, and if no match is found,
 * identifies the command as an external command which is to be sent over to the unix system to run through execvp()
 * @param command The character array representing the command
 * @return An enum type that associates with the type of command that was entered. 'External' if not recognized.
 */
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

/**
 * Lists the content of the current working directory by writing to STDOUT.
 */
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

/**
 * Attempts to read through a file given a character array of the file name, and content onto STDOUT.
 * @param fileName The character array containing the filename.
 */
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

/**
 * Copies the content of a source file into a destination file. First attempts to open the files, and if successful,
 * a buffer is allocated to the source file to for reading its content, and simultaneously as reading each byte of the
 * source file, the content is being written to the destination file (which is created if it does not exist already).
 * If the destination file exists, the content is replaced by the content read from the source file.
 * @param source Character array containing the source filename
 * @param destination Character array containing the destination filename
 */
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
/**
 * Utility function used to calculate the size of a file
 * @param fileName Character array containing the filename to analyze
 * @return Size of the file
 */
__off_t fSize(const char *fileName) {
    struct stat st;
    return stat(fileName, &st) == 0 ? st.st_size : -1;
}