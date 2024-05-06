#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int main() {
    int clientsock;
    struct sockaddr_in servAddr;
    char buffer[1024];
    char response[1024]; 

    clientsock = socket(AF_INET, SOCK_STREAM, 0);
    if (clientsock < 0) {
        perror("Error creating socket");
        return 1;
    }

    memset(&servAddr, '0', sizeof(servAddr));

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(8081);
    if(inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr) <= 0) {
        printf("\nInvalid address/address not supported \n");
        return 1;
    }

    if (connect(clientsock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0) {
        perror("Error connecting to server");
        return 1;
    }

    printf("Connected to server\n");

    while (1) {
        printf("You: ");
        fgets(buffer, 1024, stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; 

        if (strcmp(buffer, "exit") == 0) {
            send(clientsock, buffer, strlen(buffer), 0); 
            break;
        }

        send(clientsock, buffer, strlen(buffer), 0);

        int bytes_received = recv(clientsock, response, 1024, 0);
        if (bytes_received < 0) {
            perror("recv failed");
            break;
        } else if (bytes_received == 0) {
            printf("Server closed the connection.\n");
            break;
        }

        response[bytes_received] = '\0';

        printf("Server response: %s\n", response);
    }

    close(clientsock);
    return 0;
}
