CC  := gcc
CFLAGS  := -lpthread

all: server client

server: server.c
	$(CC) server.c -o server 

client: client.c
	$(CC) client.c -o client 

clean:
	rm -f server client