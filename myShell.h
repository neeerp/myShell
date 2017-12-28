#ifndef MYSHELL_H_
#define MYSHELL_H_

/** Reads a line from stdin and returns an Array of characters */
Array *readLine();

/** Parses a line of input and returns an array of pointers to 
 * the individual arguments.
 *
 * Special characters:
 *  - Quoted arguments are treated as entire arguments
 *  - '|' characters (i.e. pipes) appear as arguments in the
 *    argument list.
 * */
Array *parseLine(Array *line);

/** Executes a command given an Array of commandline arguments.
 *
 *  Supports piping between commands.
 *  
 *  Terminates early if a command fails.
 *  */
void runCommand(Array *argsArray);

#endif
