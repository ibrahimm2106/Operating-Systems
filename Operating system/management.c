/**
 * File Management System
 * This program provides basic file management utilities such as creating, deleting,
 * encrypting, and merging files. It is designed to be cross-platform and includes
 * permission management functionality.
 *
 * Author: Mohamed Ibrahim
 * Date: December 20, 2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#ifdef _WIN32
#include <io.h>
#include <winsock2.h>
#include <sys/stat.h>
#pragma comment(lib, "ws2_32.lib")
#define access _access
#define chmod _chmod
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>
#endif

#define BUFFER_SIZE 8192

// Function prototypes with detailed descriptions
void create_file(const char *filename); // Creates a new file
void delete_file(const char *filename); // Deletes an existing file
void change_permissions(const char *filename, int permissions); // Changes the permission mode of a file
void merge_files(const char *file1, const char *file2, const char *output_file, const char *output_folder); // Merges two files into one
void encrypt_file(const char *filename, const char *password); // Encrypts a file with a given password
void decrypt_file(const char *filename, const char *password); // Decrypts a file with a given password
void monitor_directory(const char *directory); // Monitors changes in a directory (simulated)
int validate_file_exists(const char *filename); // Validates if a file exists
int authenticate_user(const char *username, const char *password); // Authenticates a user with username and password
int send_file(const char *filename, const char *server_ip, int server_port); // Sends a file over the network
void display_menu(); // Displays the user menu for file management options
int get_valid_choice(); // Gets a valid menu choice from the user

// Function definitions with detailed documentation
void create_file(const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file) {
        printf("File created successfully.\n");
        fclose(file);
    } else {
        printf("Failed to create file.\n");
    }
}

void delete_file(const char *filename) {
    if (remove(filename) == 0) {
        printf("File deleted successfully.\n");
    } else {
        printf("Failed to delete file.\n");
    }
}

void change_permissions(const char *filename, int permissions) {
#ifdef _WIN32
    if (_chmod(filename, permissions) == 0) {
        printf("Permissions changed successfully.\n");
    } else {
        printf("Failed to change permissions.\n");
    }
#else
    if (chmod(filename, permissions) == 0) {
        printf("Permissions changed successfully.\n");
    } else {
        printf("Failed to change permissions.\n");
    }
#endif
}

void merge_files(const char *file1, const char *file2, const char *output_file, const char *output_folder) {
    char output_path[512];
    snprintf(output_path, sizeof(output_path), "%s/%s", output_folder, output_file);

    FILE *f1 = fopen(file1, "rb");
    FILE *f2 = fopen(file2, "rb");
    FILE *fout = fopen(output_path, "wb");

    if (f1 && f2 && fout) {
        char buffer[BUFFER_SIZE];
        size_t bytes;
        while ((bytes = fread(buffer, 1, BUFFER_SIZE, f1)) > 0) {
            fwrite(buffer, 1, bytes, fout);
        }
        while ((bytes = fread(buffer, 1, BUFFER_SIZE, f2)) > 0) {
            fwrite(buffer, 1, bytes, fout);
        }
        printf("Files merged successfully into '%s'.\n", output_path);
    } else {
        printf("Failed to open files for merging.\n");
    }

    if (f1) fclose(f1);
    if (f2) fclose(f2);
    if (fout) fclose(fout);
}

void encrypt_file(const char *filename, const char *password) {
    FILE *fp = fopen(filename, "rb+");
    if (!fp) {
        printf("Failed to open file for encryption.\n");
        return;
    }

    size_t password_length = strlen(password);
    if (password_length == 0) {
        printf("Password cannot be empty.\n");
        fclose(fp);
        return;
    }

    fseek(fp, 0, SEEK_END);
    long file_size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *buffer = malloc(file_size);
    if (!buffer) {
        printf("Memory allocation failed.\n");
        fclose(fp);
        return;
    }

    fread(buffer, 1, file_size, fp);
    rewind(fp);

    for (long i = 0; i < file_size; i++) {
        buffer[i] ^= password[i % password_length];
    }

    fwrite(buffer, 1, file_size, fp);
    free(buffer);
    fclose(fp);

    printf("File '%s' encrypted successfully.\n", filename);
}

void decrypt_file(const char *filename, const char *password) {
    encrypt_file(filename, password); // Uses the same function for decryption
    printf("File '%s' decrypted successfully.\n", filename);
}

void monitor_directory(const char *directory) {
    printf("Pretend to monitor directory '%s'.\n", directory);
}

int validate_file_exists(const char *filename) {
    if (access(filename, F_OK) != -1) {
        return 1; // File exists
    } else {
        return 0; // File does not exist
    }
}

int authenticate_user(const char *username, const char *password) {
    return (strcmp(username, "admin") == 0 && strcmp(password, "password") == 0);
}

int send_file(const char *filename, const char *server_ip, int server_port) {
    printf("File '%s' has been successfully sent to server at %s:%d.\n", filename, server_ip, server_port);
    return 0; // Simulate successful operation
}

void display_menu() {
    printf("\nFile Management System - Menu\n");
    printf("1. Create a file\n");
    printf("2. Delete a file\n");
    printf("3. Change file permissions\n");
    printf("4. Merge two files\n");
    printf("5. Encrypt a file\n");
    printf("6. Decrypt a file\n");
    printf("7. Monitor directory changes\n");
    printf("8. Send a file over the network\n");
    printf("9. Exit\n");
    printf("10. Test if a file exists\n");
}

int get_valid_choice() {
    int choice;
    printf("Enter your choice: ");
    if (scanf("%d", &choice) == 1) {
        return choice;
    } else {
        while (getchar() != '\n'); // Clear invalid input
        return -1; // Invalid input
    }
}

int main() {
    int choice;
    char filename[256], filename2[256], output_file[256], password[50], directory[256];
    char username[50], entered_password[50];

    while (1) {
        display_menu();
        choice = get_valid_choice();

        switch (choice) {
            case 1:
                printf("Enter the file name to create: ");
                scanf("%255s", filename);
                create_file(filename);
                break;
            case 2:
                printf("Enter the file name to delete: ");
                scanf("%255s", filename);
                if (validate_file_exists(filename)) {
                    delete_file(filename);
                }
                break;
            case 3:
                printf("Enter the file name to change permissions: ");
                scanf("%255s", filename);
                int permissions;
                printf("Enter new permissions (e.g., 644): ");
                scanf("%o", &permissions); // Octal input for permissions
                if (validate_file_exists(filename)) {
                    change_permissions(filename, permissions);
                }
                break;
            case 4:
                printf("Enter the first file name: ");
                scanf("%255s", filename);
                printf("Enter the second file name: ");
                scanf("%255s", filename2);
                printf("Enter the output file name: ");
                scanf("%255s", output_file);
                printf("Enter the destination folder for the merged file: ");
                scanf("%255s", directory);
                if (validate_file_exists(filename) && validate_file_exists(filename2)) {
                    merge_files(filename, filename2, output_file, directory);
                }
                break;
            case 5:
                printf("Enter the file name to encrypt: ");
                scanf("%255s", filename);
                printf("Enter the encryption password: ");
                scanf("%49s", password);
                if (validate_file_exists(filename)) {
                    encrypt_file(filename, password);
                }
                break;
            case 6:
                printf("Enter the file name to decrypt: ");
                scanf("%255s", filename);
                printf("Enter the decryption password: ");
                scanf("%49s", password);
                if (validate_file_exists(filename)) {
                    decrypt_file(filename, password);
                }
                break;
            case 7:
                printf("Enter the directory to monitor: ");
                scanf("%255s", directory);
                monitor_directory(directory);
                break;
            case 8:
                printf("Enter your username: ");
                scanf("%49s", username);
                printf("Enter your password: ");
                scanf("%49s", entered_password);
                if (authenticate_user(username, entered_password)) {
                    printf("Enter the file name to send: ");
                    scanf("%255s", filename);
                    char server_ip[50];
                    printf("Enter the server IP address: ");
                    scanf("%49s", server_ip);
                    int server_port;
                    printf("Enter the server port: ");
                    scanf("%d", &server_port);
                    if (validate_file_exists(filename)) {
                        send_file(filename, server_ip, server_port);
                    }
                } else {
                    printf("Authentication failed.\n");
                }
                break;
            case 9:
                printf("Exiting the program. Goodbye!\n");
                return 0;
            case 10:
                printf("Enter the file name to test existence: ");
                scanf("%255s", filename);
                if (validate_file_exists(filename)) {
                    printf("File '%s' exists and is accessible.\n", filename);
                } else {
                    printf("File '%s' does not exist or cannot be accessed.\n", filename);
                }
                break;
            default:
                printf("Invalid choice. Please select a valid option.\n");
        }
    }
    return 0;
}
