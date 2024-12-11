#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8888
#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

int main() {
    int server_fd, new_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS, activity, i, valread, sd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    struct pollfd fds[MAX_CLIENTS + 1];

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

    // Accept the incoming connection
    int addrlen = sizeof(address);
    puts("Waiting for connections ...");

    // Initialize the pollfd structure
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    for (i = 1; i <= max_clients; i++) {
        fds[i].fd = -1;
    }

    while (1) {
        activity = poll(fds, max_clients + 1, -1);

        if (activity < 0) {
            perror("poll error");
            exit(EXIT_FAILURE);
        }

        // If something happened on the master socket, then it's an incoming connection
        if (fds[0].revents & POLLIN) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Inform user of socket number - used in send and receive commands
            printf("New connection, socket fd is %d, ip is : %s, port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Add new socket to array of sockets
            for (i = 1; i <= max_clients; i++) {
                if (fds[i].fd == -1) {
                    fds[i].fd = new_socket;
                    fds[i].events = POLLIN;

                    // Send client number to the client
                    char client_number[10];
                    sprintf(client_number, "%d", i);
                    send(new_socket, client_number, strlen(client_number), 0);

                    break;
                }
            }
        }

        // Else it's some IO operation on some other socket
        for (i = 1; i <= max_clients; i++) {
            sd = fds[i].fd;

            if (sd > 0 && (fds[i].revents & POLLIN)) {
                // Check if it was for closing, and also read the incoming message
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    // Somebody disconnected, get his details and print
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    fds[i].fd = -1;
                } else {
                    // Echo back the message that came in
                    buffer[valread] = '\0';
                    for (int j = 1; j <= max_clients; j++) {
                        if (fds[j].fd != -1 && fds[j].fd != sd) {
                            send(fds[j].fd, buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}