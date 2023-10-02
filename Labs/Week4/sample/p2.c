#include <stdio.h>
#include <unistd.h>
#include <string.h>

// ./main < input1 > output1
int main(int argc, char *argv[]){
    #ifdef CASE1
        int arg1;
        scanf("%d", &arg1);
        char arg2[10];
        scanf("%s", arg2);
        printf("Implicit\n");
        printf("Argument [0]: %d \nArgument [1]: %s\n", arg1, arg2);
    #endif


    #ifdef CASE2
        int i = 0;
        char w_buf[20] = "Explicit\n";
        write(STDOUT_FILENO, w_buf, strlen(w_buf));
        char r_buf[20];
        while(read(STDIN_FILENO, r_buf, 20) != 0){
            write(STDOUT_FILENO, r_buf, strlen(r_buf));
        }
    #endif
    return 0;
}