#include "server.h"
#include "common.h"
#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024 


void *clientHandler(void *socket_desc) {
    int sock = *(int*)socket_desc;
    char buffer[PACKETSZ];

    // Receive the serialized packet from the client
    read(sock, buffer, PACKETSZ);

    // Deserialize the packet
    packet_t *receivedPacket = deserializeData(buffer);

    // Allocate memory for the image data
    unsigned char *img_data = malloc(receivedPacket->size);
    if (!img_data) {
        perror("ERROR allocating memory for image data");
        free(receivedPacket);
        close(sock);
        return NULL;
    }

    // Receive the image data
    read(sock, img_data, receivedPacket->size);

    // Process the image based on the operation and flags
    if (receivedPacket->operation == IMG_OP_ROTATE) {
        // Rotate the image as per the flags
        // ...

        // Optionally, send a response back to the client
        // ...
    }

    // Clean up
    free(img_data);
    free(receivedPacket);
    close(sock);
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
