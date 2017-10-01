This assignment has been written using C99. Since I have made attempts to make more reusable and modular code, I
have tried to decouple some of the functions from the main shell.c file into other files, specifically the management.c
and commands.c file. Each C file also comes with a required header file which includes type definitions and explicit function
definitions of those methods that other files need to get access to or even be used internally.
As a result, CMake 3.5 was used to bundle the project together and although a fully compiled and packaged version of the
program is included inside the assignment submission, to re-compile this code it is important that you run

> cmake .
> make

inside the root directory of the assignment, for the compiler and linker to work together to package the multiple files.
Also, please note that I have made attempts to implement as many of the required commands internally within my program
as I could, which means that more effort has been made but the outcome might result in a constraint capacity of the command.
An example of this is the 'ls' command, in which I have implemented a function for carrying out its functionality, but
have also included a branch for execution such that if extra flags such as '-l' or '-a' or any other have been passed to
the command, the OS runs it externally using execvp().

The required file for CMake to know what to do (called CMakeLists.txt) has also been included in the project directory.