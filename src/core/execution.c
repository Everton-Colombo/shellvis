#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#include "execution.h"
#include "builtins.h"
#include "utils.h"

void child_process_routine(command_t command, bool search_path) {
    char final_path[1024];
    bool found_executable = false;

    // Handle input redirection:
    if (command.istream != NULL && command.istream != stdin) {
        int input_fd = fileno(command.istream);
        if (dup2(input_fd, STDIN_FILENO) == -1) {
            perror("dup2 input");
            exit(EXIT_FAILURE);
        }
        fclose(command.istream);
    }
    // Handle output redirection
    if (command.ostream != NULL && command.ostream != stdout) {
        int output_fd = fileno(command.ostream);
        if (dup2(output_fd, STDOUT_FILENO) == -1) {
            perror("dup2 output");
            exit(EXIT_FAILURE);
        }
        fclose(command.ostream);
    }

    // Find executable
    if (search_path) {  // will be false when executing a command with './'
        struct str_listnode* node;
        TAILQ_FOREACH(node, &g_path, links) {
            snprintf(final_path, sizeof(final_path), "%s/%s", node->str, command.args[0]);
            if (access(final_path, X_OK) == 0) {
                found_executable = true;
                break;
            }
        }
        
        // If not found in g_path, try system PATH
        if (!found_executable) {
            execvp(command.args[0], command.args); // This will handle system PATH
            perror(command.args[0]);
            exit(EXIT_FAILURE);
        }
    } else {
        // Executing command with './'
        strcpy(final_path, command.args[0]);
        found_executable = access(final_path, X_OK) == 0;
    }
    
    if (found_executable) {
        execv(final_path, command.args);
        perror(command.args[0]);
    } else {
        printf("Executable \"%s\" not found.\n", command.args[0]);
    }
    
    exit(EXIT_FAILURE);
}

void start_process(command_t command, bool search_path) {
    pid_t pid;
    pid = fork();

    if (pid == 0) { // If is child process
        child_process_routine(command, search_path);

    } else if (pid > 0) {   // If is parent process and succeeded
        if (command.is_detached) {
            printf("PID: %d\n", pid);
        } else {
            int status;
            wait(&status);
        }
    } else {
        printf("Could not create process.\n");
    }
}

int shellvis_execute(command_t parsed_command) {
    if (parsed_command.argc == 0)
        return 1;

    // Searches for builtin commands
    for (int i = 0; i < shellvis_num_builtins(); i++) {
        if (strcmp(parsed_command.args[0], builtin_names[i]) == 0) {
            return (*builtin_funcs[i])(parsed_command);
        }
    }

    // If no builtin command was found, execute external command
    start_process(parsed_command, strncmp("./", parsed_command.args[0], 2) != 0);
    return 0;
}

command_t parse_command(int argc, char** args) {
    command_t parsed_command = {
        .argc = 0,
        .args = NULL,
        .istream = stdin,
        .ostream = stdout,
        .is_detached = false
    };

    // Creating new args array, clean of any redirects
    char** new_args = malloc((argc + 1) * sizeof(char*));
    int new_argc = 0;

    for (int i = 0; i < argc; i++) {
        if (strcmp(">", args[i]) == 0) {
            // Output redirection
            if (i + 1 < argc) {
                parsed_command.ostream = fopen(args[i + 1], "w");
                if (parsed_command.ostream == NULL) {
                    perror(args[i + 1]);
                    parsed_command.ostream = stdout;
                    printf("[shellvis]: Couldn't set output stream. Defaulting to stdout.\n");
                }
                i++; // Skip filename
            } else {
                printf("[shellvis]: Missing filename after '>'\n");
            }
        } else if (strcmp("<", args[i]) == 0) {
            // Input redirection
            if (i + 1 < argc) {
                parsed_command.istream = fopen(args[i + 1], "r");
                if (parsed_command.istream == NULL) {
                    perror(args[i + 1]);
                    parsed_command.istream = stdin;
                    printf("[shellvis]: Couldn't set input stream. Defaulting to stdin.\n");
                }
                i++; // Skip filename
            } else {
                printf("[shellvis]: Missing filename after '<'\n");
            }
        } else if (strcmp("&", args[i]) == 0) {
            // Detached execution marker
            parsed_command.is_detached = true;
        } else {
            // Regular argument
            new_args[new_argc] = args[i];
            new_argc++;
        }
    }

    new_args[new_argc] = NULL;
    parsed_command.argc = new_argc;
    parsed_command.args = new_args;

    return parsed_command;
}

void cleanup_command(command_t* cmd) {
    if (cmd->istream && cmd->istream != stdin) {
        fclose(cmd->istream);
    }
    if (cmd->ostream && cmd->ostream != stdout) {
        fclose(cmd->ostream);
    }

    if (cmd->args) {
        free(cmd->args);
        cmd->args = NULL;
    }
}

command_batch_t parse_command_batch(int argc, char** args) {
    command_batch_t batch = {0};
    
    // Count commands by counting '&' separators followed by other tokens
    int command_count = argc > 0 ? 1 : 0;
    for (int i = 0; i < argc; i++) {
        if (strcmp("&", args[i]) == 0 && i+1 != argc) {
            command_count++;
        }
    }
    
    batch.commands = malloc(command_count * sizeof(command_t));
    batch.command_count = command_count;
    
    int current_command = 0;
    int start_idx = 0;
    
    for (int i = 0; i < argc; i++) {
        if (i+1 == argc || strcmp("&", args[i]) == 0) {
            int cmd_argc = i+1 - start_idx;
            char** cmd_args = &args[start_idx];
            
            batch.commands[current_command] = parse_command(cmd_argc, cmd_args);
            batch.commands[current_command].is_detached = strcmp("&", args[i]) == 0;
            
            current_command++;
            start_idx = i + 1;
        }
    }
    
    return batch;
}

int shellvis_execute_batch(command_batch_t batch) {
    for (int i = 0; i < batch.command_count; i++) {
        command_t cmd = batch.commands[i];
        
        if (cmd.argc == 0) continue;
        
        // Check for builtin commands
        bool is_builtin = false;
        for (int j = 0; j < shellvis_num_builtins(); j++) {
            if (strcmp(cmd.args[0], builtin_names[j]) == 0) {
                (*builtin_funcs[j])(cmd);
                is_builtin = true;
                break;
            }
        }
        
        // Execute external command if not builtin
        if (!is_builtin) {
            start_process(cmd, strncmp("./", cmd.args[0], 2) != 0);
        }
    }
    
    return 0;
}

void cleanup_command_batch(command_batch_t* batch) {
    for (int i = 0; i < batch->command_count; i++) {
        cleanup_command(&(batch->commands[i]));
    }
    if (batch->commands) {
        free(batch->commands);
        batch->commands = NULL;
    }
    batch->command_count = 0;
}