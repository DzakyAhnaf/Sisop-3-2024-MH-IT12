#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "actions.c"

#define PORT 8080

void logMessage(char* source, char* command, char* info) {
    FILE* logFile = fopen("race.log", "a");
    if (logFile == NULL) {
        printf("Error opening file!\n");
        return;
    }

    time_t currentTime = time(NULL);
    struct tm* localTime = localtime(&currentTime);
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%d/%m/%Y %H:%M:%S", localTime);

    fprintf(logFile, "[%s] [%s]: [%s] [%s]\n", source, timeString, command, info);
    fclose(logFile);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen;
    char buffer[1024];

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        printf("Error creating socket!\n");
        return 1;
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket to server address
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("Error binding socket!\n");
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        printf("Error listening on socket!\n");
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        // Accept incoming connection
        clientAddrLen = sizeof(clientAddr);
        clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            printf("Error accepting connection!\n");
            continue;
        }

        // Receive command from client
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0) {
            printf("Error receiving data!\n");
            close(clientSocket);
            continue;
        }

        // Parse command and information
        char* command = strtok(buffer, " ");
        char* info = strtok(NULL, " ");

        // Log message
        logMessage("Driver", command, info);

        // Execute command
        char* response;
        if (strcmp(command, "Gap") == 0) {
            float gap = atof(info);
            response = gapAction(gap);
        } else if (strcmp(command, "Fuel") == 0) {
            float fuel = atof(info);
            response = fuelAction(fuel);
        } else if (strcmp(command, "Tire") == 0) {
            int tireWear = atoi(info);
            response = tireAction(tireWear);
        } else if (strcmp(command, "Tire Change") == 0) {
            response = tireChangeAction(info);
        } else {
            response = "Invalid Command";
        }

        // Log response
        logMessage("Paddock", command, response);

        // Send response to client
        send(clientSocket, response, strlen(response), 0);

        // Close client socket
        close(clientSocket);
    }

    // Close server socket
    close(serverSocket);

    return 0;
}
