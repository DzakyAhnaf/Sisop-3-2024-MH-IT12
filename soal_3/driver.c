#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 8080

int main(int argc, char* argv[]) {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    // Create client socket
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        printf("Error creating socket!\n");
        return 1;
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(PORT);

    // Connect to server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Error connecting to server!\n");
        return 1;
    }

    // Read command and information from command line arguments
    char* command = argv[1];
    char* info = NULL;
    if (argc > 2) {
        info = argv[2];
    }

    // Send command and information to server
    memset(buffer, 0, sizeof(buffer));
    if (info != NULL) {
        snprintf(buffer, sizeof(buffer), "%s %s", command, info);
    } else {
        snprintf(buffer, sizeof(buffer), "%s", command);
    }
    send(clientSocket, buffer, strlen(buffer), 0);

    // Receive response from server
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesReceived < 0) {
        printf("Error receiving data!\n");
        return 1;
    }

    /* Print response
    time_t currentTime = time(NULL);
    struct tm* localTime = localtime(&currentTime);
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%d/%m/%Y %H:%M:%S", localTime);

    printf("[Driver] [%s]: [%s] [%s]\n", timeString, command, info ? info : "");
    printf("[Paddock] [%s]: [%s] [%s]\n", timeString, command, buffer);

    */
    
    printf("Response: %s\n", buffer);
    
    
    // Close client socket
    close(clientSocket);

    return 0;
}
