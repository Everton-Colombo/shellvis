#include <stdlib.h>
#include <string.h>

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
