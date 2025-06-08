#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

#define PORT 5002
#define MAX_CLIENTS 100
#define BUFFER_SIZE 1024
#define SEP ": "

// Global variables
int client_sockets[MAX_CLIENTS];
char *usernames[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(const char *message) {
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_count; i++) {
        if (send(client_sockets[i], message, strlen(message), 0) < 0) {
            perror("Broadcast failed");
        }
    }
    pthread_mutex_unlock(&mutex);
}

void broadcast_user_list() {
    char user_list[BUFFER_SIZE] = "Online users: ";
    pthread_mutex_lock(&mutex);
    for (int i = 0; i < client_count; i++) {
        strcat(user_list, usernames[i]);
        if (i < client_count - 1) strcat(user_list, ", ");
    }
    pthread_mutex_unlock(&mutex);
    broadcast(user_list);
}

void remove_client(int client_socket, int index) {
    pthread_mutex_lock(&mutex);
    char leave_msg[BUFFER_SIZE];
    snprintf(leave_msg, BUFFER_SIZE, "%s has left the chat.", usernames[index]);
    for (int i = index; i < client_count - 1; i++) {
        client_sockets[i] = client_sockets[i + 1];
        usernames[i] = usernames[i + 1];
    }
    client_count--;
    pthread_mutex_unlock(&mutex);
    broadcast(leave_msg);
    broadcast_user_list();
    close(client_socket);
}

void send_private_message(const char *sender, const char *target, const char *message) {
    char private_msg[BUFFER_SIZE];
    snprintf(private_msg, BUFFER_SIZE, "Private from %s: %s", sender, message);
    pthread_mutex_lock(&mutex);
    int target_index = -1;
    for (int i = 0; i < client_count; i++) {
        if (strcmp(usernames[i], target) == 0) {
            target_index = i;
            break;
        }
    }
    if (target_index >= 0) {
        if (send(client_sockets[target_index], private_msg, strlen(private_msg), 0) < 0) {
            remove_client(client_sockets[target_index], target_index);
        }
    } else {
        char error_msg[BUFFER_SIZE];
        snprintf(error_msg, BUFFER_SIZE, "Error: User %s not found.", target);
        for (int i = 0; i < client_count; i++) {
            if (strcmp(usernames[i], sender) == 0) {
                send(client_sockets[i], error_msg, strlen(error_msg), 0);
                break;
            }
        }
    }
    pthread_mutex_unlock(&mutex);
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    char username[50];
    
    // Receive username
    int bytes_received = recv(client_socket, username, sizeof(username) - 1, 0);
    if (bytes_received <= 0) {
        close(client_socket);
        return NULL;
    }
    username[bytes_received] = '\0';
    
    // Add client to lists
    pthread_mutex_lock(&mutex);
    client_sockets[client_count] = client_socket;
    usernames[client_count] = strdup(username);
    client_count++;
    pthread_mutex_unlock(&mutex);
    
    char join_msg[BUFFER_SIZE];
    snprintf(join_msg, BUFFER_SIZE, "%s has joined the chat.", username);
    broadcast(join_msg);
    broadcast_user_list();
    
    // Handle client messages
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0)) > 0) {
        buffer[bytes_received] = '\0';
        if (strncmp(buffer, "PRIVATE:", 8) == 0) {
            char *target = strtok(buffer + 8, SEP);
            char *message = strtok(NULL, "");
            if (target && message) {
                send_private_message(username, target, message);
            }
        } else {
            char full_msg[BUFFER_SIZE];
            snprintf(full_msg, BUFFER_SIZE, "%s%s%.971s", username, SEP, buffer); // Limit buffer to 971 bytes
            broadcast(full_msg);
        }
    }
    
    // Client disconnected
    for (int i = 0; i < client_count; i++) {
        if (client_sockets[i] == client_socket) {
            remove_client(client_socket, i);
            break;
        }
    }
    return NULL;
}

int main() {
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }
    
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(1);
    }
    
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        exit(1);
    }
    
    printf("Server listening on port %d\n", PORT);
    
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }
        
        pthread_t thread;
        if (pthread_create(&thread, NULL, handle_client, &client_socket) != 0) {
            perror("Thread creation failed");
            close(client_socket);
        }
        pthread_detach(thread);
    }
    
    close(server_socket);
    return 0;
}