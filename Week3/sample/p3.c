#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	pid_t pid = fork();
    if(pid == 0){ // Child
        printf("Child [%d] with parent [%d] executing ls -l\n", getpid(), getppid());
        char *args[3] = {"ls", "-l", NULL}; 
        execv("/bin/ls", args);
    } else {
        wait(NULL);
        printf("Parent [%d] executing ls -la\n", getpid());
        char *args[3] = {"ls", "-la", NULL}; 
        execv("/bin/ls", args);
    }
	return 0;
}
