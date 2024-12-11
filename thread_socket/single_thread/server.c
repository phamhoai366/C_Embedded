#include<stdio.h>
#include<arpa/inet.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>

# define port 5000


void chat(int newsocket){
    char buffer[100];
    while(1){
        recv(newsocket, buffer, sizeof(buffer), 0);
        printf("\nFrom client:%s",buffer);
        if(strcmp(buffer, "bye\n") == 0)
            break;
        printf("\tTo client:");
        fgets(buffer, sizeof(buffer), stdin);
        send(newsocket, buffer, sizeof(buffer), 0);
    }
}

int main(){
    struct sockaddr_in serveraddr, newaddr;
    int sersocket, newsocket;
    sersocket = socket(AF_INET, SOCK_STREAM, 0);
    if(sersocket > 0)
        printf("\nserver socket is created");

    if(setsockopt(sersocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &(int){1}, sizeof(int)) < 0){
        perror("setsockopt(SO_REUSEADDR) failed");
    }
    
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    int s = bind(sersocket, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if(s == 0){
        printf("\nbind success");
    }
        
    listen(sersocket, 3);
    int size = sizeof(newaddr);
    printf("\nserver ready");

    newsocket = accept(sersocket, (struct sockaddr *)&newaddr, &size);   
    if(newsocket > 0){
        printf("\naccepted\n");
    }
        
    chat(newsocket);
    close(newsocket);

    return 0;
}
