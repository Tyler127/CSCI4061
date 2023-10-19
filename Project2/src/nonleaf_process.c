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
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[1024];
        snprintf(path, sizeof(path), "%s/%s", directory_path, entry->d_name);
        struct stat path_stat;
        stat(path, &path_stat);

        int child_pipe[2];
        if (pipe(child_pipe) == -1) {
            perror("pipe failed");
            exit(EXIT_FAILURE);
        }

        pid_t child_pid = fork();
        if (child_pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) {  // In child process
            close(child_pipe[0]);  // Close unused read end

            if (S_ISREG(path_stat.st_mode)) {  // If it's a file
                execl("./leaf_process", "leaf_process", path, child_pipe[1], NULL);
                perror("execl failed");  // execl only returns on failure
                exit(EXIT_FAILURE);
            } else if (S_ISDIR(path_stat.st_mode)) {  // If it's a directory
                execl("./nonleaf_process", "nonleaf_process", path, child_pipe[1], NULL);
                perror("execl failed");
                exit(EXIT_FAILURE);
            }
        } else {  // In parent process
            close(child_pipe[1]);  // Close unused write end
            pipe_read_ends[num_children++] = child_pipe[0];
        }
    }

    // Wait for all child processes and read their data
    for (int i = 0; i < num_children; i++) {
        char buffer[4096];
        ssize_t count;
        
        // Read from pipe, check for errors
        while ((count = read(pipe_read_ends[i], buffer, sizeof(buffer))) != 0) {
            if (count == -1) {
                if (errno == EINTR) {
                    continue;
                } else {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
            }

            // Append to child_data_holder
            strncat(child_data_holder, buffer, count);
        }

        close(pipe_read_ends[i]);  // Close the pipe read end
    }

    free(pipe_read_ends);
    write(pipe_write_end, child_data_holder, strlen(child_data_holder) + 1);  // Write to parent pipe, include NULL terminator
    close(pipe_write_end);
    
    // Read from pipe constructed for child process and write to pipe constructed for parent process
    sprintf(child_data_holder, "test");
    printf("    child_data_holder: %s\n", child_data_holder);
    write(pipe_write_end, child_data_holder, strlen(child_data_holder));

    free(child_data_holder);
    closedir(dir);
    printf("END\n");
    return 0;
}
