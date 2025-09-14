#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "builtins.h"
#include "utils.h"
#include "execution.h"
#include "data.h"

#define MAX_LINE 1024

struct str_list g_path;

void shellvis_init() {
    TAILQ_INIT(&g_path);

    // Add bin/external_commands to PATH
    shellvis_execute((command_t) {
        .argc = 3,
        .args = (char*[]){"path", "bin/external_commands", "--shh", NULL},
        .istream = stdin,
        .ostream = stdout
    });

    // Add /bin to PATH
    shellvis_execute((command_t) {
        .argc = 3,
        .args = (char*[]){"path", "/bin", "--shh", NULL},
        .istream = stdin,
        .ostream = stdout
    });
}


void shellvis_terminate() {
    strlist_free(&g_path);
}

void shellvis_greetings() {
    fwrite(elvis_ascii, 1, elvis_ascii_len, stdout);

    printf("\n SHELLVIS v0.0.1\n\n");
    printf("This system is 100%% bug-free. Any observed anomalies are features, not errors.\nPlease report any new features to the administrator.\n\n");
}

int main(int argc, char* argv[]) {
    char line[MAX_LINE];
    char* args[MAX_LINE / 2 + 1];

    FILE* input_stream = stdin;

    shellvis_init();

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
        shellvis_greetings();
    }

    while (true) {
        if (input_stream == stdin) {
            printf("shellvis> ");
            fflush(stdout);
        }

        if (fgets(line, sizeof(line), input_stream) == NULL) {
            printf("\n");
            break; 
        }
        line[strcspn(line, "\n")] = '\0';

        size_t token_count = (size_t) split_string(line, " ", args, MAX_LINE / 2 + 1);
        
        command_batch_t batch = parse_command_batch(token_count, args);
        shellvis_execute_batch(batch);
        cleanup_command_batch(&batch);
    }

    shellvis_terminate();

    return 0;
}