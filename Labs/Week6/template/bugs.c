#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[]){
    int M = atoi(argv[1]);
    int N = atoi(argv[2]);

    // Create an MxN matrix using dynamic allocation
    // create m rows, then allocate n memory locations per row
    int **mat = (int **)malloc(sizeof(int *) * M);
    for(int i = 0; i < M; i++){
        mat[i] = (int *)malloc(sizeof(int) * N);
        for(int j = 0; j < N; j++){
            mat[i][j] = i + j;
        }
    }

    // create another MxN matrix matcopy which stores negative values of mat
    // matcopy[i][j] = -mat[i][j]
    int **matcopy = (int **)malloc(sizeof(int *) * M);
    for(int i = 0; i < M; i++){
        matcopy[i] = (int *)malloc(sizeof(int) * N);
        for(int j = 0; j < N; j++){
            matcopy[i][j] = -mat[i][j];
        }
    }

    // Print sum of mat and matcopy
    for(int i = 0; i < M; i++){
        for(int j = 0; j < N; j++){
            printf("%d ", mat[i][j] + matcopy[i][j]); // zero
        }
        printf("\n");
    }

    // free memory for mat and matcopy
    // Free each row then the entire matrix. Don't forget a final NULL assignment.
    for(int i = 0; i < M; i++){
        free(mat[i]);
        free(matcopy[i]);
    }
    free(mat);
    free(matcopy);
    mat = NULL;
    matcopy = NULL;

    return 0;
}