OBJS    = shell.o filesys.o
SOURCE  = shell.c filesys.c
HEADER  = filesys.h
CC      = gcc

all: $(OBJS) 
	$(CC) -o result $(OBJS) && ./result

shell.o: shell.c
	$(CC) -c shell.c 

mymemory.o: filesys.c
	$(CC) -c filesys.c 