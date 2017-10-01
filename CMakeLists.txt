cmake_minimum_required(VERSION 3.5)
project(assignment1)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES shell.c commands.c commands.h shell.h process/management.h process/management.c)
add_executable(assignment1 ${SOURCE_FILES})