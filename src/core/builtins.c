#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "builtins.h"
#include "utils.h"
#include "execution.h"

int shellvis_cd(command_t cmd) {
    if (cmd.argc < 2) {
        fprintf(cmd.ostream, "cd: missing argument\n");
        return -1;
    }

    if (chdir(cmd.args[1]) == 0) {
        fprintf(cmd.ostream, "workdir changed to %s\n\n", cmd.args[1]);
        fflush(cmd.ostream);
        return 0;
    } else {
        perror("cd");
        return -1;
    }
}

int shellvis_pwd(command_t cmd) {
    char buff[1024];
    char* result = getcwd(buff, 1024);

    if (result == NULL) {
        perror("pwd");
        return -1;
    } else {
        fprintf(cmd.ostream, "%s\n", result);
        fflush(cmd.ostream);
        return 0;
    }
}

int shellvis_path(command_t cmd) { 
    int silent_mode = 0;
    if (cmd.argc == 3 && strcmp("--shh", cmd.args[2]) == 0)
        silent_mode = 1;

    for (int i = 1; i < cmd.argc; i++) {
        // Skip the --shh flag
        if (strcmp("--shh", cmd.args[i]) == 0)
            continue;

        char full_path[1024];
        if (access(cmd.args[i], F_OK) == 0 && realpath(cmd.args[i], full_path) != NULL) {
            strlist_append(&g_path, full_path);
            if (!silent_mode)
                fprintf(cmd.ostream, "Added \"%s\" to internal PATH.\n", full_path);
        } else {
            if (!silent_mode)
                fprintf(cmd.ostream, "Could not access \"%s\". Not added to internal PATH.\n", cmd.args[i]);
        }
    }

    if (!silent_mode) {
        fprintf(cmd.ostream, "\n");
        fprintf(cmd.ostream, "Internal PATH list:\n");
        strlist_fprint(&g_path, cmd.ostream);
    }

    return 0; 
}

int shellvis_exit(command_t cmd) {
    exit(EXIT_SUCCESS);
    return 0;
}

char *builtin_names[] = {
  "cd",
  "pwd",
  "path",
  "exit"
};

int (*builtin_funcs[]) (command_t) = {
  &shellvis_cd,
  &shellvis_pwd,
  &shellvis_path,
  &shellvis_exit
};

int shellvis_num_builtins() {
  return sizeof(builtin_names) / sizeof(char*);
}