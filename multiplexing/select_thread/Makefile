

all: client server

client: client.c
	gcc client.c -o client -lpthread

server: server.c
	gcc server.c -o server -lpthread

clean: 
	rm -f client server


# default port 9999
# ./server [port]
# ./client [name] [ip host] [port]