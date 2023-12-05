#include "client.h"
#include <netdb.h>  // Include this for struct hostent and related network functions
#include <string.h> // Include this for memcpy'
#include <sys/stat.h>  // For stat


#define PORT 8080
#define BUFFER_SIZE 1024 

int send_file(int socket, const char *filename, int rotation_angle) {
    // Open the file
    FILE *file = fopen(filename, "rb");
    if (file == NULL) { //error check
        perror("ERROR opening file");
        return -1;
    }
    fseek(file, 0, SEEK_END);
    long img_size = ftell(file);
    rewind(file);
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

    packet_t Packet;
    Packet.size = img_size;
    Packet.operation = IMG_OP_ROTATE;
    if (rotation_angle == 180){
        Packet.flags == IMG_FLAG_ROTATE_180;
    }
    else if(rotation_angle == 270){
        Packet.flags == IMG_FLAG_ROTATE_270;
    }



    // Send packet to the server
    // ...

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
    if(argc != 3){
        fprintf(stderr, "Usage: ./client File_Path_to_images File_Path_to_output_dir Rotation_angle. \n");
        return 1;
    }
    
    // Set up socket
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    DIR *dir;
    struct dirent *ent;
    struct stat file_stat;  // For checking file type

    if ((dir = opendir(argv[1])) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            char filepath[PATH_MAX];
            sprintf(filepath, "%s/%s", argv[1], ent->d_name);
            if (stat(filepath, &file_stat) == 0 && S_ISREG(file_stat.st_mode)) {
                // Add the filename to the queue
                // ...
            }
        }
        closedir(dir);
    } else {
        perror("Could not open directory");
        return EXIT_FAILURE;
    }

    // Connect the socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { //error check
        perror("ERROR opening socket");
        exit(0);
    }

    server = gethostbyname("localhost"); // Replace "localhost" with the actual server address if needed
    if (server == NULL) { //error check
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }

    memset((char *) &serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    memcpy((char *)&serv_addr.sin_addr.s_addr, server->h_addr_list[0], server->h_length);
    serv_addr.sin_port = htons(PORT);

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }
    // Read the directory for all the images to rotate


    // Send the image data to the server

    // Check that the request was acknowledged

    // Receive the processed image and save it in the output dir

    // Terminate the connection once all images have been processed

    // Release any resources
    return 0;
}
