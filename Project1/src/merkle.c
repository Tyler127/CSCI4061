#include "utils.h"
#include "print_tree.h"

// ##### DO NOT MODIFY THESE VARIABLES #####
char *blocks_folder = "output/blocks";
char *hashes_folder = "output/hashes";
char *visualization_file = "output/visualization.txt";


int main(int argc, char* argv[]) {
    if (argc != 3) {
        // N is the number of data blocks to split <file> into (should be a power of 2)
        // N will also be the number of leaf nodes in the merkle tree
        printf("Usage: ./merkle <file> <N>\n");
        return 1;
    }

    // Read in the command line arguments and validate them
    char *input_file = argv[1];
    printf("%s\n", input_file);
    int n = atoi(argv[2]);
    if (n <= 0 || (n & (n - 1)) != 0) { // Check if N is a power of 2
        printf("ERROR: N should be a positive power of 2\n");
        return 1;
    }

    // ##### DO NOT REMOVE #####
    setup_output_directory(blocks_folder, hashes_folder);

    // Partition file data into N blocks
    partition_file_data(input_file, n, blocks_folder);


    // Start the recursive merkle tree computation by spawning first child process (root)
    pid_t root_process = fork();
    // Child process (root of the merkle tree)
    if (root_process == 0) { 
        execl("./child_process", "./child_process", blocks_folder, hashes_folder, argv[2], "0", (char *)NULL);

        // execl will only return if there is an error executing the process
        perror("execl");  
        exit(1);
    } else if (root_process > 0) { // Parent process
        wait(NULL); // Wait for the child process to complete
    } else {
        perror("fork"); // handles any error that would come from forking
        exit(1);
    }

    // ##### DO NOT REMOVE #####
    #ifndef TEST_INTERMEDIATE
        // Visually display the merkle tree using the output in the hashes_folder
        print_merkle_tree(visualization_file, hashes_folder, n);
    #endif

    return 0;
}
