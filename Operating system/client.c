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

int send_file(const char *filename, const char *server_ip, int server_port) {
#ifdef _WIN32
    WSADATA wsa;
    SOCKET sock;
#else
    int sock;
#endif
    struct sockaddr_in server;
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

#ifdef _WIN32
    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return -1; // Initialization failed
    }
#endif

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error: Unable to create socket");
#ifdef _WIN32
        WSACleanup();
#endif
        return -2; // Socket creation failed
    }

    // Configure server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(server_ip);
    server.sin_port = htons(server_port);

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Error: Unable to connect to server");
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return -3; // Connection failed
    }

    printf("Connected to server %s:%d.\n", server_ip, server_port);

    // Open file for reading
    file = fopen(filename, "rb");
    if (!file) {
        perror("Error: Unable to open file");
#ifdef _WIN32
        closesocket(sock);
        WSACleanup();
#else
        close(sock);
#endif
        return -4; // File open failed
    }

    // Send file data
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        if (send(sock, buffer, bytes_read, 0) < 0) {
            perror("Error: Failed to send file data");
            fclose(file);
#ifdef _WIN32
            closesocket(sock);
            WSACleanup();
#else
            close(sock);
#endif
            return -5; // Data sending failed
        }
    }

    printf("File '%s' sent successfully to %s:%d.\n", filename, server_ip, server_port);

    fclose(file);
#ifdef _WIN32
    closesocket(sock);
    WSACleanup();
#else
    close(sock);
#endif
    return 0; // Success
}

int main() {
    char filename[256];
    char server_ip[50];
    int server_port;

    printf("Enter the file name to send: ");
    scanf("%255s", filename);

    printf("Enter the server IP address: ");
    scanf("%49s", server_ip);

    printf("Enter the server port: ");
    scanf("%d", &server_port);

    // Validate file existence before sending
#ifdef _WIN32
    if (_access(filename, 0) != 0) {
#else
    if (access(filename, F_OK) != 0) {
#endif
        printf("Error: File '%s' does not exist.\n", filename);
        return -1;
    }

    // Call send_file function to send the file
    int result = send_file(filename, server_ip, server_port);
    if (result != 0) {
        printf("Failed to send file. Error code: %d\n", result);
    }

    return 0;
}
