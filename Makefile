CC = gcc
CFLAGS = -Wall -std=gnu99 -g
DEPS = array.h myShell.h builtIns.h
OBJ = array.o myShell.o builtIns.o

main: $(DEPS) $(OBJ) main.c
	$(CC) $(CFLAGS) -o $@ $(OBJ) main.c

array.o: array.h array.c
	$(CC) $(CFLAGS) -c array.c

builtIns.o: builtIns.h builtIns.c
	$(CC) $(CFLAGS) -c builtIns.c

myShell.o: $(DEPS) myShell.c
	$(CC) $(CFLAGS) -c myShell.c


