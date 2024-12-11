#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <poll.h>

#define PORT 8888
#define BUFFER_SIZE 2048

void str_trim_lf(char* arr, int length) {
    for (int i = 0; i < length; i++) {
        if (arr[i] == '\n') {
            arr[i] = '\0';
            break;
        }
    }
}

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    struct pollfd fds[2];
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char client_number[10];

    // Socket settings
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    // Connect to Server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("ERROR: connect");
        return EXIT_FAILURE;
    }

    // Receive client number from server
    int receive = recv(sockfd, client_number, sizeof(client_number), 0);
    if (receive > 0) {
        client_number[receive] = '\0';
        printf("Your client number is %s\n", client_number);
    } else {
        perror("ERROR: recv client number");
        return EXIT_FAILURE;
    }

    printf("=== WELCOME TO THE CHATROOM ===\n");

    // Initialize the pollfd structure
    fds[0].fd = sockfd;
    fds[0].events = POLLIN;
    fds[1].fd = STDIN_FILENO;
    fds[1].events = POLLIN;

    while (1) {
        int activity = poll(fds, 2, -1);

        if (activity < 0) {
            perror("poll error");
            break;
        }

        if (fds[1].revents & POLLIN) {
            if (fgets(message, BUFFER_SIZE, stdin) != NULL) {
                str_trim_lf(message, BUFFER_SIZE);

                // Ensure the message_with_number buffer is large enough
                char message_with_number[BUFFER_SIZE + 20];
                int message_length = snprintf(message_with_number, sizeof(message_with_number), "Client %s: %s", client_number, message);

                if (message_length >= sizeof(message_with_number)) {
                    fprintf(stderr, "Warning: message truncated\n");
                }

                send(sockfd, message_with_number, strlen(message_with_number), 0);
                if (strcmp(message, "exit") == 0) {
                    break;
                }
            }
        }

        if (fds[0].revents & POLLIN) {
            int receive = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (receive > 0) {
                buffer[receive] = '\0';
                printf("%s\n", buffer);
            } else if (receive == 0) {
                printf("Server disconnected\n");
                break;
            } else {
                perror("recv error");
                break;
            }
        }
    }

    close(sockfd);
    return 0;
}