#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

// Number of philosophers
#define PHILOSOPHERS 5

// Philosopher states
#define THINKING 0
#define HUNGRY 1
#define EATING 2
int philosopher_state[PHILOSOPHERS];

// Neighbor ids
#define LEFT(x) (x + PHILOSOPHERS - 1) % PHILOSOPHERS
#define RIGHT(x) (x + 1) % PHILOSOPHERS  

sem_t mutex;
sem_t forks[PHILOSOPHERS];

int loop  = 1;

// Handle Ctrl + C
void signalHandler(int signum){
    loop = 0;
}

void acquireForks(int id){
    // check if the philosopher's state is hungry and their left and right neighbors are not eating
    // if true, then change the philosopher's state to eating and acquire forks[id] using post
    if (philosopher_state[id] == HUNGRY && philosopher_state[LEFT(id)] != EATING && philosopher_state[RIGHT(id)] != EATING){
        philosopher_state[id] = EATING;
        sem_post(&forks[id]);

    }
}

void takeForks(int id) {
    // enter critical section, use mutex semaphore with wait
    sem_wait(&mutex);
    // set philosopher state to HUNGRY
    philosopher_state[id] = HUNGRY;
    // acquire the forks if possible
    acquireForks(id);
    // leave the critical section
    sem_post(&mutex);
    // block if forks were not acquired using forks[id] with wait
    sem_wait(&forks[id]);
}

void putForks(int id) {
    // enter critical section, use mutex semaphore with wait
    sem_wait(&mutex);
    // set philosopher state to THINKING
    philosopher_state[id] = THINKING;
    // check if LEFT neighbor can acquire forks
    acquireForks(LEFT(id));
    // check if RIGHT neighbor can acquire forks
    acquireForks(RIGHT(id));
    // leave the critical section
    sem_post(&mutex);
}

void *philosopher(void *args) {
    int id = *((int *)args);
    while(loop) {
        fprintf(stdout, "Philosopher %d HUNGRY...\n", id);
        fflush(stdout);
        sleep(1);
        takeForks(id);  // get left and right forks
        fprintf(stdout, "Philosopher %d EATING...\n", id);
        fflush(stdout);
        sleep(2);
        putForks(id);   // put back left and right forks
        fprintf(stdout, "Philosopher %d THINKING...\n", id);
        fflush(stdout);
        sleep(2);
    }
    fprintf(stdout, "Philosopher %d exiting...\n", id);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]){

    // Signal handling will be taught towards end of semester
    signal(SIGINT, signalHandler);

    pthread_t philosopher_thds[PHILOSOPHERS];

    // initialize semaphores
    sem_init(&mutex, 0, 1); // allow only one philosopher in the critical section
    for(int i = 0; i < PHILOSOPHERS; i++)
        sem_init(&forks[i], 0, 1); // only one philosopher can get hold of a fork

    // Start philosopher threads
    int philosopher_ids[PHILOSOPHERS];
    for(int i = 0; i < PHILOSOPHERS; i++){
        philosopher_ids[i] = i;
        pthread_create(&philosopher_thds[i], NULL, (void *)philosopher, (void *)&philosopher_ids[i]);
    }

    // Join philosopher threads
    for(int i = 0; i < PHILOSOPHERS; i++)
        pthread_join(philosopher_thds[i], NULL);

    return 0;
}