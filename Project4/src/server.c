#include "server.h"

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024 


void *clientHandler(void *socket) {
    int newsockfd = *(int*)socket;
    // Receive packets from the client

    // Determine the packet operatation and flags

    // Receive the image data using the size

    // Process the image data based on the set of flags

    // Acknowledge the request and return the processed image data
    close(newsockfd);
    return NULL;
}


int main(int argc, char* argv[]) {

    // Creating socket file descriptor
    int socket_fd;
    int new_socket_fd;
    int port_number;
    socklen_t client_length;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int n;

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0) 
       perror("ERROR opening socket");

    // Initialize the server adddress
    bzero((char*) &server_addr, sizeof(server_addr));
    port_number = PORT;

    server_addr.sin_family = AF_INET; // IPv4
    server_addr.sin_addr.s_addr = INADDR_ANY; // Listen to any of the network interface (INADDR_ANY) htonl()??
    server_addr.sin_port = htons(port_number); // Port number

    // Bind the socket to the port
    if (bind(socket_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0){
        perror("ERROR on binding");
        exit(0);
    }   

    // Listen on the socket
    listen(socket_fd, 5); // Max Clients
    client_length = sizeof(client_addr);

    // Accept connections and create the client handling threads
    while (1) {
        new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_length);
        if (new_socket_fd < 0) {
            perror("ERROR on accept");
            continue;
        }

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, clientHandler, (void*)&new_socket_fd) < 0) {
            perror("ERROR creating thread");
            return 1;
        }
        pthread_detach(thread_id); // Optionally detach the thread
    }

    // Release any resources
    close(socket_fd);
    return 0;
}
