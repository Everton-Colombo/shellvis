#ifndef EXECUTION_H
#define EXECUTION_H

#include <stdio.h>
#include <stdbool.h>
#include "utils.h"

extern struct str_list g_path;

typedef struct {
    int argc;
    char** args;
    FILE* istream;
    FILE* ostream;
    bool is_detached; 
} command_t;

typedef struct {
    command_t* commands;
    int command_count;
} command_batch_t;

int shellvis_execute(command_t parsed_command);
command_t parse_command(int argc, char** args);
void cleanup_command(command_t* cmd);

int shellvis_execute_batch(command_batch_t batch);
void cleanup_command_batch(command_batch_t* batch);
command_batch_t parse_command_batch(int argc, char** args);

#endif