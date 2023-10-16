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
    //TODO(overview): fork the child processes(non-leaf process or leaf process) each associated with items under <directory_path>

    //TODO(step1): get <file_path> <pipe_write_end> from argv[]


    //TODO(step2): malloc buffer for gathering all data transferred from child process as in root_process.c


    //TODO(step3): open directory


    //TODO(step4): traverse directory and fork child process
    // Hints: Maintain an array to keep track of each read end pipe of child process


    //TODO(step5): read from pipe constructed for child process and write to pipe constructed for parent process

}
