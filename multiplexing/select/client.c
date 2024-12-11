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
    fd_set readfds;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    char client_number[10];

    // Socket settings
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd == -1) {
        perror("ERROR: socket");
        return EXIT_FAILURE;
    }
    
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

    // printf("=== WELCOME TO THE CHATROOM ===\n");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(sockfd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int activity = select(sockfd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0) {
            perror("select error");
            break;
        }

        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(message, BUFFER_SIZE, stdin) != NULL) {
                // str_trim_lf(message, BUFFER_SIZE);

                char message_with_number[BUFFER_SIZE + 20];
                int msg_len = snprintf(message_with_number, sizeof(message_with_number), "Client %s: %s", client_number, message);

                send(sockfd, message_with_number, strlen(message_with_number), 0);
                if (strcmp(message, "exit") == 0) {
                    break;
                }
            }
        }

        if (FD_ISSET(sockfd, &readfds)) {
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