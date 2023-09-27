#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
    pid_t pid = fork();
    if(pid == 0){ // Child
        printf("Child [%d] with parent [%d] executing ls -l\n", getpid(), getppid());
        execl("/bin/ls", "ls", "-l", NULL);
    } else {
        wait(NULL);
        printf("Parent [%d] executing ls -la\n", getpid());
        execl("/bin/ls", "ls", "-la", NULL);
    }
    return 0;
}
