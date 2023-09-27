#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    // output filename and length of chain as arguments
    if(argc != 3){
        printf("Incorrect number of arguments...\n");
        exit(-1);
    }

    // store length of chain in a variable
    int chainLength = atoi(argv[2]) - 1; // take off 1 to include main process in the chain
    pid_t pid;

    for(int i=0; i<chainLength; i++){ // spawn processes in a chain manner
        // parent process breaks out of loop, child proceeds to next iteration
        pid = fork();
        if(pid > 0){
            break;
        }
    }

    // parent waits for its child
    waitpid(pid, NULL, 0);

    // get child pid
    pid = getpid();

    // get parent pid
    pid_t ppid = getppid();

    // write the child pid and parent pid to chain2_out.txt
    // close file after writing
    FILE* file = fopen(argv[1], "a");
    fprintf(file, "%d -- %d\n", pid, ppid);
    fclose(file);

    return 0;
}

