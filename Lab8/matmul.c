#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

struct Arguments{
    int **A;
    int **B;
    int **C;
    int row_start;
    int row_end;
    int M;
    int N;
};

typedef struct Arguments Args;

// Print time taken in ms
void timeTaken(struct timeval *start, struct timeval *end){
    long sec = (end->tv_sec - start->tv_sec);
    long mic = (end->tv_usec - start->tv_usec);
    long micros = (sec * 1000) + (mic / 1000);

    fprintf(stdout, "Time: %ld ms\n", micros);
}

/***************Sequential code******************/
void seqMatMul(Args *args){
    for(int i = 0; i < args->M; i++){
        for(int j = 0; j < args->N; j++){
            args->C[i][j] = args->A[i][j] + args->B[i][j];
        }
    }
}

int **sequentialCode(int M, int N){
    int **A, **B, **C;
    A = (int **)malloc(sizeof(int*) * M);
    B = (int **)malloc(sizeof(int*) * M);
    C = (int **)malloc(sizeof(int*) * M);
    for(int i = 0; i < M; i++){
        A[i] = (int *)malloc(sizeof(int) * N);
        B[i] = (int *)malloc(sizeof(int) * N);
        C[i] = (int *)malloc(sizeof(int) * N);
    }
    for(int i = 0; i < M; i++)
        for(int j = 0; j < N; j++){
            A[i][j] = i + j;
            B[i][j] = i - j;
            C[i][j] = 0;
        }

    Args seq_args;
    seq_args.A = A;
    seq_args.B = B;
    seq_args.C = C;
    seq_args.M = M;
    seq_args.N = N;

    // Timing sequential code
    struct timeval start, end;
    gettimeofday(&start, NULL);

    seqMatMul(&seq_args);
    
    gettimeofday(&end, NULL);
    fprintf(stdout, "Sequential ");
    timeTaken(&start, &end);

    // Free malloced memory
    for(int i = 0; i < M; i++){
        free(A[i]);
        free(B[i]);
    }
    free(A);
    A = NULL;
    free(B);
    B = NULL;
    
    return C;
}


/***************Parallel code******************/
void *parMatMul(void *targs){
    // cast targs pointer to Args* and store in args
    Args *args = (Args *) targs;
    for(int i = args->row_start; i < args->row_end; i++)
        for(int j = 0; j < args->N; j++)
            args->C[i][j] = args->A[i][j] + args->B[i][j];
    return NULL; // thread returns NULL as it doesn't need to return any value
}


int **parallelCode(int M, int N, int T){
    int **A, **B, **C;
    A = (int **)malloc(sizeof(int*) * M);
    B = (int **)malloc(sizeof(int*) * M);
    C = (int **)malloc(sizeof(int*) * M);
    for(int i = 0; i < M; i++){
        A[i] = (int *)malloc(sizeof(int) * N);
        B[i] = (int *)malloc(sizeof(int) * N);
        C[i] = (int *)malloc(sizeof(int) * N);
    }
    for(int i = 0; i < M; i++)
        for(int j = 0; j < N; j++){
            A[i][j] = i + j;
            B[i][j] = i - j;
            C[i][j] = 0;
        }
    pthread_t thds[T];
    Args par_args[T]; // create separate argument structure per thread and assign
    int n_rows_per_thread = M / T;
    int row_start = 0, row_end = n_rows_per_thread;
    // Timing parallel code
    struct timeval start, end;
    gettimeofday(&start, NULL);
    // create T threads calling parMatMul
    for (int i = 0; i < T; i++){
        par_args[i].A = A;
        par_args[i].B = B;
        par_args[i].C = C;
        par_args[i].M = M;
        par_args[i].N = N;
        par_args[i].row_start = row_start;
        par_args[i].row_end = row_end;
        
        pthread_create(&thds[i], NULL, parMatMul, &par_args[i]);

        // Adjust for next iteration
        row_start = row_end;
        row_end += n_rows_per_thread;
    }
    // Join all the created threads in a loop
    for (int i = 0; i < T; i++) {
        pthread_join(thds[i], NULL);
    }
    gettimeofday(&end, NULL);
    fprintf(stdout, "Parallel ");
    timeTaken(&start, &end);
    // Free malloced memory
    for(int i = 0; i < M; i++){
        free(A[i]);
        free(B[i]);
    }
    free(A);
    A = NULL;
    free(B);
    B = NULL;
    return C;
}


// main driver
int main(int argc, char *argv[]){
    if(argc != 4){
        fprintf(stderr, "Usage ./main <Rows> <Columns> <Threads>");
        exit(EXIT_FAILURE);
    }

    int M = atoi(argv[1]);
    int N = atoi(argv[2]);
    int T = atoi(argv[3]);

    if(M%T != 0){
        printf("Rows should be divisible by Threads");
        exit(EXIT_FAILURE);
    }

    // Sequential Code
    int **s_res = sequentialCode(M, N);

    // Parallel Code
    int **p_res = parallelCode(M, N, T);

    // Chek if the results from sequential and parallel code are the same
    int same = 1;
    for(int i = 0; i < M && same; i ++){
        for(int j = 0; j < N && same; j++){
            if(s_res[i][j] != p_res[i][j]){
                same = 0;
            }
        }
    }

    // free malloced memory
    for(int i = 0; i < M; i++){
        free(s_res[i]);
        free(p_res[i]);
    }
    free(s_res);
    s_res = NULL;
    free(p_res);
    p_res = NULL;

    if(!same)
        fprintf(stdout, "Seq C != Par C\n");
    else
        fprintf(stdout, "Seq C = Par C\n");


    return 0;
}