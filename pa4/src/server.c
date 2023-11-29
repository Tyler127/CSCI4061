#include "server.h"

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024 


void *clientHandler(void *socket) {

    // Receive packets from the client

    // Determine the packet operatation and flags

    // Receive the image data using the size

    // Process the image data based on the set of flags

    // Acknowledge the request and return the processed image data
}

int main(int argc, char* argv[]) {

    // Creating socket file descriptor

    // Bind the socket to the port

    // Listen on the socket

    // Accept connections and create the client handling threads

    // Release any resources
    return 0;
}
