#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>


#define PORT 8888
#define MAX_CLIENTS 100
#define BUFFER_SIZE 2048

int main() {
    int server_fd, new_socket, client_socket[MAX_CLIENTS], max_clients = MAX_CLIENTS, activity, i, valread, sd;
    int max_sd;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    fd_set readfds;

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

    while (1) {
        // Clear the socket set
        FD_ZERO(&readfds);

        // Add master socket to set
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        // Add child sockets to set
        for (i = 0; i < max_clients; i++) {
            // Socket descriptor
            sd = client_socket[i];

            // If valid socket descriptor then add to read list
            if (sd > 0)
                FD_SET(sd, &readfds);

            // Highest file descriptor number, need it for the select function
            if (sd > max_sd)
                max_sd = sd;
        }

        // Wait for an activity on one of the sockets, timeout is NULL, so wait indefinitely
        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("select error");
        }

        // If something happened on the master socket, then it's an incoming connection
        if (FD_ISSET(server_fd, &readfds)) {
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            // Inform user of socket number - used in send and receive commands
            printf("New connection, socket fd is %d, ip is : %s, port : %d\n", new_socket, inet_ntoa(address.sin_addr), ntohs(address.sin_port));

            // Add new socket to array of sockets
            for (i = 0; i < max_clients; i++) {
                // If position is empty
                if (client_socket[i] == 0) {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n", i);

                    // Send client number to the client
                    char client_number[10];
                    sprintf(client_number, "%d", i);
                    send(new_socket, client_number, strlen(client_number), 0);

                    break;
                }
            }
        }

        // Else it's some IO operation on some other socket
        for (i = 0; i < max_clients; i++) {
            sd = client_socket[i];

            if (FD_ISSET(sd, &readfds)) {
                // Check if it was for closing, and also read the incoming message
                if ((valread = read(sd, buffer, BUFFER_SIZE)) == 0) {
                    // Somebody disconnected, get his details and print
                    getpeername(sd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
                    printf("Host disconnected, ip %s, port %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

                    // Close the socket and mark as 0 in list for reuse
                    close(sd);
                    client_socket[i] = 0;
                } else {
                    // Echo back the message that came in
                    buffer[valread] = '\0';
                    printf("%s\n", buffer);
                    for (int j = 0; j < max_clients; j++) {
                        if (client_socket[j] != 0 && client_socket[j] != sd) {
                            send(client_socket[j], buffer, strlen(buffer), 0);
                        }
                    }
                }
            }
        }
    }

    return 0;
}