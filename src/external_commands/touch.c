#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: touch <file>\n");
        return 0;
    }

    FILE* file = fopen(argv[1], "w");
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    fclose(file);

    return 0;
}