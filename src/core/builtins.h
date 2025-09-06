#ifndef BULTINS_H
#define BULTINS_H

#include "utils.h"

extern struct str_list g_path;

void builtins_init();
void builtins_terminate();
int shellvis_num_builtins();

extern char *builtin_names[];
extern int (*builtin_funcs[]) (int, char**);

#endif
