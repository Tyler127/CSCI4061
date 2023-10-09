#include "utils.h"

// Create N files and distribute the data from the input file evenly among them
// See section 3.1 of the project writeup for important implementation details
// Hint: Use fseek() and ftell() to determine the size of the file

void partition_file_data(char *input_file, int n, char *blocks_folder) {
    FILE *fp;
    size_t block_size, remaining_bytes, i;

    // Open the input file
    fp = fopen(input_file, "r");
    if (!fp) {
        perror("Failed to open the input file");
        exit(1);
    }

    // Determine the size of the file
    fseek(fp, 0, SEEK_END);
    size_t file_size = ftell(fp);
    rewind(fp);

    // Calculate the size of each block
    block_size = file_size / n;
    remaining_bytes = file_size % n;

    // Create N blocks and write data to each block
    for (i = 0; i < n; i++) {
        int current_block_size;
        if(i == (n-1)){
            current_block_size = block_size + remaining_bytes;
        } else {
            current_block_size = block_size;
        }

        char file_path[PATH_MAX];
        sprintf(file_path, "%s/%ld.txt", blocks_folder, i);
        //printf("%s\n", file_path);

        FILE *new_file = fopen(file_path, "a");

        char buffer[1000000];
        fread(buffer, 1, current_block_size, fp);

        fprintf(new_file, "%s", buffer);

        fclose(new_file);
    }

    // Close the input file
    fclose(fp);
    printf("file partitioned\n---\n");
}



// ##### DO NOT MODIFY THIS FUNCTION #####
void setup_output_directory(char *block_folder, char *hash_folder) {
    // Remove output directory
    pid_t pid = fork();
    if (pid == 0) {
        char *argv[] = {"rm", "-rf", "./output/", NULL};
        if (execvp(*argv, argv) < 0) {
            printf("ERROR: exec failed\n");
            exit(1);
        }
        exit(0);
    } else {
        wait(NULL);
    }

    sleep(1);

    // Creating output directory
    if (mkdir("output", 0777) < 0) {
        printf("ERROR: mkdir output failed\n");
        exit(1);
    }

    sleep(1);

    // Creating blocks directory
    if (mkdir(block_folder, 0777) < 0) {
        printf("ERROR: mkdir output/blocks failed\n");
        exit(1);
    }

    // Creating hashes directory
    if (mkdir(hash_folder, 0777) < 0) {
        printf("ERROR: mkdir output/hashes failed\n");
        exit(1);
    }
}
