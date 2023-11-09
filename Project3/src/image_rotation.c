#include "image_rotation.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

// Initialize the queue
void queue_init(request_queue_t *q) {
    q->size = 0;
    q->front = 0;
    q->rear = -1;
}

// Add an item to the queue
void queue_enqueue(request_queue_t *q, node_t item) { 
    q->rear = (q->rear + 1) % MAX_QUEUE_LEN;
    q->items[q->rear] = item;
    q->size++;
}

// Remove an item from the queue
node_t queue_dequeue(request_queue_t *q) {
    node_t item = q->items[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_LEN;
    q->size--;
    return item;
}

 
//Global integer to indicate the length of the queue??
//Global integer to indicate the number of worker threads
//Global file pointer for writing to log file in worker??
//Might be helpful to track the ID's of your threads in a global array
//What kind of locks will you need to make everything thread safe? [Hint you need multiple]
//What kind of CVs will you need  (i.e. queue full, queue empty) [Hint you need multiple]
//How will you track the requests globally between threads? How will you ensure this is thread safe?
//How will you track which index in the request queue to remove next?
//How will you update and utilize the current number of requests in the request queue?
//How will you track the p_thread's that you create for workers?
//How will you know where to insert the next request received into the request queue?


/*
    The Function takes:
    to_write: A file pointer of where to write the logs. 
    requestNumber: the request number that the thread just finished.
    file_name: the name of the file that just got processed. 

    The function output: 
    it should output the threadId, requestNumber, file_name into the logfile and stdout.
*/
void log_pretty_print(FILE* to_write, int threadId, int requestNumber, char * file_name){
   
}


/*

    1: The processing function takes a void* argument called args. It is expected to be a pointer to a structure processing_args_t 
    that contains information necessary for processing.

    2: The processing thread need to traverse a given dictionary and add its files into the shared queue while maintaining synchronization using lock and unlock. 

    3: The processing thread should pthread_cond_signal/broadcast once it finish the traversing to wake the worker up from their wait.

    4: The processing thread will block(pthread_cond_wait) for a condition variable until the workers are done with the processing of the requests and the queue is empty.

    5: The processing thread will cross check if the condition from step 4 is met and it will signal to the worker to exit and it will exit.

*/
void* processing(void *args) {
    processing_args_t *processing_args = (processing_args_t *)args;
    DIR *dir;
    struct dirent *entry;

    if ((dir = opendir(processing_args->directory_path)) == NULL) {
        perror("opendir() error");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Check for regular file
            char* file_path = malloc(PATH_MAX);
            sprintf(file_path, "%s/%s", processing_args->directory_path, entry->d_name);
            
            node_t item;
            item.file_path = file_path;
            item.rotation_angle = processing_args->rotation_angle;
            
            // Enqueue the item
            queue_enqueue(processing_args->request_queue, item);
        }
    }
    
    closedir(dir);
    return NULL;
}


/*
    1: The worker threads takes an int ID as a parameter

    2: The Worker thread will block(pthread_cond_wait) for a condition variable that there is a requests in the queue. 

    3: The Worker threads will also block(pthread_cond_wait) once the queue is empty and wait for a signal to either exit or do work.

    4: The Worker thread will processes request from the queue while maintaining synchronization using lock and unlock. 

    5: The worker thread will write the data back to the given output dir as passed in main. 

    6:  The Worker thread will log the request from the queue while maintaining synchronization using lock and unlock.  

    8: Hint the worker thread should be in a While(1) loop since a worker thread can process multiple requests and It will have two while loops in total
        that is just a recommendation feel free to implement it your way :) 
    9: You may need different lock depending on the job.  

*/
void* worker(void *args)
{
    int thread_id = *(int*)args; // Casts to int pointer then dereferences
    printf("Worker thread %d \n", thread_id);
    return NULL;

        /*
            Stbi_load takes:
                A file name, int pointer for width, height, and bpp

        */
       // uint8_t* image_result = stbi_load("??????","?????", "?????", "???????",  CHANNEL_NUM);
       // uint8_t** result_matrix = (uint8_t **)malloc(sizeof(uint8_t*) * width);
        //uint8_t** img_matrix = (uint8_t **)malloc(sizeof(uint8_t*) * width);
       // for(int i = 0; i < width; i++){
         //   result_matrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * height);
         //   img_matrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * height);
       // }
        /*
        linear_to_image takes: 
            The image_result matrix from stbi_load
            An image matrix
            Width and height that were passed into stbi_load
        
        */
        //linear_to_image("??????", "????", "????", "????");
        ////TODO: you should be ready to call flip_left_to_right or flip_upside_down depends on the angle(Should just be 180 or 270)
        //both take image matrix from linear_to_image, and result_matrix to store data, and width and height.
        //Hint figure out which function you will call. 
        //flip_left_to_right(img_matrix, result_matrix, width, height); or flip_upside_down(img_matrix, result_matrix ,width, height);
     
        //uint8_t* img_array = NULL; ///Hint malloc using sizeof(uint8_t) * width * height
        ///TODO: you should be ready to call flatten_mat function, using result_matrix
        //img_arry and width and height; 
        //flatten_mat("??????", "??????", "????", "???????");

        ///TODO: You should be ready to call stbi_write_png using:
        //New path to where you wanna save the file,
        //Width
        //height
        //img_array
        //width*CHANNEL_NUM
       // stbi_write_png("??????", "?????", "??????", CHANNEL_NUM, "??????", "?????"*CHANNEL_NUM);
}

/*
    Main:
        Get the data you need from the command line argument 
        Open the logfile
        Create the threads needed
        Join on the created threads
        Clean any data if needed. 
*/
int main(int argc, char* argv[]) 
{
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <image directory> <output directory> <number of worker threads> <rotation angle>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read arguments
    char *image_dir = argv[1];
    char *output_dir = argv[2];
    int num_workers = atoi(argv[3]);
    int rotation_angle = atoi(argv[4]);

    // Initialize queue
    request_queue_t request_queue;
    queue_init(&request_queue);

    // Create processing thread
    processing_args_t processing_args = {image_dir, rotation_angle, &request_queue};
    pthread_t processing_thread;
    pthread_create(&processing_thread, NULL, processing, &processing_args);

    // Create worker threads
    pthread_t workers[num_workers];
    int thread_ids[num_workers];
    for (int i = 0; i < num_workers; ++i) {
        thread_ids[i] = i;
        pthread_create(&workers[i], NULL, worker, &thread_ids[i]);
    }

    // Wait for processing thread to finish
    pthread_join(processing_thread, NULL);

    // Join worker threads
    for (int i = 0; i < num_workers; ++i) {
        pthread_join(workers[i], NULL);
    }

    return 0;
}