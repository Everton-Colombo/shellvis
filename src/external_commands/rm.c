#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: rm <file>\n");
        return 0;
    }
    
    if (remove(argv[1]) == 0) {
        printf("Deleted \"%s\".\n", argv[1]);
        return 0;
    } else {
        perror("Error deleting file");
        return -1;
    }
}