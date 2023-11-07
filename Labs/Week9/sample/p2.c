/*
Program idea: Implement a barrier that ensures all threads reach the barrier point before exiting the code
    All threads add their id to local variable total_sum
    As soon as a thread's total_sum >= 1000, put it in wait mode, for other thread to reach the same point
    Last thread which reaches total_sum >= 1000, awakes all the other threads and everyone exits
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <pthread.h>

#define TRUE 1

int T = 0;
int barrier_count = 0;

pthread_mutex_t barrier_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t barrier_cond = PTHREAD_COND_INITIALIZER;

void *sumFunction(void *args){
    int threadID = *((int *)args);
    fprintf(stdout, "Thread %d running...\n", threadID);
    int total_sum = 0;
    while(TRUE){
        if(total_sum < 1000){
            total_sum += threadID;
        } else {
            pthread_mutex_lock(&barrier_lock);
            barrier_count++; // shared resource
            sleep(1);
            if(barrier_count < T){ // wait for all threads
                fprintf(stdout, "Thread %d sleeping...\n", threadID);
                fflush(stdout);
                pthread_cond_wait(&barrier_cond, &barrier_lock);
            }else{ //awake other threads
                fprintf(stdout, "Thread %d awaking all...\n", threadID);
                fflush(stdout);

                // Using signal
                for(int i = 0; i < T - 1; i++)
                    pthread_cond_signal(&barrier_cond);

                // Using broadcast
                // pthread_cond_broadcast(&barrier_cond);
            }

            pthread_mutex_unlock(&barrier_lock);

            fprintf(stdout, "Thread %d reports total sum: %d\n", threadID, total_sum);
            fflush(stdout);

            pthread_exit(NULL);
        }
    }
}
int main(int argc, char *argv[]){
    T = atoi(argv[1]); // number of threads

    // create T threads
    pthread_t thds[T];
    int threadID[T];
    for(int i = 0; i < T; i++){
        threadID[i] = i + 1;
        pthread_create(&thds[i], NULL, (void *)sumFunction, (void *)&threadID[i]);
    }

    for(int i = 0; i < T; i++)
        pthread_join(thds[i], NULL);

    return 0;
}