#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define WRITE (O_WRONLY | O_CREAT | O_TRUNC)
#define PERM (S_IRUSR | S_IWUSR)

int main(int argc, char *argv[]){

    int fd = open(argv[1], WRITE, PERM);
    if(fd == -1){
        perror("Failed to open file\n");
        exit(-1);
    }

    // redirect standard ouput to the file
    // create a duplicate of STDOUT_FILENO
    int TEMP_STDOUT_FILENO = dup(STDOUT_FILENO);

    // closes STDOUT_FILENO and then copies fd to STDOUT_FILENO
    if(dup2(fd, STDOUT_FILENO) == -1){
        perror("Failed to redirect output\n");
        exit(-1);
    }

    printf("Writing to file using STDOUT_FILENO\n");
    // required fflush as printf will buffer data in memory and not write to the file directly
    fflush(stdout);

    char buf[] = "Writing to file using fd\n";
    write(fd, buf, strlen(buf));
    close(fd);

    printf("Writing to file after closing fd\n");
    // required fflush as printf will buffer data in memory and not write to the file directly
    fflush(stdout);

    // closes STDOUT_FILENO and then copies TEMP_STDOUT_FILENO to STDOUT_FILENO
    if(dup2(TEMP_STDOUT_FILENO, STDOUT_FILENO) == -1){
        perror("Failed to restore output\n");
        exit(-1);
    }
    close(TEMP_STDOUT_FILENO);

    printf("Writing after restoring STDOUT_FILENO\n");

    return 0;
}