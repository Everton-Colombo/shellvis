#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "builtins.h"
#include "utils.h"

#define MAX_LINE 1024

struct str_list g_path;

void init() {
    TAILQ_INIT(&g_path);

    // Add bin/external_commands to PATH
    shellvis_execute(2, (char*[]){"path", "bin/external_commands", NULL});
}


void terminate() {
    strlist_free(&g_path);
}

void greetings() {
    FILE *file;
    char line[MAX_LINE];
    
    file = fopen("data/elvis.txt", "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    
    while (fgets(line, sizeof(line), file) != NULL) {
        printf("%s", line);
    }
    
    fclose(file);


    printf("\n\t SHELLVIS v0.0.1\n\n");
    printf("This system is 100%% bug-free. Any observed anomalies are features, not errors.\nPlease report any new features to the administrator.\n\n");
}

void start_process(char** args, int is_detached) {
    pid_t pid;
    pid = fork();

    if (pid == 0) { // If is child process
        char full_path[MAX_LINE];
        int found_executable = 0;

        struct str_listnode* node;
        TAILQ_FOREACH(node, &g_path, links) {
            snprintf(full_path, sizeof(full_path), "%s/%s", node->str, args[0]);
            if (access(full_path, X_OK) == 0) {
                found_executable = 1;
                break;
            }
        }
        
        if (found_executable) {
            int result = execvp(full_path, args);
            if (result == -1) {
                printf("\"%s\": ", args[0]);
                fflush(stdout);
                perror("");
            }
        } else {
            printf("Executable \"%s\" not found. Use the \'path\' command to add directories to the internal PATH list.\n", args[0]);
        }

    } else if (pid > 0) {   // If is parent process and succeeded
        if (!is_detached) {
            int status;
            wait(&status);
        }
    } else {
        printf("Could not create process.\n");
    }
}

int shellvis_execute(int argc, char** args) {
    if (argc == 0)
        return 1;     

    // Searches for builtin commands
    for (int i = 0; i < shellvis_num_builtins(); i++) {
        if (strcmp(args[0], builtin_names[i]) == 0) {
            return (*builtin_funcs[i])(argc, args);
        }
    }

    // If no builtin command was found, execute external command
    start_process(args, 0);
    return 0;
}


int main(int argc, char* argv[]) {
    char line[MAX_LINE];
    char* args[MAX_LINE / 2 + 1];

    FILE* input_stream = stdin;

    init();

    if (argc > 1) {
        input_stream = fopen(argv[1], "r");
        if (input_stream == NULL) {
            perror("Error opening file");
            input_stream = stdin;
        } else {
            printf("[shellvis]: Operating in batch mode. Input file: %s\n", argv[1]);
            fflush(stdout);
        }
    } else {
        greetings();
    }

    while (1) {
        if (input_stream == stdin) {
            printf("shellvis> ");
            fflush(stdout);
        }

        if (fgets(line, sizeof(line), input_stream) == NULL) {
            printf("\n");
            break; 
        }
        line[strcspn(line, "\n")] = 0;

        size_t token_count = (size_t) split_string(line, " ", args, MAX_LINE / 2 + 1);
        shellvis_execute(token_count, args);
    }

    terminate();

    return 0;
}