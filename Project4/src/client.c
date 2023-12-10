#include "client.h"
#include "common.h"
#include <netdb.h>  // Include this for struct hostent and related network functions
#include <string.h> // Include this for memcpy'
#include <sys/stat.h>  // For stat

#define PORT 8082
#define BUFFER_SIZE 1024 

/* START QUEUE IMPLEMENTATION */

// Initialize the queue
void queue_init(request_queue_t *q) {
    q->size = 0;
    q->front = 0;
    q->rear = -1;
}

// Add a request to the queue
void queue_enqueue(request_queue_t *q, request_t request) { 
    q->rear = (q->rear + 1) % MAX_QUEUE_LEN;
    q->requests[q->rear] = request;
    q->size++;
}

// Remove a request from the queue
request_t queue_dequeue(request_queue_t *q) {
    request_t request = q->requests[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_LEN;
    q->size--;
    return request;
}

// View queue size and requests
void queue_visualize(request_queue_t *q){
    printf("|-----VISUALIZING QUEUE-----|\n");
    printf(" queue size: %d\n", q->size);
    printf(" queue front request file: %s\n\n", q->requests[q->front].file_path);

    for(int i=0; i < q->size; i++){
        request_t current_request = q->requests[i];
        printf("i: %d\n", i);
        printf("    request file: %s\n", current_request.file_path);
        printf("    rotation angle: %d\n", current_request.rotation_angle);
    }

    printf("|-----VISUALIZING QUEUE DONE-----|\n");
}

// Global queue
request_queue_t request_queue;

/* END QUEUE IMPLEMENTATION */



int send_file(int socket, const char *filename, int rotation_angle) {
    printf("    [send_file]: filename: %s - rot angle: %d - socket: %d\n", filename, rotation_angle, socket);

    // Open the file
    FILE *file = fopen(filename, "r");
    if (file == NULL) { //error check
        perror("ERROR opening file");
        return -1;
    }

    // Calculate the file size
    fseek(file, 0, SEEK_END);
    long img_size = ftell(file);
    rewind(file);
    printf("    [send_file]: file size: %ld\n", img_size);

    // Allocate memory for the image data
    char* img_data = (char*)malloc(img_size);
    memset(img_data, 0, img_size);
    if (img_data == NULL) {
        perror("ERROR allocating memory for image");
        fclose(file);
        return -1;
    }

    // Read the image data into the buffer
    if (fread(img_data, 1, img_size, file) != img_size) {
        perror("ERROR reading image file");
        free(img_data);
        fclose(file);
        return -1;
    }
    printf("send_file: img_data: ");
    for (int i = 0; i < 300 && i < img_size; ++i) {
        printf("%02X ", (char)img_data[i]);
    }
    printf("\n");

    // Create a packet with size & flag
    packet_t Packet;
    Packet.size = img_size;
    Packet.operation = IMG_OP_ROTATE;
    if(rotation_angle == 180){
        Packet.flags == IMG_FLAG_ROTATE_180;
    }
    else if(rotation_angle == 270){
        Packet.flags == IMG_FLAG_ROTATE_270;
    }

    // Serialize the packet
    char *serializedPacket = serializePacket(&Packet);
    printf("    [send_file]: packet serialized\n");

    // Send the serialized packet to the server
    send(socket, serializedPacket, PACKETSZ, 0);

    // Send the image data to the server
    size_t sent_data_size = send(socket, img_data, img_size, 0);
    printf("    [send_file]: img sent to server. size: %ld\n", sent_data_size);

    fclose(file);
    return 0;
}


int receive_file(int socket, const char* output_filename) {
    // Open the file for writing
    printf("    [receive_file]: output_filename: %s\n", output_filename);
    FILE *file = fopen(output_filename, "wb");
    if(file == NULL){ 
        perror("ERROR opening file");
        return -1;
    }
    printf("    [receive_file]: file created\n");

    // Receive the processed image data from the server
    char* data_buffer = malloc(1000);
    recv(socket, data_buffer, 1000, 0);
    printf("    [receive_file]: img data received from server\n");

    // Write buffer to the created file
    fwrite(file, 1, data_buffer, 1000);
    printf("    [receive_file]: img data successfully written to file\n");

    fclose(file);
    return 0;
}


int main(int argc, char* argv[]) {
    if(argc != 4){
        fprintf(stderr, "Usage: ./client Files_input_path File_Path_to_output_dir Rotation_angle. \n");
        return 1;
    }
    char* input_path = argv[1];
    char* output_path = argv[2];
    int rotation_angle = atoi(argv[3]);

    printf("CLIENT STARTING:\n");
    printf("    src folder: %s\n", input_path);

    // Initialize the global queue
    queue_init(&request_queue);

    // Add .png files to the request queue
    DIR *dir;
    struct dirent *entry;

    if((dir = opendir(input_path)) == NULL){
        perror("opendir() error");
        exit(EXIT_FAILURE);
    }

    while((entry = readdir(dir)) != NULL){
        struct stat file_stat;
        stat(entry->d_name, &file_stat);
        printf("    reading entry:\n");
        
        if(entry->d_type == DT_REG){  // Check for regular file
            // Skip any non .png files
            char* file_extension = strchr(entry->d_name, '.');
            if(strcmp(file_extension, ".png") != 0){
                printf("        non-png file found\n");
                continue;
            }

            // Assemble the file path string 
            char* file_path = malloc(PATH_MAX);
            sprintf(file_path, "%s/%s", input_path, entry->d_name);
            printf("        file path: %s\n", file_path);
            
            // Create a new node struct
            request_t request;
            request.file_path = file_path;
            request.rotation_angle = rotation_angle;
            printf("            request.filepath: %s\n", request.file_path);
            
            // Enqueue the request
            queue_enqueue(&request_queue, request);
            printf("    File %s added to queue\n", file_path);
        }
    }
    queue_visualize(&request_queue);

    // Set up socket
    int sockfd;
    struct sockaddr_in serv_addr;

    // Connect the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){ //error check
        perror("ERROR opening socket");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // server ip
    serv_addr.sin_port = htons(PORT);

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        perror("ERROR connecting");
        exit(1);
    }
    printf("Client connected. \n");

    while(request_queue.size != 0){
        printf("|----- Making a request -----|\n");

        // Take a request off of the queue
        request_t request = queue_dequeue(&request_queue);
        printf("    [main]: request file_path: %s\n", request.file_path);
        
        // TODO: move packet sending code here out of send_file function

        // Send the image data to the server
        send_file(sockfd, request.file_path, rotation_angle);

        // Check that the request was acknowledged
        char ack_packet_buffer[PACKETSZ];
        recv(sockfd, ack_packet_buffer, PACKETSZ, MSG_WAITALL);
        
        // Deserialize the packet
        packet_t *received_packet = deserializeData(ack_packet_buffer);
        unsigned char received_operation = received_packet->operation;
        if(received_operation == IMG_OP_ACK){
            printf("    [main]: acknowledgement packet from server received!\n");
        }

        // Receive the processed image and save it in the output dir
        const char* file_name = get_filename_from_path(request.file_path);
        char* output_file_path = (char*)malloc(strlen(output_path) + strlen(file_name) + 1);
        sprintf(output_file_path, "%s/%s", output_path, file_name);
        printf("    [main]: output file path: %s\n", output_file_path);
        receive_file(sockfd, output_file_path);

        printf("|----- End of Request -----|\n");
    }

    // TODO: send a good exit packet
    packet_t packet_bad;
    packet_bad.operation = IMG_OP_EXIT;
    packet_bad.size = 15;

    char* data = serializePacket(&packet_bad);

    // Send the serialized packet to the server
    int ret = send(sockfd, data, PACKETSZ, 0);
    if(ret == -1)
        perror("ERROR sending packet to server");


    // Terminate the connection once all images have been processed

    // Release any resources
    return 0;
}