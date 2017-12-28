#include "array.h"
#include "myShell.h"
#include <stdio.h>

int main(int argc, char** argv) { 
    Array *line = readLine();
    Array *args = parseLine(line);

    printf("Our args are... \n");
    for (int i = 0; i < len(args); i++) {
        char **arg = getElement(args, i);
        printf("%d. %s\n", i, *arg);
        free(arg);
    }
    printf("\nIf we got here, then wooo, we did it!\n");
   
    for (int i =0; i < len(args); i++) {
        char **arg = getElement(args, i);
        free(*arg);
        free(arg);
    }
    freeArray(args);
    freeArray(line);
    return 0;
}

