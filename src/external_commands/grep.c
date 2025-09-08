#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE 1024

int main(int argc, char* argv[]) {
    int show_line_numbers = 0;
    char* search_string;
    char* filename;
    
    if (argc == 3) {
        search_string = argv[1];
        filename = argv[2];
    } else if (argc == 4 && strcmp(argv[1], "-n") == 0) {
        show_line_numbers = 1;
        search_string = argv[2];
        filename = argv[3];
    } else {
        fprintf(stderr, "Usage: %s [-n] <string> <file>\n", argv[0]);
        return -1;
    }

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror(filename);
        return -1;
    }

    char buffer[MAX_LINE];
    int line_number = 0;

    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        line_number++;
        if (strstr(buffer, search_string) != NULL) {
            if (show_line_numbers) {
                printf("%d: %s", line_number, buffer);
            } else {
                printf("%s", buffer);
            }
        }
    }

    fclose(file);
    return 0;
}