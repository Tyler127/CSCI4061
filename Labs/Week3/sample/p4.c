#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void filePointerCheck(char *filename){
    FILE *fp = fopen(filename, "a+");

    printf("filedes before fork - %d\n", fileno(fp));

    pid_t pid = fork();
    if(pid == 0){
        printf("filedes of child [%d] - [%d]\n", getpid(), fileno(fp));
        fclose(fp);
        exit(0);
    } else {
        printf("filedes of parent [%d] - [%d]\n", getpid(), fileno(fp));
    }
    fclose(fp);
}

void tokenRead(char *filename){
    FILE *fp = fopen(filename, "a+");

    pid_t parent_id = getpid();
    pid_t pid = -1;
    for(int i = 0; i < 3; i++){
        if(!(pid = fork())){
            fprintf(fp, "%d -- %d\n", getpid(), getppid()); // blocking call
            break;
        }
    }
    // current position in file
    long pos = ftell(fp);
    printf("File pointer position for process [%d]: [%ld]\n", getpid(), pos);
    
    if(parent_id == getpid()){ // parent waits for all children to exit
        while(wait(NULL) > 0);
        pos = ftell(fp);
        printf("File pointer position for parent process [%d]: [%ld]\n", getpid(), pos);

        // move file pointer to start of file
        fseek(fp, 0, SEEK_SET);

        char buffer[1024];
        int ret;
        while(1){
            ret = fscanf(fp, "%s", buffer); // reads until a space or new line, whichever comes first
            if (ret == EOF) break;
            printf("%s\n", buffer);
        }
        fclose(fp);
    } else {
        // terminate children
        fclose(fp);
        exit(0);
    }
    
    
    
}

void lineRead(char *filename) {
    FILE *fp = fopen(filename, "a+");

    pid_t parent_id = getpid();
    pid_t pid = -1;
    for(int i = 0; i < 3; i++){
        if(!(pid = fork())){
            fprintf(fp, "%d -- %d\n", getpid(), getppid()); // blocking call
            break;
        }
    }
    // current position in file
    long pos = ftell(fp);
    printf("File pointer position for process [%d]: [%ld]\n", getpid(), pos);
    
    if(parent_id == getpid()){ // parent waits for all children to exit
        while(wait(NULL) > 0);
        pos = ftell(fp);
        printf("File pointer position for parent process [%d]: [%ld]\n", getpid(), pos);

        // move file pointer to start of file
        fseek(fp, 0, SEEK_SET);

        char buffer[1024];
        char *ret = NULL;
        while(1){
            ret = fgets(buffer, 1024, fp); // reads 1024 bytes or until a new line whichever comes first
            if (ret == NULL) break;
            printf("%s", buffer);
        }
        fclose(fp);
    } else{
        // terminate children
        fclose(fp);
        exit(0);
    }
}

int main(int argc, char * argv[]){
    char *filename = argv[1];
    char *choice = argv[2];

    switch(*choice){
        case '1':
            filePointerCheck(filename);
            break;
        case '2':
            tokenRead(filename);
            break;
        case '3':
            lineRead(filename);
            break;
        default:
            printf("Incorrect choice\n");
    }
    return 0;
}