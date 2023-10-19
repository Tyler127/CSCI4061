#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include "../include/utils.h"
#include <sys/wait.h>

#define WRITE (O_WRONLY | O_CREAT | O_TRUNC)
#define PERM (S_IRUSR | S_IWUSR)
char *output_file_folder = "output/final_submission/";

    // TODO(overview): redirect standard output to an output file in output_file_folder("output/final_submission/")
    // TODO(step1): determine the filename based on root_dir. e.g. if root_dir is "./root_directories/root1", the output file's name should be "root1.txt"

    //TODO(step2): redirect standard output to output file (output/final_submission/root*.txt)

    //TODO(step3): read the content each symbolic link in dup_list, write the path as well as the content of symbolic link to output file(as shown in expected)

void redirection(char **dup_list, int size, char* root_dir) {
    // Constructing the filename
    char output_file_path[PATH_MAX];
    snprintf(output_file_path, PATH_MAX, "%s%s.txt", output_file_folder, extract_filename(root_dir));

    // Redirecting standard output
    fflush(stdout); 
    int original_stdout = dup(STDOUT_FILENO);
    int file = open(output_file_path, WRITE, PERM);
    if (file < 0) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    if (dup2(file, STDOUT_FILENO) < 0) {
        perror("Failed to redirect stdout");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < size; i++) {
        char buffer[PATH_MAX];
        ssize_t len;

        if ((len = readlink(dup_list[i], buffer, sizeof(buffer)-1)) != -1) {
            buffer[len] = '\0';
            printf("%s -> %s\n", dup_list[i], buffer);
        } else {
            perror("Failed to read symlink");
            exit(EXIT_FAILURE);
        }
    }

    // Restoring standard output
    fflush(stdout); 
    dup2(original_stdout, STDOUT_FILENO);
    close(original_stdout);
    close(file);
}


    //TODO(): create symbolic link at the location of deleted duplicate file
    //TODO(): dup_list[i] will be the symbolic link for retain_list[i]
void create_symlinks(char **dup_list, char **retain_list, int size) {
    for (int i = 0; i < size; i++) {
        if (symlink(retain_list[i], dup_list[i]) != 0) {
            perror("Failed to create symlink");
            exit(EXIT_FAILURE);
        }
    }
}



void delete_duplicate_files(char **dup_list, int size) {
    for (int i = 0; i < size; i++) {
        if (remove(dup_list[i]) != 0) {
            perror("Failed to delete file");
            exit(EXIT_FAILURE);
        }
    }
}

// ./root_directories <directory>
int main(int argc, char* argv[]) {
    if (argc != 2) {
        // dir is the root_directories directory to start with
        // e.g. ./root_directories/root1
        printf("Usage: ./root <dir> \n");
        return 1;
    }

    // (overview): fork the first non_leaf process associated with root directory("./root_directories/root*")

    char* root_directory = argv[1];
    char all_filepath_hashvalue[4098]; //buffer for gathering all data transferred from child process
    memset(all_filepath_hashvalue, 0, sizeof(all_filepath_hashvalue));// clean the buffer

    // Construct pipe
    int root_pipe[2];
    int ret = pipe(root_pipe);
    if(ret == -1){
        printf("Error creating pipe in root process.\n");
        exit(-1);
    }

    // fork() child process
    pid_t child = fork();
    if (child == 0){
        char root_pipe_write_end_string[10];
        sprintf(root_pipe_write_end_string, "%d", root_pipe[1]);
        execl("./nonleaf_process", "./nonleaf_process", root_directory, root_pipe_write_end_string, NULL);
        perror("Error executing child process.");
    } 

    // Read data from pipe to all_filepath_hashvalue
    waitpid(child, NULL, 0);
    read(root_pipe[0], &all_filepath_hashvalue, sizeof(all_filepath_hashvalue));
    printf("ROOT PROCESS all_filepath_hashvalue: %s\n", all_filepath_hashvalue);


    // malloc dup_list and retain list & use parse_hash() in utils.c to parse all_filepath_hashvalue
    // dup_list: list of paths of duplicate files. We need to delete the files and create symbolic links at the location
    // retain_list: list of paths of unique files. We will create symbolic links for those files
    char** dup_list = (char *)malloc(sizeof(char *) * 15);
    char** retain_list = (char *)malloc(sizeof(char *) * 15);
    for (int i=0; i < 15; i++){
        dup_list[i] = malloc(sizeof(1024));
        retain_list[i] = malloc(sizeof(1024));
    }

    





    //TODO(step4): implement the functions
    // delete_duplicate_files(dup_list,size);
    // create_symlinks(dup_list, retain_list, size);
    // redirection(dup_list, size, argv[1]);

    //TODO(step5): free any arrays that are allocated using malloc!!
    // Cleanup
    for (int i = 0; i < size; ++i) {
        free(dup_list[i]);
        free(retain_list[i]);
    }
    free(dup_list);
    free(retain_list);

    return 0;
}
