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

    // TODO: If the current process is a leaf process, read in the associated block file 
    // and compute the hash of the block.

    // TODO: If the current process is not a leaf process, spawn two child processes using  
    // exec() and ./child_process. 

    // TODO: Wait for the two child processes to finish

    // TODO: Retrieve the two hashes from the two child processes from output/hashes/
    // and compute and output the hash of the concatenation of the two hashes.

    int N = atoi(argv[3]);
    int id = atoi(argv[4]);

    int left_id = 2 * id + 1;
    int right_id = 2 * id + 2;

    if (N - 1 <= id && id <= 2 * N - 2) { // Leaf node
        // ... rest of your code ...
    } else if (id < N - 1) { // Not a leaf node
        pid_t left_child = fork();
        if (left_child == 0) { // Left child process
            char left_id_str[10];
            snprintf(left_id_str, 10, "%d", left_id);
            execl("./child_process", "./child_process", argv[1], argv[2], argv[3], left_id_str, (char *)NULL);
        } else if (left_child > 0) {
            pid_t right_child = fork();
            if (right_child == 0) { // Right child process
                char right_id_str[10];
                snprintf(right_id_str, 10, "%d", right_id);
                execl("./child_process", "./child_process", argv[1], argv[2], argv[3], right_id_str, (char *)NULL);
            } else if (right_child > 0) {
                wait(NULL); // Wait for the right child to complete
            } else {
                perror("Right child fork failed");
                exit(1);
            }
            wait(NULL); // Wait for the left child to complete
        } else {
            perror("Left child fork failed");
            exit(1);
        }

        // ... rest of your code for hashing and writing output ...
    }
    
    char block_file[PATH_MAX];
    char hash_file[PATH_MAX];
    char left_hash_file[PATH_MAX];
    char right_hash_file[PATH_MAX];
    char hash[HASH_SIZE * 2 + 1];  // Assuming HASH_SIZE is the size of the hash in bytes

    if (N - 1 <= id && id <= 2 * N - 2) { // Leaf node
        sprintf(block_file, "%s/%d.txt", argv[1], id - (N - 1));
        sprintf(hash_file, "%s/%d.out", argv[2], id);
        FILE *block_fp = fopen(block_file, "r");
        if (!block_fp) {
            perror("Error opening block file");
            exit(1);
        }
        hash_block(block_fp, hash);  // Assume hash_block is a function that hashes the contents of a file
        fclose(block_fp);

        FILE *hash_fp = fopen(hash_file, "w");
        if (!hash_fp) {
            perror("Error opening hash file");
            exit(1);
        }
        fprintf(hash_fp, "%s", hash);
        fclose(hash_fp);
    } else if (id < N - 1) { // Not a leaf node
        // ... rest of your code ...

        sprintf(left_hash_file, "%s/%d.out", argv[2], left_id);
        sprintf(right_hash_file, "%s/%d.out", argv[2], right_id);
        FILE *left_hash_fp = fopen(left_hash_file, "r");
        FILE *right_hash_fp = fopen(right_hash_file, "r");
        if (!left_hash_fp || !right_hash_fp) {
            perror("Error opening hash files");
            exit(1);
        }

        char left_hash[HASH_SIZE * 2 + 1];
        char right_hash[HASH_SIZE * 2 + 1];
        fscanf(left_hash_fp, "%s", left_hash);
        fscanf(right_hash_fp, "%s", right_hash);
        fclose(left_hash_fp);
        fclose(right_hash_fp);

        combine_hashes(hash, left_hash, right_hash);  // Assume combine_hashes is a function to combine and hash two hash values

        sprintf(hash_file, "%s/%d.out", argv[2], id);
        FILE *hash_fp = fopen(hash_file, "w");
        if (!hash_fp) {
            perror("Error opening hash file");
            exit(1);
        }
        fprintf(hash_fp, "%s", hash);
        fclose(hash_fp);
    }

    return 0;
}