CC=gcc
CFLAGS= -g -Wall

SRCS=clox_main.c scanner.c values.c vm.c memory.c compile.c chunk.c debug.c object.c
OBJS=$(SRCS:.c=.o)
MAIN=clox

all:  $(MAIN)

$(MAIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(MAIN) $(OBJS) 

clean:
	$(RM) *.o
