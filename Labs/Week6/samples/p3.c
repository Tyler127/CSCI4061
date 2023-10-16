#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int *arr = (int *) malloc (sizeof(int) * 10);
    for(int i = 0; i < 11; i++){
        arr[i] = i;
    }

    free(arr);
    arr = NULL;
    return 0;
}