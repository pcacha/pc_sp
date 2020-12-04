CC = gcc
CFLAGS = -Wall -pedantic -ansi
BIN = fsmgen.exe
OBJ = err_manager.o memory_observer.o hash_table.o loader.o main.o

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

$(BIN): $(OBJ)
	$(CC) $^ -o $@
