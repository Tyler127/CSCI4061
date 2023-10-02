#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){

    int fd[2];
    int ret = pipe(fd);
    if(ret == -1){
        printf("Error creating pipe...\n");
        exit(-1);
    }
    pid_t pid = fork();
    if(pid == 0){// child
        int nbytes = 0;
        int value;
        
        // close write end
        close(fd[1]);

        while((nbytes = read(fd[0], &value, sizeof(int))) != 0){
            printf("Child received %d -- %d\n", value, nbytes);
        }
        close(fd[0]);

    } else {
        // close read end
        close(fd[0]);
        
        for(int i = 0; i < 10; i++){
            write(fd[1], &i, sizeof(int));
            printf("Parent sent %d\n", i);
        }
        close(fd[1]);
        wait(NULL);
    }
    return 0;
}