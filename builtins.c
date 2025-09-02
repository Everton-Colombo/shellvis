#include <unistd.h>
#include <string.h>
#include <stdio.h>

void shellvis_cd(char** args) {
    if (chdir(args[1]) == 0) {
        printf("wordir changed to %s\n\n", args[1]);
        fflush(stdout);
    } else {
        perror("cd");
    }
}

void shellvis_pwd() {
    char buff[1024];
    char* result = getcwd(buff, 1024);

    if (result == NULL) {
        perror("pwd");
    } else {
        printf("%s\n", result);
        fflush(stdout);
    }
}

/**
 * Tries to find a builtin with name args[0] and execute it.
 * Returns 1 if a builtin was found, 0 otherwise.
 */
int call_builtin(char** args) {
    if (strcmp(args[0], "cd") == 0) {
        shellvis_cd(args);
    } else if (strcmp(args[0], "pwd") == 0) {
        shellvis_pwd();
    } else {
        return 0;
    }

    return 1;
}