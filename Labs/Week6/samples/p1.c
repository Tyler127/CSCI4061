#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int arr_stat[10]; // static allocation of an array of size 10
    int *arr_dyn = (int *) malloc (sizeof(int) * 10); // dynamic allocation 
                                                      // of an array of size 10
    for(int i = 0; i < 10; i++){
        arr_stat[i] = i;
        arr_dyn[i] = i;
    }

    printf("static size - %ld\n", sizeof(arr_stat));
    printf("dynamic size - %ld\n", sizeof(arr_dyn));

    free(arr_dyn); // frees allocated heap memory
    arr_dyn = NULL; // avoids double free
    return 0;
}