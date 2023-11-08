#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

int loop = 1;

// Handle Ctrl + C
void signalHandler(int signum){
    loop = 0;
}

int main(int argc, char *argv[]){
    // Signal handling will be taught towards end of semester
    signal(SIGINT, signalHandler);

    char *str = (char *)malloc(sizeof(char) * 10);
    memset(str, 0, 10 * sizeof(char));
    strcpy(str, "Hello");

    long i = 0;
    while(loop){
        printf("[%ld] %s [%p]\n", i, str, str);
        sleep(2);
        i++;
    }

    free(str);
    str = NULL;

    return 0;
}