CC  := gcc
CFLAGS  := -lpthread

all: client server

client: client.c 
	$(CC) -o client client.c $(CFLAGS)

server: server.c
	$(CC) -o server server.c $(CFLAGS)

.PHONY: clean
clean:
	rm -f client server