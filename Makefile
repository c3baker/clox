CC=gcc
CFLAGS= -g -Wall

SRCS=clox_main.c scanner.c values.c vm.c clox_memory.c compile.c chunk.c debug.c
OBJS=$(SRCS:.c=.o)
MAIN=clox

all:  $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) 

clean:
	$(RM) *.o
