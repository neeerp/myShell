#ifndef BUILTINS_H_
#define BUILTINS_H_

int num_builtins();
int echo(int argc, char **args);

extern char *builtins[];

extern int (*builtin_functions[]) (int, char **);

#endif
