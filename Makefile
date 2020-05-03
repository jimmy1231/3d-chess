CC=gcc
CFLAGS=-I.

INCLUDE_DIR=include
_INCLUDE=main.h
INCLUDE=$(patsubst %,$(INCLUDE_DIR)/%,$(_INCLUDE))

OBJ_DIR=obj
OBJ=main.o

$(OBJ_DIR)/%.o: %.c $(INCLUDE)
	$(CC) -c -o $@ $? $(CFLAGS)

# Generate executable
gcc-test: main.c
	$(CC) -o $@ $? $(CFLAGS)

# PHONY makes the target "clean" always out-of-date - so it is always run whenever
# "make clean" is called
.PHONY clean
clean:
	rm -f $(EXE) *.s *.o