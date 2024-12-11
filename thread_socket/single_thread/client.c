#include<stdio.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>


# define port 5000
# define ipaddr "127.0.0.1"

void chat(int clisocket){
    char buffer[100];
    while(1){
        printf("\nTo server:");
        fgets(buffer, sizeof(buffer), stdin);
        send(clisocket, buffer, sizeof(buffer), 0);
        recv(clisocket, buffer, sizeof(buffer), 0);
        printf("\tFrom server:%s", buffer);
        if(strcmp(buffer, "bye\n") == 0)
            break;
    }
}

int main(){
    int clisocket;
    struct sockaddr_in serveraddr;
    clisocket = socket(AF_INET, SOCK_STREAM, 0);

    if(clisocket > 0){
        printf("client socket created\n"); 
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = inet_addr(ipaddr);

    int status = connect(clisocket, (struct sockaddr*)&serveraddr, sizeof(serveraddr));
    if(status < 0){
        perror("Couldn't connect to the server");
        exit(EXIT_FAILURE);
    }

    chat(clisocket);
    close(clisocket);

    return 0;
}