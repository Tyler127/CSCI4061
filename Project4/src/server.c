#include "server.h"
#include "common.h"
#define PORT 8081
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024 


void *clientHandler(void *socket_desc) {
    // Setup the socket
    int sock = *(int*)socket_desc;

    while(1){
        // Receive the serialized packet from the client
        char buffer[PACKETSZ];
        recv(sock, buffer, PACKETSZ, MSG_WAITALL);

        // Deserialize the packet
        packet_t *received_packet = deserializeData(buffer);
        unsigned char received_operation = received_packet->operation;
        unsigned int received_size = received_packet->size;
        printf("    Received Packet:\n");
        if(received_operation == IMG_OP_ROTATE){
            printf("        operation: IMG_OP_ROTATE\n");
        } 
        else if(received_operation == IMG_OP_EXIT){
            printf("        operation: IMG_OP_EXIT\n");
            // TODO: Close the socket here?
            pthread_exit(NULL);
        }
        printf("        size: %d\n", received_size);

        // Allocate memory for the image data
        unsigned char* img_data = malloc(received_size);
        if(!img_data){
            perror("ERROR allocating memory for image data");
            free(received_packet);
            close(sock);
            return NULL;
        }

        // Receive the image data
        recv(sock, img_data, received_size, MSG_WAITALL);
        printf("img_data: ");
        for (int i = 0; i < 300 && i < received_size; ++i) {
            printf("%02X ", (unsigned char)img_data[i]);
        }
        printf("\n");

        // Process the image based on the operation and flags
        if(received_operation == IMG_OP_ROTATE){
            printf("    Processing image\n");

            // Create a temp file for the input image data
            const char* input_filename = "input.txt";
            FILE* input_file = fopen(input_filename, "w");
            write(input_file, img_data, received_size);

            // Rotate the image as per the flags (copied code from P3)
            int width;
            int height;
            int bpp;
            uint8_t* image_result = stbi_load(input_filename, &width, &height, &bpp,  CHANNEL_NUM);
            uint8_t** result_matrix = (uint8_t **)malloc(sizeof(uint8_t*) * width);
            uint8_t** img_matrix = (uint8_t **)malloc(sizeof(uint8_t*) * width);
            for(int i = 0; i < width; i++){
                result_matrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * height);
                img_matrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * height);
            }
            linear_to_image(image_result, img_matrix, width, height);
            if(received_packet->flags == IMG_FLAG_ROTATE_180){
                flip_left_to_right(img_matrix, result_matrix, width, height); 
            } 
            else if(received_packet->flags == IMG_FLAG_ROTATE_270){
                flip_upside_down(img_matrix, result_matrix, width, height);
            }
            uint8_t* img_array = (uint8_t *)malloc(sizeof(uint8_t) * width * height); 
            flatten_mat(result_matrix, img_array, width, height);

            // Create temp file for output image data
            const char* output_filename = "output.txt";
            FILE* output_file = fopen(output_filename, "w");
            stbi_write_png(output_filename, width, height, CHANNEL_NUM, img_array, (width*CHANNEL_NUM));

            // Create a success acknowledgement packet
            packet_t ack_packet;
            ack_packet.operation = IMG_OP_ACK;
            ack_packet.flags = received_packet->flags;
            ack_packet.size = received_size;

            // Send acknowledgement packet to client
            char* serialized_packet = serializePacket(&ack_packet);
            send(sock, serialized_packet, PACKETSZ, 0);

            // Send rotated img data to client
            char* rotated_image_buffer = (char*)malloc(received_size);
            read(output_filename, rotated_image_buffer, received_size);
            send(sock, rotated_image_buffer, received_size, 0);

            // TODO: send packet then data before cleanup

            // Clean up
            free(img_array);
            for(int i=0; i < width; i++){
                free(result_matrix[i]);
                free(img_matrix[i]);
            }
            free(result_matrix);
            free(img_matrix);
            fclose(input_file);
            fclose(output_file);
            remove(input_filename);
            remove(output_filename);
        }

        // Clean up
        free(img_data);
        free(received_packet);

    }

    // Clean up
    close(sock);
    return NULL;
}



int main(int argc, char* argv[]){
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
    listen(socket_fd, 5); // max Clients
    client_length = sizeof(client_addr);

    // Accept connections and create the client handling threads
    printf("SERVER: accepting connections.\n");
    while (1) {
        new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_addr, &client_length);
        if (new_socket_fd < 0) {
            perror("ERROR on accept");
            continue;
        }
        printf("SERVER: client accepted.\n");

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, clientHandler, (void*)&new_socket_fd) < 0) {
            perror("ERROR creating thread");
            return 1;
        }
        printf("SERVER: new thread made for client.\n");

        pthread_detach(thread_id); // Detach the thread
    }

    // Release any resources
    close(socket_fd);
    return 0;
}
