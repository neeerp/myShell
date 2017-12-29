#include "array.h"
#include "myShell.h"
#include "builtIns.h"
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void parseArg(Array *args, const char *line, int *i);
int runCommandHelper(int argc, char** argv, int *prev, int *curr);
int execCommand(int argc, char **argv, int *prev, int *curr);
void closeHelper(int *prev, int *curr);

/** Reads a line of text from the command line and returns an
 * Array of characters */
Array *readLine() {
    Array *line;
    init(&line, 1, 128);
    int quoted = 0;

    char cwd[512];
    getcwd(cwd, 512);
    printf("%s@myShell:%s$ ", getenv("USER"), cwd);
    while(1) {
        char c = getchar();
        if (c == EOF || c == '\n') {
            break;
        } else if (c == '"') { // Keep track of open quotes
            quoted = !quoted;
        }
        append(line, &c);
    }
    if (quoted) { // Close people's quotes for them should they forget
        append(line, "\"");
    }
    append(line, "\0");
    return line;
}

/** Parses a command line and returns a vector of arguments.
 *  Assumes that <line> is an Array of characters, and
 *  returns an Array of character pointers. */
Array *parseLine(Array *line) {
    char *lineChars = array(line);

    Array *args;
    init(&args, sizeof(char *), 8);
    int i = 0;
    int length = len(line);
    while (i < length - 1) {
        switch (lineChars[i]) {
            case '|': ;// Keep track of pipes
                char *pipe = malloc(2);
                pipe[0] = '|';
                pipe[1] = '\0';
                append(args, &pipe);
            case ' ': // Skip spaces
                i++;
                break;
            default: // This must be an entire argument
                parseArg(args, lineChars, &i);
        }
    }
    free(lineChars);
    return args;
}


/** Read the argument starting at the ith index of the given array and store 
 * it in the arguments array */
void parseArg(Array *args, const char *line, int *i) {
    if (e_size(args) != sizeof(char *)) {
        fprintf(stderr, "Invalid argument array!");
        exit(1);
    }
    
    Array *arg;
    init(&arg, 1, 16);
    int quoted = 0;
    while (1) {
        char c = line[*i];
        if (quoted) { // Read entire quoted argument
            if (c == '"') { // End quotes
                quoted = 0;
            } else {
                append(arg, &c);
            }
        } else {
            switch (c) {
                case '"': // Begin quotes
                    quoted = 1;
                    break;
                case '\0': // End of argument
                case ' ':
                case '|':
                    append(arg, "\0");
                    // Store the arg's address
                    char *argStr = array(arg);
                    append(args, &argStr);
                    freeArray(arg);
                    return;
                default:
                    append(arg, &c);
            }
        }
        (*i)++;
    }
}

/** Executes a command given a vector of commandline arguments.
 *  Assumes argsArray is an Array of char **'s.
 *  Supports piping between programs. */
void runCommand(Array *argsArray) {
    char **args = array(argsArray);
    int length = len(argsArray);
    
    // Pipes
    int *curr = NULL;
    int *prev = NULL;

    int i = 0;
    int start = 0;
    while (i <= length) { 
        if (i == length || !strcmp(args[i], "|")) { // End of one program's args
            // Shift pipe pointers
            if (prev) {
                free(prev);
            }
            prev = curr;
            if (i != length) { // This triggers and breaks prev...
                curr = malloc(sizeof(int *) * 2);
                pipe(curr);
            } else {
                curr = NULL;
            }

            int status = (runCommandHelper(i - start, args + start, prev, curr));
            if (status) { // Abort if a command fails!
                break;
            }
            start = i + 1;
        }
        i++;
    }

    // Clean up
    if (prev) {
        free(prev);
    }
    if (curr) {
        free(curr);
    }
    free(args);
    printf("\n");
}

/** Find out whether the command is a builtin and run it, otherwise
 * attempt to execute the command.
 * 
 * Return the exit status of the command if executed, or success status
 * of builtins. */
int runCommandHelper(int argc, char** argv, int *prev, int *curr) {
    // Run a builtin function
    int initial_stdout = dup(fileno(stdout));
    for (int i = 0; i < num_builtins(); i++) {
        if (!strcmp(argv[0], builtins[i])) {
            if (curr) { // redirect stdout to pipe
                dup2(curr[1], fileno(stdout));
                close(curr[1]);
            }
            // Run the builtin
            int status = builtin_functions[i](argc, argv);
            // Revert stdout from pipe to what it was initially
            if (curr) {
                fflush(stdout);
                dup2(initial_stdout, fileno(stdout));
            }
            return status;
        }
    }
    return execCommand(argc, argv, prev, curr);
}

/** Fork a process to execute a command and wait on it to terminate,
 * returning its exit status (or 1 if the command could not be executed */
int execCommand(int argc, char **argv, int *prev, int *curr) {    
    if (!access(argv[0], X_OK)) {
        int n = fork();
        if (n == 0) { // Child calling execvp
            if (prev) { // If receiving input from pipe
                dup2(prev[0], fileno(stdin));
                close(prev[0]);
            }

            if (curr) { // If piping to another process
                dup2(curr[1], fileno(stdout));
                close(curr[1]);
            }

            // Need to add a sentinel to the args
            char **args = malloc(sizeof(char *) * (argc + 1));
            memcpy(args, argv, sizeof(char *) * argc);
            args[argc] = NULL;

            execvp(args[0], args);
            perror("execvp");
            exit(1);
        } else if (n > 0) { // Waiting parent
            // Close pipes
            closeHelper(prev, curr);

            // Wait on child
            int status;
            wait(&status);
            if (WIFEXITED(status)) {
                return WEXITSTATUS(status);
            } else {
                return 1;
            }
        } else { // Fork failed
            perror("fork");
            closeHelper(prev, curr);
            return 1;
        }

    } else { // command does not exist
        printf("myShell: %s: command not found.", argv[0]);
        return 1;
    }
}

/** Helper that closes the read end of prev and the write end of curr,
 * should they exist. */
void closeHelper(int *prev, int *curr) {
    if (prev) {
        close(prev[0]);
    }

    if (curr) {
        close(curr[1]);
    }
}

