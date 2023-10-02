/*
Check p3.c and p4.c
All the required code for the activity is present in the mentioned samples
You will have to work a bit more to figure out certain small details
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define WRITE (O_WRONLY | O_CREAT | O_TRUNC)
#define PERM (S_IRUSR | S_IWUSR)

// check p3.c for MODE and FLAG macros

int main(int argc, char *argv[]){
    // accept single argument n and convert to integer
    int chainLength = atoi(argv[1]);

    // char array to store the pids to
    char processIDsString[200] = "";

    pid_t pid;
    for(int i; i<chainLength; i++){ // create process chain
        // declare required variables that must be cloned by child from parent (pipes, character arrays, think about this)
        int pipe1[2];
        int ret = pipe(pipe1);
        if(ret == -1){
            printf("Error creating pipe...\n");
            exit(-1);
        }

        pid = fork();
        // fork
        if(pid != 0){ // parent
            // create a string to put the current pid into
            char pidString[200];
            sprintf(pidString, "%d", getpid());

            // concatenate a space and the curr pid into the main pid string
            strcat(processIDsString, " ");
            strcat(processIDsString, pidString);

            // parent sends whatever it received +  its process ID to child (p4.c)
            write(pipe1[1], &processIDsString, sizeof(processIDsString));
           
            // ensure to close pipe ends properly
            close(pipe1[0]); // close read end
            close(pipe1[1]); // close write end

            // wait for child and break
            wait(NULL);
            break; // break so then the current child becomes parent next loop iteration
        } else if(pid == 0){// child
            // read data from parent, not that the data size from parent is unknow, you may have to read the pipe in a loop to ensure all data is received
            // ensure to close pipe ends properly
            // close write end
            close(pipe1[1]); 

            // read the string in pipe into processIDsString to replace it 
            read(pipe1[0], &processIDsString, sizeof(processIDsString)); 

            // close read end
            close(pipe1[0]); 

            // if the current process is the final process
            if(i==chainLength-1){
                // create a new output file
                int file = open("out.txt", WRITE, PERM);

                //      1. redirect STDOUT to out.txt (p3.c)
                // redirect standard ouput to the file
                // create a duplicate of STDOUT_FILENO
                int TEMP_STDOUT_FILENO = dup(STDOUT_FILENO);

                 // closes STDOUT_FILENO and then copies fd to STDOUT_FILENO
                 if(dup2(file, STDOUT_FILENO) == -1){
                    perror("Failed to redirect output\n");
                    exit(-1);
                }

                // write to file and flush stdout so its not stuck in buffer
                printf("%s\n", processIDsString);
                fflush(stdout);

                // this also works to write to file:
                // write(file, processIDsString, strlen(processIDsString));
                
                //      2. restore STDOUT
                if(dup2(TEMP_STDOUT_FILENO, STDOUT_FILENO) == -1){
                    perror("Failed to restore output\n");
                    exit(-1);
                }
                close(TEMP_STDOUT_FILENO);

                // print string to terminal too after stdout restored
                printf("%s\n", processIDsString);
            }
        }
    }
    return 0;
}