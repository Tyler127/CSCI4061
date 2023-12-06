#include "client.h"
#include "common.h"
#include <netdb.h>  // Include this for struct hostent and related network functions
#include <string.h> // Include this for memcpy'
#include <sys/stat.h>  // For stat
#include <dirent.h>

#define PORT 8080
#define BUFFER_SIZE 1024 

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

int send_file(int socket, const char *filename, int rotation_angle) {
    printf("CLIENT: sending file: %s - rot angle: %d - socket: %d\n", filename, rotation_angle, socket);

    // Open the file
    FILE *file = fopen(filename, "rb");
    if (file == NULL) { //error check
        perror("ERROR opening file");
        return -1;
    }

    // Calculate the file size
    fseek(file, 0, SEEK_END);
    long img_size = ftell(file);
    rewind(file);
    printf("    CLIENT: file size: %ld\n", img_size);

    // Allocate memory for the image data
    unsigned char *img_data = malloc(img_size);
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

    // Create a packet 
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
    printf("CLIENT: packet serialized\n");

    // Send the serialized packet to the server
    write(socket, serializedPacket, PACKETSZ);

    fclose(file);
    return 0;
}


int receive_file(int socket, const char *output_filename) {
    // Open the file for writing
    FILE *file = fopen(output_filename, "wb");
    if (file == NULL) { //error check
        perror("ERROR opening file");
        return -1;
    }

    // Receive the processed image data from the server
    // ...

    fclose(file);
    return 0;
}


int main(int argc, char* argv[]) {
    if(argc != 4){
        fprintf(stderr, "Usage: ./client File_Path_to_images File_Path_to_output_dir Rotation_angle. \n");
        return 1;
    }
    char* path_to_images = argv[1];
    char* path_to_output = argv[2];
    int rotation_angle = atoi(argv[3]);

    // Initialize the global queue
    queue_init(&request_queue);

    // Read the directory for all the images to rotate
    DIR *dir;
    struct dirent *entry;

    if((dir = opendir(path_to_images)) == NULL){
        perror("opendir() error");
        exit(EXIT_FAILURE);
    }

    while((entry = readdir(dir)) != NULL){
        struct stat file_stat;
        stat(entry->d_name, &file_stat);

        if(S_ISREG(file_stat.st_mode)){  // Check for regular file
            // Assemble the file path string 
            char* file_path = malloc(PATH_MAX);
            sprintf(file_path, "%s/%s", argv[1], entry->d_name);
            
            // Create a new node struct
            request_t request;
            request.file_path = file_path;
            request.rotation_angle = rotation_angle;
            
            // Enqueue the request
            queue_enqueue(&request_queue, request);

            printf("    File %s added to queue\n", file_path);
        }
    }
    queue_visualize(&request_queue);

    // Set up socket
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    // Connect the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0){ //error check
        perror("ERROR opening socket");
        exit(0);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // server ip
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    printf("Client connected. \n");
    


    // Send the image data to the server
    packet_t packet_bad;
    packet_bad.operation = htons(IMG_OP_ROTATE);

    char* data = serializePacket(&packet_bad);

    // Send the serialized packet to the server
    int ret = send(sockfd, data, PACKETSZ, 0);
    if(ret == -1)
        perror("ERROR sending packet to server");

    printf("Client sent packet\n");


    // Check that the request was acknowledged

    // Receive the processed image and save it in the output dir

    // Terminate the connection once all images have been processed

    // Release any resources
    return 0;
}
