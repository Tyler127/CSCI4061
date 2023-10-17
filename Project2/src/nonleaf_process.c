#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: ./nonleaf_process <directory_path> <pipe_write_end> \n");
        return 1;
    }
    // overview: fork the child processes(non-leaf process or leaf process) each associated with items under <directory_path>

    // Get <file_path> <pipe_write_end> from argv[]
    char* directory_path = argv[1];
    int pipe_write_end = atoi(argv[2]);
    printf("IN NONLEAF PROCESS for %s\n", directory_path);

    // Malloc buffer for gathering all data transferred from child process as in root_process.c
    char* child_data_holder = malloc(4098);

    // Open directory
    DIR* dir = opendir(directory_path);
    struct dirent *entry;

    //TODO(step4): traverse directory and fork child process
    // Hints: Maintain an array to keep track of each read end pipe of child process
    int* pipe_read_ends = (int *)malloc(sizeof(int) * 50);

    while((entry = readdir(dir)) != NULL){
        // skip . and ..
        if(strcmp(entry->d_name, ".") == 0){
            entry = readdir(dir);
        }
        if(strcmp(entry->d_name, "..") == 0){
            entry = readdir(dir);
        }

        printf("    dirname: %s\n", entry->d_name);



    }
    printf("    end of loop\n");
    
    // Read from pipe constructed for child process and write to pipe constructed for parent process
    sprintf(child_data_holder, "test");
    printf("    child_data_holder: %s\n", child_data_holder);
    write(pipe_write_end, child_data_holder, strlen(child_data_holder));

    free(child_data_holder);
    closedir(dir);
    printf("END\n");
    return 0;
}
