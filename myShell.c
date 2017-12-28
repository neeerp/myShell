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

/** Reads a line of text from the command line and returns an
 * Array of characters */
Array *readLine() {
    Array *line;
    init(&line, 1, 128);
    int quoted = 0;
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
    
    // Keep track of the original stdout for when we pipe
    int original_stdout = dup(fileno(stdout));
    
    // Pipes
    int *curr = NULL;
    int *prev = NULL;

    int i = 0;
    int start = 0;
    while (i <= length) {
        if (i == length || strcmp(args[i], "|")) { // End of one program's args
            // Shift pipe pointers
            if (prev) {
                free(prev);
            }
            prev = curr;
            if (!strcmp(args[i], "|")) {
                curr = malloc(sizeof(int *) * 2);
                pipe(curr);
            } else {
                curr = NULL;
            }

            int status = (runCommandHelper(i - start, args + start, prev, curr));
            dup2(fileno(stdout), original_stdout);
            if (status) { // Abort if a command fails!
                break;
            }
            start = i + 1;
        }
    }
    close(original_stdout);
    free(args);
}

/** Find out whether the command is a builtin and run it, otherwise
 * attempt to execute the command.
 * 
 * Return the exit status of the command if executed, or success status
 * of builtins. */
int runCommandHelper(int argc, char** argv, int *prev, int *curr) {
    // Run a builtin function
    for (int i = 0; i < num_builtins(); i++) {
        if (!strcmp(argv[0], builtins[i])) {
            if (curr) { // Write to pipe
                dup2(curr[1], fileno(stdout));
                close(curr[1]);
            }
            return builtin_functions[i](argc, argv);
        }
    }

    return execCommand(argc, argv, prev, curr);
}

/** Fork a process to execute a command and wait on it to terminate,
 * returning its exit status (or 1 if the command could not be executed */
int execCommand(int argc, char **argv, int *prev, int *curr) {
    if (access(argv[0], X_OK)) {
        int n = fork();
        if (n == 0) { // Child calling execvp
            if (prev) { // If receiving input from pipe
                dup2(prev[0], fileno(stdin));
                close(prev[0]);
            }

            if (curr) { // If piping to another process
                dup2(curr[1], fileno(stdin));
                close(curr[1]);
            }
            execvp(argv[0], argv);
            perror("execvp");
            exit(1);
        } else if (n > 0) { // Waiting parent
            // Close pipes
            if (prev) {
                close(prev[0]);
            }

            if (curr) {
                close(curr[1]);
            }

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
            return 1;
        }
    } else { // command does not exist
        printf("myShell: %s: command not found.", argv[0]);
        return 1;
    }
}

// https://unix.stackexchange.com/questions/266794/does-the-shell-fork-when-i-use-built-in-commands
