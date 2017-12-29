#ifndef BUILTINS_H_
#define BUILTINS_H_

int num_builtins();

extern char *builtins[];

extern int (*builtin_functions[]) (int, char **);

#endif
