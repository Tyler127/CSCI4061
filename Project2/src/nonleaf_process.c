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

    // Traverse directory and fork child process
    // Maintain an array to keep track of each read end pipe of child process
    int* pipe_read_ends = malloc(sizeof(int) * 15);
    int num_children = 0;
    while ((entry = readdir(dir)) != NULL){
        // Skip over . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){
            continue;
        }
        
        // Create the path of the current file in the directory
        char path[1024];
        memset(path, 0, sizeof(path));
        sprintf(path, "%s/%s", directory_path, entry->d_name);

        // Create a pipe for the child process
        int child_pipe[2];
        if (pipe(child_pipe) == -1){
            perror("Failed to create pipe");
            exit(1);
        }

        // Fork to make the child process for the current file
        pid_t child_pid = fork();
        if (child_pid == -1){
            perror("Failed to fork");
            exit(1);
        }

        // In child process
        if (child_pid == 0){  
            // Close unused read end of the pipe
            close(child_pipe[0]);

            // Turn the child pipe write end into a string
            char child_pipe_write_end_string[10];
            sprintf(child_pipe_write_end_string, "%d", child_pipe[1]);  

            // Execl leaf process if current file is regular
            if (entry->d_type == DT_REG){ 
                execl("./leaf_process", "./leaf_process", path, child_pipe_write_end_string, NULL);
                perror("leaf_process execl failed"); 
                exit(1);
            } 
            // Execl nonleaf process if current file is a directory
            else if (entry->d_type == DT_DIR){  
                execl("./nonleaf_process", "./nonleaf_process", path, child_pipe_write_end_string, NULL);
                perror("nonleaf_process execl failed");
                exit(1);
            }
        } 
        // In parent process
        else {  
            // Close unused write end of the pipe
            close(child_pipe[1]);

            // Wait for the child process to finish
            waitpid(child_pid, NULL, 0);

            // Add the read end of the child pipe to the array
            pipe_read_ends[num_children++] = child_pipe[0];
        }
    }

    // Read from the pipes constructed for child processes
    for (int i = 0; i < num_children; i++){
        // Create a buffer to store children hash data
        char buffer[4096];
        memset(buffer, 0, sizeof(buffer));

        // Read from the pipe read end 
        int count = read(pipe_read_ends[i], &buffer, sizeof(buffer));

        // Concatenate the read string to the data holder
        strncat(child_data_holder, buffer, count);

        // Close the pipe read end
        close(pipe_read_ends[i]); 
    }

    // Write to pipe constructed for parent process
    write(pipe_write_end, child_data_holder, strlen(child_data_holder) + 1);
    printf("    child_data_holder: %s\n", child_data_holder);

    // Clean up
    close(pipe_write_end);
    closedir(dir);
    free(child_data_holder);
    free(pipe_read_ends);
    printf("END NONLEAF PROCESS for %s\n\n", directory_path);
    return 0;
}