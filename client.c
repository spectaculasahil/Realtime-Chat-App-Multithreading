#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // Added for inet_addr
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5002
#define BUFFER_SIZE 1024
#define SEP ": "

void *receive_messages(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int bytes_received;
    
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        printf("%s\n", buffer);
    }
    
    printf("Disconnected from server.\n");
    exit(0);
    return NULL;
}

int main() {
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(1);
    }
    
    char username[50];
    printf("Enter your username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';
    
    if (send(client_socket, username, strlen(username), 0) < 0) {
        perror("Failed to send username");
        exit(1);
    }
    
    pthread_t receive_thread;
    if (pthread_create(&receive_thread, NULL, receive_messages, &client_socket) != 0) {
        perror("Thread creation failed");
        exit(1);
    }
    
    printf("Type 'quit' to exit or 'PRIVATE:username:message' for private messages.\n");
    
    char message[BUFFER_SIZE];
    while (1) {
        fgets(message, BUFFER_SIZE, stdin);
        message[strcspn(message, "\n")] = '\0';
        
        if (strcmp(message, "quit") == 0) {
            break;
        }
        
        if (send(client_socket, message, strlen(message), 0) < 0) {
            perror("Send failed");
            break;
        }
    }
    
    close(client_socket);
    return 0;
}