#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    if (argc < 3) {
        printf("Usage: %s <M> <N>\n", argv[0]);
        return 1;
    }

    int M = atoi(argv[1]);
    int N = atoi(argv[2]);

    // Create an MxN matrix using dynamic allocation
    int **mat = (int **)malloc(sizeof(int *) * M);
    for(int i = 0; i < M; i++){
        mat[i] = (int *)malloc(sizeof(int) * N); // allocate memory for each row
        for(int j = 0; j < N; j++){
            mat[i][j] = i + j; // initialize values
        }
    }

    // create another MxN matrix matcopy which stores negative values of mat
    int **matcopy = (int **)malloc(sizeof(int *) * M);
    for(int i = 0; i < M; i++){
        matcopy[i] = (int *)malloc(sizeof(int) * N); // allocate memory for each row
        for(int j = 0; j < N; j++){
            matcopy[i][j] = -mat[i][j]; // store negative values
        }
    }

    // print the sum of mat and matcopy values (should all be zero)
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            printf("%d ", mat[i][j] + matcopy[i][j]);
        }
        printf("\n");
    }

    // free memory for mat
    for(int i = 0; i < M; i++){
        free(mat[i]); // free each row
    }
    free(mat); // free the array of pointers
    mat = NULL; // good practice after freeing

    // free memory for matcopy
    for(int i = 0; i < M; i++){
        free(matcopy[i]); // free each row
    }
    free(matcopy); // free the array of pointers
    matcopy = NULL; // good practice after freeing

    return 0;
}
