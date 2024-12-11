#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define PORT 8888
#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048
#define MAX_EVENTS 10

int main() {
    int server_fd, new_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS, activity, i, valread, sd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    struct epoll_event ev, events[MAX_EVENTS];
    int epoll_fd;

    // Initialize all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++) {
        client_socket[i] = 0;
    }

    // Create a master socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Set master socket to allow multiple connections
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to localhost port 8888
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Try to specify maximum of 3 pending connections for the master socket
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Add server_fd to epoll instance
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl: server_fd");
        exit(EXIT_FAILURE);
    }

    // Accept the incoming connection
    int addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while (1) {
        activity = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);

        if (activity < 0) {
            perror("epoll_wait");
            exit(EXIT_FAILURE);
        }

        for (i = 0; i < activity; i++) {
            if (events[i].data.fd == server_fd) {
                if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                    perror("accept");
                    exit(EXIT_FAILURE);
                }

                // Inform user of socket number - used in send and receive commands
                printf("New connection, socket fd is %d, ip is : %s, port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                // Add new socket to epoll instance
                ev.events = EPOLLIN;
                ev.data.fd = new_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1) {
                    perror("epoll_ctl: new_socket");
                    exit(EXIT_FAILURE);
                }

                // Send client number to the client
                char client_number[11];
                snprintf(client_number, sizeof(client_number), "%d", new_socket);
                send(new_socket, client_number, strlen(client_number), 0);
            } else {
                sd = events[i].data.fd;

                // Check if it was for closing, and also read the incoming message
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    // Somebody disconnected, get his details and print
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    // Close the socket and remove from epoll instance
                    close(sd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, sd, NULL);
                } else {
                    // Echo back the message that came in
                    buffer[valread] = '\0';
                    for (int j = 0; j < max_clients; j++) {
                        if (client_socket[j] != 0 && client_socket[j] != sd) {
                            send(client_socket[j], buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}