OBJS    = shell.o mymemory.o
SOURCE  = shell.c mymemory.c
HEADER  = mymemory.h
CC      = gcc

all: $(OBJS) 
	$(CC) -o result $(OBJS) && ./result

shell.o: shell.c
	$(CC) -c shell.c 

mymemory.o: mymemory.c
	$(CC) -c mymemory.c 
