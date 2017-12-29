#include "array.h"
#include "myShell.h"
#include <stdio.h>

int main(int argc, char** argv) { 
    
    while (1) {

        Array *line = readLine();
        Array *args = parseLine(line);
        runCommand(args);

        for (int i =0; i < len(args); i++) {
            char **arg = getElement(args, i);
            free(*arg);
            free(arg);
        }
        freeArray(args);
        freeArray(line);
    }
    return 0;
}

