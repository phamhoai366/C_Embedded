all: client server

client: client.o
	gcc -o client client.o -lpthread

server: server.o
	gcc -o server server.o -lpthread

client.o: client.c
	gcc -c client.c

server.o: server.c
	gcc -c server.c

clean: 
	rm -f client server *.o