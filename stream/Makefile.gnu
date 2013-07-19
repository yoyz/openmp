# Makefile

SRC	= stream.c
OBJ	= stream.o
BIN	= stream


CC	= gcc
CFLAGS	=  -fopenmp -O3
LDLIBS	=

all: $(BIN)


BIN: $(BIN)
	$(CC) $(CFLAGS) $(OBJ) -o $(BIN)   $(LDLIBS)

OBJ: $(OBJ)
	$(CC) $(CFLAGS) $(SRC) -o $(OBJ)

clean:
	rm -f $(BIN) $(OBJ)

