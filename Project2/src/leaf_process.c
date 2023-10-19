#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/hash.h"
#include "../include/utils.h"

char *output_file_folder = "output/inter_submission/";

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: Inter Submission --> ./leaf_process <file_path> 0\n");
        printf("Usage: Final Submission --> ./leaf_process <file_path> <pipe_write_end>\n");
        return -1;
    }
    
    // Get <file_path> <pipe_write_end> from argv[]
    char* file_path = argv[1];
    int pipe_write_end = atoi(argv[2]);
    printf("IN LEAF PROCESS for %s\n", file_path);

    // Create the hash of given file
    char hash_value[SHA256_BLOCK_SIZE * 2 + 1];
    memset(hash_value, 0, sizeof(hash_value));
    hash_data_block(hash_value, file_path);

    // Construct string write to pipe. The format is "<file_path>|<hash_value>"
    char* to_pipe_buffer = malloc(strlen(file_path) + strlen(hash_value) + 3);
    sprintf(to_pipe_buffer, "%s|%s|", file_path, hash_value);
    printf("    to_pipe_buffer: %s\n", to_pipe_buffer);

    if (pipe_write_end == 0){
        // intermediate submission
        // overview: create a file in output_file_folder("output/inter_submission/root*") and write the constructed string to the file

        // Extract the file_name from file_path using extract_filename() in utils.c
        char* file_name = extract_filename(file_path);
        printf("        file name: %s\n", file_name);

        // Extract the root directory(e.g. root1 or root2 or root3) from file_path using extract_root_directory() in utils.c
        char* root_dir = extract_root_directory(file_path);
        printf("        root dir: %s\n", root_dir);

        // Get the location of the new file (e.g. "output/inter_submission/root1" or "output/inter_submission/root2" or "output/inter_submission/root3")
        char con_string_output_file_path[PATH_MAX];
        sprintf(con_string_output_file_path, "%s%s%s", output_file_folder, root_dir, file_name);
        printf("    output file path: %s\n", con_string_output_file_path);

        // Create and write to file, and then close file
        FILE* new_file = fopen(con_string_output_file_path, "a");
        fprintf(new_file, "%s", to_pipe_buffer);

        // Free any arrays that are allocated using malloc!! Free the string returned from extract_root_directory()!! It is allocated using malloc in extract_root_directory()
        fclose(new_file);
        free(root_dir);
        free(to_pipe_buffer);
    } else {
        // Final submission: write the string to pipe
        write(pipe_write_end, to_pipe_buffer, strlen(to_pipe_buffer)); //might need sizeof here instead?
        free(to_pipe_buffer);
        printf("END\n");
    }

    exit(0);
}



