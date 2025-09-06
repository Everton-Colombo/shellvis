#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "builtins.h"

int shellvis_cd(char** args) {
    if (chdir(args[1]) == 0) {
        printf("wordir changed to %s\n\n", args[1]);
        fflush(stdout);
        return 0;
    } else {
        perror("cd");
        return -1;
    }
}

int shellvis_pwd(char** args) {
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

int shellvis_path(char** args) {
   return 0; 
}

int shellvis_exit(char** args) {
    exit(EXIT_SUCCESS);
    return 0;
}

char *builtin_names[] = {
  "cd",
  "pwd",
  "path",
  "exit"
};

int (*builtin_funcs[]) (char **) = {
  &shellvis_cd,
  &shellvis_pwd,
  &shellvis_path,
  &exit
};

int shellvis_num_builtins() {
  return sizeof(builtin_names) / sizeof(char *);
}