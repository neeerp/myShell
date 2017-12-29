#include "builtIns.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



/** Print the given arguments to stdout */
int echo(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }

    return 0;
}

/** Terminate myShell */
int exit_myShell(int argc, char **argv) {
    exit(0);
}

/** Change the working directory */
int cd(int argc, char **argv) {
    if (argc < 2) {
        chdir(getenv("HOME"));
        return 0;
    } else if (argc > 2) {
        fprintf(stderr, "myShell: cd: too many arguments");
    } else if (chdir(argv[1])) {
        perror("myShell: cd:");
    }

    return 0;
}



/** Globals for builtin access **/
char *builtins[] = {
    "echo",
    "exit",
    "cd"
};


int (*builtin_functions[]) (int, char **) = {
    echo,
    exit_myShell,
    cd
};

/** Return the number of builtin functions */
int num_builtins() {
    return sizeof(builtins)/sizeof(char *);
}

