#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include "hash.h"

#define PATH_MAX 1024

int main(int argc, char* argv[]) {
    if (argc != 5) {
        printf("Usage: ./child_process <blocks_folder> <hashes_folder> <N> <child_id>\n");
        return 1;
    }

    // Create variables from argv array
    char* blocks_folder = argv[1];
    char* hashes_folder = argv[2];
    int N = atoi(argv[3]);
    int id = atoi(argv[4]);

    int left_id = 2 * id + 1;
    int right_id = 2 * id + 2;
    pid_t left_child;
    pid_t right_child;

    printf("child_process id: %d\n", id);

    // If the current process is a leaf process, read in the associated block file 
    // and compute the hash of the block.
    if((N - 1 <= id) && (id <= 2 * N - 2)){
        printf("    - leaf node\n");
        // Determine which block file num needs to be hashed by this process
        int fileToHash = id - N + 1; 

        // Create a path to the block file that needs to be hashed
        char block_path[PATH_MAX];
        sprintf(block_path, "%s/%d.txt", blocks_folder, fileToHash);

        // Hash the block file
        char block_hash[SHA256_BLOCK_SIZE * 2 + 1];
        hash_data_block(block_hash, block_path);

        // Create a path for the new hash file using the process id
        char hash_path[PATH_MAX];
        sprintf(hash_path, "%s/%d.out", hashes_folder, id);

        // Create a new file and write the block's hash to it
        FILE *new_file = fopen(hash_path, "a");
        fprintf(new_file, "%s", block_hash);

        // Close the file and exit the process
        fclose(new_file);
        return 0;
    } 
    // Else if the current process is not a leaf process, spawn two child processes using  
    // exec() and ./child_process. 
    else if (id < N - 1){
        printf("    - non-leaf node\n");

        // turn N into a string
        char n_str[10];
        sprintf(n_str, "%d", N);

        // Fork to create left child
        left_child = fork();

        // Left child process execs into new child_process
        if (left_child == 0) { 
            char left_id_str[10];
            sprintf(left_id_str, "%d", left_id);
            execl("./child_process", "./child_process", blocks_folder, hashes_folder, n_str, left_id_str, NULL);
            perror("Error executing left child process");
        } 
        // Else parent process activities
        else if (left_child > 0) {
            printf("    - %d's left child created\n", id);
            // Fork to create right child
            right_child = fork();

            // Right child process execs into new child_process
            if (right_child == 0) { 
                char right_id_str[10];
                sprintf(right_id_str, "%d", right_id);
                execl("./child_process", "./child_process", blocks_folder, hashes_folder, n_str, right_id_str, NULL);
                perror("Error executing right child process");
            } else if (right_child > 0) {
                printf("    - %d's right child created\n", id);
                waitpid(right_child, NULL, 0); // Waits for right child to complete
            } else {
                perror("Right child fork failed");
                return 1;
            }

            waitpid(left_child, NULL, 0); // Waits for the left child to complete
        } else {
            perror("Left child fork failed");
            return 1;
        }
    }

    // Retrieve the two hashes from the two child processes from output/hashes/
    // and compute and output the hash of the concatenation of the two hashes.
    printf("pid: %d is computing combined hash\n", id);
    printf("    pid: %d looking for left: %d and right: %d\n", id, left_id, right_id);
    // Open the left and right hash files
    char left_hash_file[PATH_MAX];
    char right_hash_file[PATH_MAX];
    sprintf(left_hash_file, "%s/%d.out", hashes_folder, left_id);
    sprintf(right_hash_file, "%s/%d.out", hashes_folder, right_id);
    printf("pid %d is opening: %s and %s\n", id, left_hash_file, right_hash_file);
    FILE *left_hash_fp = fopen(left_hash_file, "r");
    FILE *right_hash_fp = fopen(right_hash_file, "r");

    // Error if either file wasn't opened
    if (!left_hash_fp || !right_hash_fp) {
        perror("Error opening hash files");
        return 1;
    }

    // Read the left and right hashes into strings then close the files
    char left_hash[SHA256_BLOCK_SIZE * 2 + 1];
    char right_hash[SHA256_BLOCK_SIZE * 2 + 1];
    fscanf(left_hash_fp, "%s", left_hash);
    fscanf(right_hash_fp, "%s", right_hash);
    fclose(left_hash_fp);
    fclose(right_hash_fp);

    // Compute the hash of both files
    char result_hash[SHA256_BLOCK_SIZE * 2 + 1];
    compute_dual_hash(result_hash, left_hash, right_hash);

    
    char hash_file[PATH_MAX];
    sprintf(hash_file, "%s/%d.out", hashes_folder, id);

    printf("pid: %d is outputing mega hash\n", id);
    FILE *hash_fp = fopen(hash_file, "w");
    if (!hash_fp) {
        perror("Error opening hash file");
        exit(1);
    }
    fprintf(hash_fp, "%s", result_hash);


    fclose(hash_fp);
    return 0;
}