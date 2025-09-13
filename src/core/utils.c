#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utils.h"

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

void strlist_free(struct str_list* strlist) {
    struct str_listnode *node = TAILQ_FIRST(strlist);
    while (node != NULL) {
        struct str_listnode *next_node = TAILQ_NEXT(node, links);
        free(node->str);
        free(node);
        node = next_node;
    }
}

void strlist_append(struct str_list* strlist, char* str) {
    struct str_listnode* new_node = malloc(sizeof(struct str_listnode));
    if (!new_node) return;

    new_node->str = strdup(str);
    if (!new_node->str) {
        free(new_node);
        return;
    }
    TAILQ_INSERT_TAIL(strlist, new_node, links);
}

void strlist_fprint(struct str_list* strlist, FILE* stream) {
    if (stream == NULL)
        stream = stdout;

    int is_empty = 1;
    struct str_listnode *node;
    TAILQ_FOREACH(node, strlist, links) {
        is_empty = 0;
        fprintf(stream, "- %s\n", node->str);
    }

    if (is_empty)
        fprintf(stream, "(empty)\n");
    fprintf(stream, "\n");
}

int strarray_pop(char** strarray, size_t array_size, int i) {
    if (!strarray || i < 0 || i >= array_size || !strarray[i]) {
        return -1;
    }

    free(strarray[i]);
    strarray[i] = NULL;

    // Shift elements to the left to fill the gap
    for (int j = i; j < array_size - 1; j++) {
        strarray[j] = strarray[j + 1];
    }
    strarray[array_size - 1] = NULL;

    return 0;
}