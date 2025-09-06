#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "builtins.h"
#include "utils.h"

int shellvis_cd(int argc, char** args) {
    if (chdir(args[1]) == 0) {
        printf("wordir changed to %s\n\n", args[1]);
        fflush(stdout);
        return 0;
    } else {
        perror("cd");
        return -1;
    }
}

int shellvis_pwd(int argc, char** args) {
    char buff[1024];
    char* result = getcwd(buff, 1024);

    if (result == NULL) {
        perror("pwd");
        return -1;
    } else {
        printf("%s\n", result);
        fflush(stdout);
        return 0;
    }
}

int shellvis_path(int argc, char** args) {
    for (int i = 1; i < argc; i++) {
        char full_path[1024];
        if (access(args[i], F_OK) == 0 && realpath(args[1], full_path) != NULL) {
            strlist_append(&g_path, full_path);
            printf("Added \"%s\" to internal PATH.\n", full_path);
        } else {
            printf("Could not access \"%s\". Not added to internal PATH.\n", args[i]);
        }
    }

    printf("\n");

    printf("Internal PATH list:\n");
    strlist_print(&g_path);

    return 0; 
}

int shellvis_exit(int argc, char** args) {
    exit(EXIT_SUCCESS);
    return 0;
}

char *builtin_names[] = {
  "cd",
  "pwd",
  "path",
  "exit"
};

int (*builtin_funcs[]) (int, char**) = {
  &shellvis_cd,
  &shellvis_pwd,
  &shellvis_path,
  &exit
};

int shellvis_num_builtins() {
  return sizeof(builtin_names) / sizeof(char*);
}