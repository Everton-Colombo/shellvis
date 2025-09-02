#include <unistd.h>
#include <string.h>
#include <stdio.h>

void shellvis_cd(char** args) {
    if (chdir(args[1]) == 0) {
        printf("wordir changed to %s\n\n", args[1]);
    } else {
        perror("cd");
    }
}

/**
 * Tries to find a builtin with name args[0] and execute it.
 * Returns 1 if a builtin was found, 0 otherwise.
 */
int call_builtin(char** args) {
    if (strcmp(args[0], "cd") == 0) {
        shellvis_cd(args);
        return 1;
    }

    return 0;
}