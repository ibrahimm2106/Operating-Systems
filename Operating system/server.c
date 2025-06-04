#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#define BUFFER_SIZE 8192

void start_server(int port, const char *output_file) {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
#ifdef _WIN32
    int client_addr_len = sizeof(client_addr);
    WSADATA wsa;
#else
    socklen_t client_addr_len = sizeof(client_addr);
#endif
    FILE *file;
    char buffer[BUFFER_SIZE];
    int bytes_received;

#ifdef _WIN32
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return;
    }
#endif

    // Create server socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error: Unable to create socket");
        return;
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    // Bind socket to the address
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error: Unable to bind socket");
#ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
#else
        close(server_socket);
#endif
        return;
    }

    // Listen for connections
    if (listen(server_socket, 5) < 0) {
        perror("Error: Unable to listen on socket");
#ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
#else
        close(server_socket);
#endif
        return;
    }

    printf("Server listening on port %d...\n", port);

    // Accept a connection
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_socket < 0) {
        perror("Error: Unable to accept connection");
#ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
#else
        close(server_socket);
#endif
        return;
    }

    printf("Client connected. Receiving file...\n");

    // Open file for writing
    file = fopen(output_file, "wb");
    if (!file) {
        perror("Error: Unable to open output file");
#ifdef _WIN32
        closesocket(client_socket);
        closesocket(server_socket);
        WSACleanup();
#else
        close(client_socket);
        close(server_socket);
#endif
        return;
    }

    // Receive file data
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    printf("File received and saved as '%s'.\n", output_file);

    fclose(file);
#ifdef _WIN32
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();
#else
    close(client_socket);
    close(server_socket);
#endif
}

int main() {
    int port = 8080; // Port to listen on
    const char *output_file = "received_file.txt"; // File to save the received data

    start_server(port, output_file);

    return 0;
}
