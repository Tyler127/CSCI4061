#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char *argv[]){
    if(argc != 2){ // There should only one input argument, otherwise error out
        printf("One one input argument is accepted!\n");
        exit(-1);
    }
    
    // use atoi to convert argv[1] to integer and store in n_procs
    int n_procs;
    n_procs = atoi(argv[1]);

    // save pid of the main parent process
    pid_t pid;
    pid = getpid();
    for(int i = 1; i <= n_procs; i++){
        // after child is spawned, the parent continues with the loop and the child breaks out of the loop
        if(getpid() == pid){ // if the process is the parent one forks
            fork();  // fork() returns 0 for a child process. could do: pid = fork(); then: if(pid == 0){break;}
        } else {
            break;
        }
    }

    // print the child id and the parent id
    pid_t ppid = getppid();
    pid_t cid = getpid();
    printf("Child [%d] --- Parent [%d]\n", cid, ppid);

    return 0;
}

