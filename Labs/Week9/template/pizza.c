/*
    pizza_order_stand is a buffer of size STANDSIZE
    producer and consumer will add and pick pizza from the pizza_order_stand in a circular fashion
        next_pos_for_pizza = (next_pos_for_pizza + 1) % STANDSIZE
    Note the use of pointers and access them correctly
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define STANDSIZE 4
#define TRUE 1
#define FALSE 0

typedef struct ProducerArgument{
    int *pizza_order_stand;
    int *n_pizza_on_stand; // tracks number of pizzas at a given point of time
    int n_pizzas;
    int *orders_complete; // checks all orders are completed
}prodArgs;

typedef struct ConsumerArgument{
    int *pizza_order_stand;
    int *n_pizza_on_stand;
    int *orders_complete;
}consArgs;

// initialize stand_lock used for mutual exclusion on pizza stand
pthread_mutex_t stand_lock  = ????;

// initial condition variables that will be used by producer and consumer
pthread_cond_t prod_cond    = ????;
pthread_cond_t cons_cond    = ????;

void* pizzaProducer(void *args){
    prodArgs *pargs = (prodArgs *)args;
    int next_pos_for_pizza = 0;

    // producer producer n_pizzas and places it on the stand
    for(int i = 1; i <= pargs->n_pizzas; i++){
        // lock the stand
        ????
        
        // Wait for stand to have atleast one free space
        // Use prod_cond to wait
        while(*(pargs->n_pizza_on_stand) == STANDSIZE)
            ????

        // store pizza index i at next_pos_for_pizza location in pizza_order_stand and update the next position to store pizza
        ????
        ????

        // increment total number of pizza on stand by 1
        *(pargs->n_pizza_on_stand) = *(pargs->n_pizza_on_stand) + 1;
        
        // signal consumer using cons_cond that one pizza is added to stand and unlock the stand
        ????
        ????
        
        fprintf(stdout, "Producer added Pizza %d to stand\n", i);
        fflush(stdout);
    }

    // Let the consumer know the orders are completed
    // lock the stand
    ????
    // set orders_complete to TRUE
    *(pargs->orders_complete) = TRUE;

    // signal consumer on cons_cond to exit from wait and unlock the stand
    ????
    ????
    fprintf(stdout, "Producer completed all orders, exiting...\n");
    fflush(stdout);
}

void *pizzaConsumer(void *args){
    consArgs *cargs = (consArgs *)args;
    int pizza = 0; // picked pizza value
    int next_pos_for_pizza = 0;
    while(TRUE){
        // lock the stand
        ????

        // Wait for atleast one pizza to be available on stand
        while(*(cargs->n_pizza_on_stand) <= 0){
            // If the orders are completed, consumer should exit
            if(*(cargs->orders_complete)){
                // unlock the stand
                ????
                fprintf(stdout, "Consumer completed all orders, exiting...\n");
                fflush(stdout);
                pthread_exit(NULL);
            }
            // Wait on cons_cond
            ????
        }

        // Pick pizza from stand, assign picked value to pizza variable and update the next position to pick pizza from
        ????
        ????

        // Decrement the total number of pizzas on stand by 1
        *(cargs->n_pizza_on_stand) = *(cargs->n_pizza_on_stand) - 1;

        // Signal producer on prod_cond that one pizza is consumed and unlock stand
        ????
        ????
        
        fprintf(stdout, "Consumer picked Pizza %d from stand\n", pizza);
        fflush(stdout);
    }
}
int main(int argc, char *argv[]){
    int n_pizzas = 20;
    int n_pizza_on_stand = 0;
    int orders_complete = FALSE;
    int pizza_order_stand[STANDSIZE];

    // producer arguments
    prodArgs pargs = {pizza_order_stand, &n_pizza_on_stand, n_pizzas, &orders_complete};

    // consumer arguments
    consArgs cargs = {pizza_order_stand, &n_pizza_on_stand, &orders_complete};

    // create producer and consumer thread
    pthread_t producer, consumer;
    if(pthread_create(&producer, NULL, (void*)pizzaProducer, &pargs) != 0){
        fprintf(stderr, "Error creating producer!!!\n");
        exit(EXIT_FAILURE);
    }
    if(pthread_create(&consumer, NULL, (void*)pizzaConsumer, &cargs) != 0){
        fprintf(stderr, "Error creating consumer!!!\n");
        exit(EXIT_FAILURE);
    }

    // wait for threads to join
    pthread_join(consumer, NULL);
    pthread_join(producer, NULL);

    return 0;
}