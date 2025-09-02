#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "builtins.h"

#define MAX_LINE 1024

size_t split_string(char *input, const char *delimiters, char *tokens[], size_t max_tokens) {
    if (!input || !delimiters || !tokens || max_tokens == 0) {
        return 0;
    }
    
    size_t count = 0;
    char *saveptr = NULL;
    char *token = strtok_r(input, delimiters, &saveptr);
    
    while (token != NULL && count < max_tokens - 1) {
        tokens[count] = token;
        count++;
        token = strtok_r(NULL, delimiters, &saveptr);
    }
    
    // Null-terminate the array
    tokens[count] = NULL;
    return count;
}

void start_process(char** args, int is_detached) {
    pid_t pid;
    pid = fork();

    if (pid == 0) { // If is child process
        int result = execvp(args[0], args);
        if (result == -1) {
            printf("\"%s\": ", args[0]);
            fflush(stdout);
            perror("");
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

int main(void) {
    char line[MAX_LINE];
    char* args[MAX_LINE / 2 + 1];

    while (1) {
        printf("shellvis> ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            printf("\n");
            break; 
        }
        line[strcspn(line, "\n")] = 0;

        size_t token_count = (size_t) split_string(line, " ", args, MAX_LINE / 2 + 1);

        if (token_count > 0 && strcmp(args[0], "exit") == 0)
            break;        

        int builtin_called = call_builtin(args);
        if (!builtin_called) {
            int is_detached = args[token_count-1][0] == '&';
            start_process(args, is_detached);
        }
    }

    return 0;
}