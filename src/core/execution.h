#ifndef EXECUTION_H
#define EXECUTION_H

#include <stdio.h>
#include "utils.h"

extern struct str_list g_path;

typedef struct {
    int argc;
    char** args;
    FILE* istream;
    FILE* ostream;
} command_t;

int shellvis_execute(command_t parsed_command);
command_t parse_command(int argc, char** args);
void cleanup_command(command_t* cmd);

#endif