#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Incorrect number of arguments...\n");
        exit(-1);
    }

    // store length of chain in a variable
    int chainLength = atoi(argv[1]) - 1; // take off 1 to include main process in the chain
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

    // get pid and convert to character array
    // use sprintf
    pid = getpid();
    char pidArray[chainLength];
    sprintf(pidArray, "%d", pid);

    // get ppid and convert to character array
    // use sprintf
    int ppid = getppid();
    char ppidArray[chainLength]; // ppidArray is alr a ptr bc it was initialized as an array
    sprintf(ppidArray, "%d", ppid);

    // use execl or execv to pass the process ids to echo
    // echo path: /bin/echo
    execl("/bin/echo", "/bin/echo", pidArray, " -- ", ppidArray, NULL);

    return 0;
}

