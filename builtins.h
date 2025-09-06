#ifndef BULTINS_H
#define BULTINS_H

int shellvis_num_builtins();

extern char *builtin_names[];
extern int (*builtin_funcs[]) (char **);

#endif
