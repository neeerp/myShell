#include "builtIns.h"
#include <stdio.h>

char *builtins[] = {
    "echo"
};

int (*builtin_functions[]) (int, char **) = {
    &echo
};

/** Return the number of builtin functions */
int num_builtins() {
    return sizeof(builtins)/sizeof(char *);
}

/** Print the given arguments to stdout */
int echo(int argc, char **args) {
    for (int i = 1; i < argc; i++) {
        printf("%s ", args[i]);
    }

    return 0;
}
