/*
Program idea: Given an array of integers, each thread will sum a section of the array and add the sum to total_sum
    total_sum is the shared resource that is modified
*/
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>

int *arr;
int n_elements_per_thread = 0;
int total_sum = 0;

pthread_mutex_t sum_lock = PTHREAD_MUTEX_INITIALIZER;

// thread function to sum integers are start to end index
void *sumFunction(void *args){
    int threadID = *((int *)args);

    int start = threadID * n_elements_per_thread;
    int end = start + n_elements_per_thread;

    fprintf(stdout, "Thread %d sums from index %d to %d\n", threadID, start, end);
    fflush(stdout);

    for(int i = start; i < end; i++){
        // lock and access the shared resource, in this case, the global total_sum variable
        pthread_mutex_lock(&sum_lock);
        total_sum += arr[i];
        pthread_mutex_unlock(&sum_lock);
    }
}
int main(int argc, char *argv[]){
    const int arr_sz = atoi(argv[1]); // array size
    const int T = atoi(argv[2]); // number of threads

    if(arr_sz % T != 0){
        fprintf(stderr, "Array size not divisible by number of threads!!!\n");
        exit(EXIT_FAILURE);
    }

    n_elements_per_thread = arr_sz / T;

    // initialize array
    arr = (int *)malloc(sizeof(int) * arr_sz);
    for(int i = 0; i < arr_sz; i++){
        arr[i] = i;
    }

    // create T threads
    pthread_t thds[T];
    int threadID[T];
    for(int i = 0; i < T; i++){
        threadID[i] = i;
        pthread_create(&thds[i], NULL, (void *)sumFunction, (void *)&threadID[i]);
    }

    for(int i = 0; i < T; i++)
        pthread_join(thds[i], NULL);

    free(arr);
    arr = NULL;

    fprintf(stdout, "Total sum : %d\n", total_sum);

    return 0;
}