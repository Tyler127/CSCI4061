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
// TODO(step2): redirect standard output to output file (output/final_submission/root*.txt)
// TODO(step3): read the content each symbolic link in dup_list, write the path as well as the content of symbolic link to output file(as shown in expected)

void redirection(char** dup_list, int size, char* root_dir){
    // Constructing the filename
    char output_file_path[PATH_MAX];
    snprintf(output_file_path, PATH_MAX, "%s%s.txt", output_file_folder, extract_filename(root_dir));

    // Redirecting standard output
    fflush(stdout); 
    int original_stdout = dup(STDOUT_FILENO);
    int file = open(output_file_path, WRITE, PERM);
    if (file < 0){
        perror("Failed to open file");
        exit(1);
    }
    if (dup2(file, STDOUT_FILENO) < 0){
        perror("Failed to redirect stdout");
        exit(1);
    }

    for (int i = 0; i < size; i++){
        char buffer[PATH_MAX];
        ssize_t len;

        if ((len = readlink(dup_list[i], buffer, sizeof(buffer)-1)) != -1){
            buffer[len] = '\0';
            printf("%s --> %s\n", dup_list[i], buffer);
        } else {
            perror("Failed to read symlink");
            exit(1);
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
void create_symlinks(char** dup_list, char** retain_list, int size){
    for (int i = 0; i < size; i++){
        if (symlink(retain_list[i], dup_list[i]) != 0){
            perror("Failed to create symlink");
            exit(1);
        }
    }
}

void delete_duplicate_files(char** dup_list, int size){
    for (int i = 0; i < size; i++){
        if (remove(dup_list[i]) != 0){
            perror("Failed to delete file");
            exit(1);
        }
    }
}

// ./root_directories <directory>
int main(int argc, char* argv[]){
    if (argc != 2){
        // dir is the root_directories directory to start with
        // e.g. ./root_directories/root1
        printf("Usage: ./root <dir> \n");
        return 1;
    }

    // (overview): fork the first non_leaf process associated with root directory("./root_directories/root*")

    char* root_directory = argv[1];

    // Buffer for gathering all data transferred from child process
    char all_filepath_hashvalue[4098]; 
    memset(all_filepath_hashvalue, 0, sizeof(all_filepath_hashvalue));

    // Construct pipe
    int root_pipe[2];
    int ret = pipe(root_pipe);
    if(ret == -1){
        printf("Error creating pipe in root process.\n");
        exit(-1);
    }

    // fork() child process
    pid_t child_pid = fork();
    if (child_pid == 0){
        char root_pipe_write_end_string[10];
        sprintf(root_pipe_write_end_string, "%d", root_pipe[1]);
        //printf("root dir in root: %s\n", root_directory);
        execl("./nonleaf_process", "./nonleaf_process", root_directory, root_pipe_write_end_string, NULL);
        perror("Error executing child process.");
    } 

    // Read data from pipe to all_filepath_hashvalue
    waitpid(child_pid, NULL, 0);
    read(root_pipe[0], &all_filepath_hashvalue, sizeof(all_filepath_hashvalue));
    printf("------\nROOT PROCESS all_filepath_hashvalue: \n%s\n------\n", all_filepath_hashvalue);

    // Malloc dup_list and retain list & 
    // dup_list: list of paths of duplicate files. We need to delete the files and create symbolic links at the location
    // retain_list: list of paths of unique files. We will create symbolic links for those files
    int MAX_FILES = 100;
    char** dup_list = malloc(sizeof(char *) * MAX_FILES);
    char** retain_list = malloc(sizeof(char *) * MAX_FILES);
    for (int i=0; i < MAX_FILES; i++){
        dup_list[i] = NULL;
        retain_list[i] = NULL;
    }

    // Use parse_hash() in utils.c to parse all_filepath_hashvalue
    int file_count = parse_hash(all_filepath_hashvalue, dup_list, retain_list);

    // Print out the file count and the lists
    printf("file count: %d\n------\n", file_count);
    printf("dup_list:\n");
    for (int i=0; i < file_count; i++){
        if (dup_list[i] != NULL){
            printf("    %d | %s\n", i, dup_list[i]);
        }
        if (i == file_count-1){
            printf("------\n");
        }
    }
    printf("retain_list:\n");
    for (int i=0; i < file_count; i++){
        if (retain_list[i] != NULL){
            printf("    %d | %s\n", i, retain_list[i]);
        }
        if (i == file_count-1){
            printf("------\n");
        }
    }


    //TODO(step4): implement the functions
    delete_duplicate_files(dup_list, file_count);
    create_symlinks(dup_list, retain_list, file_count);
    redirection(dup_list, file_count, argv[1]);

    // Free any arrays that are allocated using malloc
    // Cleanup
    for (int i = 0; i < file_count; i++) {
        free(dup_list[i]);
        printf("freed dup_list[%d]\n", i);
    }
    // for (int i = 0; i < file_count; i++) {
    //     free(retain_list[i]);
    //     printf("freed ret %d\n", i);
    // }
    free(dup_list);
    free(retain_list);

    printf("ROOT PROCESS DONE\n");
    return 0;
}