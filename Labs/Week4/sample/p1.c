#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define READ O_RDONLY
#define WRITE (O_WRONLY|O_CREAT)
#define PERM (S_IRUSR|S_IWUSR)

#define BLKSIZE 10

int main(int argc, char *argv[]){
    // read
    #ifdef RDFILE
    int fd = open(argv[1], READ);
    if(fd == -1){
        char errmsg[] = {"Cannot open file..."};
        write(STDERR_FILENO, errmsg, strlen(errmsg));
        exit(-1);
    }

    int bytes_read = 0;
    char buf[BLKSIZE];
    while((bytes_read = read(fd, buf, BLKSIZE)) != 0){
        printf("%s", buf);
    }
    close(fd);
    #endif

    // read and write
    #ifdef RWFILE
    int fd_1 = open(argv[1], READ);
    if(fd_1 == -1){
        char errmsg[] = {"Cannot open file...\n"};
        write(STDERR_FILENO, errmsg, strlen(errmsg));
        exit(-1);
    }

    int fd_2 = open(argv[2], WRITE, PERM);
    if(fd_2 == -1){
        close(fd_1);
        char errmsg[] = {"Cannot open file...\n"};
        write(STDERR_FILENO, errmsg, strlen(errmsg));
        exit(-1);
    }

    int bytes_read = 0;
    char buf[BLKSIZE];
    while((bytes_read = read(fd_1, buf, BLKSIZE)) != 0){
        write(fd_2, buf, bytes_read);
    }
    close(fd_2);
    close(fd_1);
    #endif

    return 0;
}