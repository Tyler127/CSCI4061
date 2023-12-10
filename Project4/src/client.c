#include "client.h"
#include "common.h"
#include <netdb.h>  // Include this for struct hostent and related network functions
#include <string.h> // Include this for memcpy'
#include <sys/stat.h>  // For stat

#define PORT 8083
#define BUFFER_SIZE 1024 


/********************* [ Queue Implementation ] **********************/

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
    printf("|----- VISUALIZING QUEUE -----|\n");
    printf(" queue size: %d\n", q->size);
    printf(" queue front request file: %s\n\n", q->requests[q->front].file_path);

    for(int i=0; i < q->size; i++){
        request_t current_request = q->requests[i];
        printf("i: %d\n", i);
        printf("    request file: %s\n", current_request.file_path);
        printf("    rotation angle: %d\n", current_request.rotation_angle);
    }

    printf("|----- VISUALIZING QUEUE DONE -----|\n");
}

// Populate the queue with a request struct for each .png file in directory_path with the given rotation angle
void queue_populate(request_queue_t *q, char* directory_path, int rotation_angle){
    printf("    [q_populate]: directory_path - %s\n", directory_path);
    printf("    [q_populate]: rotation_angle - %d\n", rotation_angle);

    DIR *dir;
    struct dirent *entry;

    if((dir = opendir(directory_path)) == NULL){
        perror("ERROR opendir() error");
        exit(EXIT_FAILURE);
    }

    while((entry = readdir(dir)) != NULL){
        struct stat file_stat;
        stat(entry->d_name, &file_stat);
        printf("    [q_populate]: reading entry\n");
        
        if(entry->d_type == DT_REG){  // Check for regular file
            // Skip any non .png files
            char* file_extension = strchr(entry->d_name, '.');
            if(strcmp(file_extension, ".png") != 0){
                printf("        [q_populate]: non-png file found\n");
                continue;
            }

            // Assemble the file path string 
            char* file_path = malloc(PATH_MAX);
            sprintf(file_path, "%s/%s", directory_path, entry->d_name);
            printf("        [q_populate]: file path -> %s\n", file_path);
            
            // Create a new node struct
            request_t request;
            request.file_path = file_path;
            request.rotation_angle = rotation_angle;
            printf("            [q_populate]: request.filepath -> %s\n", request.file_path);
            
            // Enqueue the request
            queue_enqueue(q, request);
            printf("    [q_populate]: file %s added to queue\n", file_path);
        }
    }
}

// Global queue
request_queue_t request_queue;

/********************* [ End of Queue Implementation ] **********************/


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

    // Create a packet with size & flag
    packet_t packet;
    packet.size = img_size;
    packet.operation = IMG_OP_ROTATE;
    if(rotation_angle == 180){
        packet.flags = IMG_FLAG_ROTATE_180;
    }
    else if(rotation_angle == 270){
        packet.flags = IMG_FLAG_ROTATE_270;
    }

    // Serialize the packet
    char *serialized_packet = serializePacket(&packet);

    // Send the serialized packet to the server
    size_t sent_packet_size = send(socket, serialized_packet, PACKETSZ, 0);
    if(sent_packet_size != PACKETSZ){
        perror("ERROR incorrect packet size sent");
        return 1;
    }

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
        return 1;
    }
    printf("    [receive_file]: file created\n");

    // Receive the processed image data from the server 
    int max_img_size = 10000;
    char* img_data = malloc(max_img_size);
    size_t read_data_size = recv(socket, img_data, max_img_size, 0);
    if(read_data_size == -1){
        perror("ERROR reading img packet from server");
        return 1;
    }
    printf("    [receive_file]: img data received from server. size: %ld\n", read_data_size);

    // Write read amount of data to the file from the buffer
    size_t written_data_size = fwrite(img_data, 1, read_data_size, file);
    if(written_data_size != read_data_size){
        perror("ERROR incorrect amount of data written to file");
        return 1;
    }
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

    printf("[CLIENT]: STARTING\n");
    printf("[CLIENT]: src folder -> %s\n", input_path);

    // Initialize the global queue
    queue_init(&request_queue);
    queue_populate(&request_queue, input_path, rotation_angle);
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

        // Send the image data to the server
        send_file(sockfd, request.file_path, rotation_angle);

        // Check that the request was acknowledged
        char ack_packet_buffer[PACKETSZ];
        size_t read_packet_size = recv(sockfd, ack_packet_buffer, PACKETSZ, 0);
        if(read_packet_size != PACKETSZ){
            perror("ERROR incorrect packet size received");
            return 1;
        }
        
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

    // Create a packet with the exit operation
    packet_t exit_packet;
    exit_packet.operation = IMG_OP_EXIT;
    exit_packet.size = 0;
    exit_packet.flags = IMG_FLAG_ROTATE_180;
    
    // Send the serialized packet to the server
    char* exit_packet_data = serializePacket(&exit_packet);
    int ret = send(sockfd, exit_packet_data, PACKETSZ, 0);
    if(ret == -1){
        perror("ERROR sending exit packet to server");
        return 1;
    }
    printf("    [main]: exit packet sent to server\n");

    // Terminate the connection once all images have been processed
    // TODO: termination

    // Release any resources
    // TODO: release rss

    return 0;
}