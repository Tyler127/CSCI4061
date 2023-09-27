#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]){
	int x = 1;
	pid_t pid = fork();
    if(pid == 0){ // Child
        x++;
        printf("Value of x in child [%d] with parent [%d] = [%d]\n",
                                        getpid(), getppid(), x);
        printf("Parent of child process [%d] = [%d]\n", getpid(), getppid());
    } else {
        x += 3;
        printf("Value of x in parent process [%d] = [%d]\n",
                                    getpid(), x);
        // waits for pid child to complete execution
        // waitpid(pid, NULL, 0);

        // Just check if pid child has terminated and return
        // int status;
        // waitpid(pid, &status, WNOHANG);
        
        // Wait for any one child
        // wait(NULL);
    }
	return 0;
}
