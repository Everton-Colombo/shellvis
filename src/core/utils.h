#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <sys/queue.h>

struct str_listnode {
    char* str;

    TAILQ_ENTRY(str_listnode) links;
};
TAILQ_HEAD(str_list, str_listnode);

void strlist_free(struct str_list* strlist);
void strlist_append(struct str_list* strlist, char* str);
void strlist_fprint(struct str_list* strlist, FILE* stream);

size_t split_string(char *input, const char *delimiters, char *tokens[], size_t max_tokens);

int strarray_pop(char** strarray, size_t array_size, int i);

#endif // UTILS_H