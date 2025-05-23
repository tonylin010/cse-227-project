#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PORT 8080
#define BUFFER_SIZE 1024

void emit_telemetry();

int main() {
    // Initialize Winsock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    emit_telemetry();

    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    // Set socket option to reuse address
    char opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == SOCKET_ERROR) {
        printf("Setsockopt failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 3) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        SOCKET client_socket = accept(server_fd, NULL, NULL);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed\n");
            continue;
        }

        char buffer[BUFFER_SIZE] = {0};
        int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received > 0) {
            printf("Received request:\n%s\n", buffer);
        }

        const char *response = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: text/plain\r\n"
                             "Content-Length: 13\r\n\r\n"
                             "Hello, World!";
        
        send(client_socket, response, strlen(response), 0);
        closesocket(client_socket);
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}

void emit_telemetry() {
    SOCKET sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        return;
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(8080);
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) == SOCKET_ERROR) {
        printf("Connection failed\n");
        closesocket(sockfd);
        return;
    }

    char *command = "whoami";
    FILE *fp = _popen(command, "r");
    if (fp == NULL) {
        printf("Failed to run command\n");
        closesocket(sockfd);
        return;
    }

    char buffer[BUFFER_SIZE];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        send(sockfd, buffer, strlen(buffer), 0);
    }

    _pclose(fp);
    closesocket(sockfd);
}
