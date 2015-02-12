CC=gcc
CFLAGS=-Wall -g -c
LFLAGS=-Wall -g

all: shell 

shell: shell.o
	$(CC) $(LFLAGS) -o $@ $^

shell.o: shell.c 
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f *.o shell
