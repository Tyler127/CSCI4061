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

// Add a request to the queue
void queue_enqueue(request_queue_t *q, request_t request) { 
    q->rear = (q->rear + 1) % MAX_QUEUE_LEN;
    q->requests[q->rear] = request;
    q->size++;
}

// Remove a request from the queue
request_t queue_dequeue(request_queue_t *q) {
    request_t request = q->requests[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_LEN;
    q->size--;
    return request;
}

void queue_visualize(request_queue_t *q){
    printf("|-----VISUALIZING QUEUE-----|\n");
    printf(" queue size: %d\n", q->size);
    printf(" queue front request file: %s\n\n", q->requests[q->front].file_path);

    for(int i=0; i < q->size; i++){
        request_t current_request = q->requests[i];
        printf("i: %d\n", i);
        printf("    request file: %s\n", current_request.file_path);
        printf("    rotation angle: %d\n", current_request.rotation_angle);
    }

    printf("|-----VISUALIZING QUEUE DONE-----|\n");
}

// Global queue
request_queue_t request_queue;

// Global integer to indicate the number of worker threads
int num_worker_threads = 0; // set in main

// Global file pointer for writing to log file in worker
FILE* log_file; // set in main

// Global output directory
char* output_directory; // set in main

// Global condition trackers
int all_requests_added_to_queue = 0;
int all_requests_completed_by_workers = 0;

//Might be helpful to track the ID's of your threads in a global array

//What kind of locks will you need to make everything thread safe? [Hint you need multiple]
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t processing_waiting_lock = PTHREAD_MUTEX_INITIALIZER;

//What kind of CVs will you need  (i.e. queue full, queue empty) [Hint you need multiple]
pthread_cond_t queue_has_request_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t queue_filled_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t workers_done_cond = PTHREAD_COND_INITIALIZER;

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
void log_pretty_print(FILE* to_write, int threadId, int requestNumber, const char* file_name){
   fprintf(to_write, "[%d][%d][%s]\n", threadId, requestNumber, file_name);
   printf("[%d][%d][%s]\n", threadId, requestNumber, file_name);
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

    printf("-----> ENTERING PROCESSING THREAD\n");

    if ((dir = opendir(processing_args->directory_path)) == NULL) {
        perror("opendir() error");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {  // Check for regular file
            // Lock the queue 
            pthread_mutex_lock(&queue_lock);
            printf("PROCESSING THREAD: locked queue_lock\n");

            // Assemble the file path string 
            char* file_path = malloc(PATH_MAX);
            sprintf(file_path, "%s/%s", processing_args->directory_path, entry->d_name);
            
            // Create a new node struct
            request_t request;
            request.file_path = file_path;
            request.rotation_angle = processing_args->rotation_angle;
            
            // Enqueue the request
            queue_enqueue(processing_args->request_queue, request);

            printf("    file %s added to queue\n", file_path);

            // Unlock the queue
            pthread_mutex_unlock(&queue_lock);
            printf("PROCESSING THREAD: unlocked queue_lock\n");
        }
    }

    queue_visualize(&request_queue);

    // Update that all files have been added to the request queue
    all_requests_added_to_queue = 1;
    printf("PROCESSING THREAD: all files have been added to the queue\n");

    // Broadcast that the queue has had a request added
    pthread_cond_broadcast(&queue_has_request_cond);
    printf("PROCESSNG THREAD: broadcasted queue_has_request_cond\n");

    /* Wait for a worker thread to finish.
    Once one worker signals done that means the queue is empty and all work is done.
    So then broadcast the condition the workers are waiting on so they can reloop 
    and then exit.
    */
    pthread_mutex_lock(&processing_waiting_lock);

    /* CODE TO EXIT WORKERS ATTEMPT 1: */
    while(all_requests_completed_by_workers != 1){
        printf("PROCESSING THREAD: waiting for a worker to finish\n");

        // Wait for a worker thread to signal all work is done
        pthread_cond_wait(&workers_done_cond, &processing_waiting_lock);
        printf("PROCESSING THREAD: saw a worker finish\n");

        if(processing_args->request_queue->size <= 0){
            // Broadcast the condition the other workers are waiting on
            pthread_cond_broadcast(&queue_has_request_cond);
            printf("PROCESSING THREAD: sent final broadcast\n");
        }
    }

    /* CODE TO EXIT WORKERS ATTEMPT 2: */
    // while(1){
    //     printf("PROCESSING THREAD: waiting for workers_done_cond\n");
    //     pthread_cond_wait(&workers_done_cond, &processing_waiting_lock);
    //     if(processing_args->request_queue->size <= 0){
    //         pthread_cond_broadcast(&queue_has_request_cond);
    //         printf("PROCESSING THREAD: sent final broadcast of queue_has_request_cond\n");
    //         break;
    //     }
    // }

    pthread_mutex_unlock(&processing_waiting_lock);

    printf("-----> EXITING PROCESSING THREAD\n");

    closedir(dir);
    return NULL;
}


/*
    1: The worker threads takes an int ID as a parameter

    2: The Worker thread will block(pthread_cond_wait) for a condition variable that there is a requests in the queue. 

    3: The Worker threads will also block(pthread_cond_wait) once the queue is empty and wait for a signal to either exit or do work.

    4: The Worker thread will processes request from the queue while maintaining synchronization using lock and unlock. 

    5: The worker thread will write the data back to the given output dir as passed in main. 

    6: The Worker thread will log the request from the queue while maintaining synchronization using lock and unlock.  

    8: Hint the worker thread should be in a While(1) loop since a worker thread can process multiple requests and It will have two while loops in total
        that is just a recommendation feel free to implement it your way :) 
    9: You may need different lock depending on the job.  
*/
void* worker(void *args){
    int thread_id = *(int*)args; // Casts to int pointer then dereferences
    int requests_handled = 0;

    while(1){
        // Lock the queue
        pthread_mutex_lock(&queue_lock);
        printf("WORKER %d: locked queue_lock\n", thread_id);

        // Wait for the queue to have at least one request
        while(request_queue.size <= 0){
            // Check if all work has been completed
            if(request_queue.size <= 0 && all_requests_added_to_queue == 1){
                // Unlock the queue
                pthread_mutex_unlock(&queue_lock);
                
                // Lock the processing waiting lock
                pthread_mutex_lock(&processing_waiting_lock);

                // Signal that the worker is exiting
                pthread_cond_signal(&workers_done_cond);
                printf("WORKER %d: exiting\n", thread_id);

                all_requests_completed_by_workers = 1;

                // Unlock the waiting lock
                pthread_mutex_unlock(&processing_waiting_lock);

                pthread_exit(NULL);
            }

            printf("    WORKER %d: waiting for queue_has_request_cond\n", thread_id);
            pthread_cond_wait(&queue_has_request_cond, &queue_lock);
        }

        // Get a request off the queue 
        request_t request = queue_dequeue(&request_queue);

        // Call pretty print to add to the log file
        const char* file_name = get_filename_from_path(request.file_path);
        log_pretty_print(log_file, thread_id, requests_handled, file_name);

        // Unlock the queue
        pthread_mutex_unlock(&queue_lock);

        // Increment the amount of requests handled by this worker
        requests_handled++;

        printf("----------------------------------------------> WORKER %d: got file: %s\n", thread_id, request.file_path);

        // Create the output path
        char* output_file_path = (char*)malloc(strlen(output_directory) + strlen(file_name) + 1);
        sprintf(output_file_path, "%s/%s", output_directory, file_name);
        printf("    output path: %s\n", output_file_path);

        /*
            Stbi_load takes: A file name, int pointer for width, height, and bpp
        */
        int width;
        int height;
        int bpp;
        uint8_t* image_result = stbi_load(request.file_path, &width, &height, &bpp,  CHANNEL_NUM);
        uint8_t** result_matrix = (uint8_t **)malloc(sizeof(uint8_t*) * width);
        uint8_t** img_matrix = (uint8_t **)malloc(sizeof(uint8_t*) * width);
        for(int i = 0; i < width; i++){
            result_matrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * height);
            img_matrix[i] = (uint8_t *)malloc(sizeof(uint8_t) * height);
        }

        /*
            linear_to_image takes: 
            The image_result matrix from stbi_load
            An image matrix
            Width and height that were passed into stbi_load
        */
        linear_to_image(image_result, img_matrix, width, height);
        
        // Call flip_left_to_right or flip_upside_down depends on the angle(Should just be 180 or 270)
        // Both take image matrix from linear_to_image, and result_matrix to store data, and width and height.
        if(request.rotation_angle == 180){
            flip_left_to_right(img_matrix, result_matrix, width, height); 
        } 
        else if(request.rotation_angle == 270){
            flip_upside_down(img_matrix, result_matrix, width, height);
        }
        
        /// Call flatten_mat function, using result_matrix, img_arry and width and height; 
        uint8_t* img_array = (uint8_t *)malloc(sizeof(uint8_t) * width * height); ///Hint malloc using sizeof(uint8_t) * width * height
        flatten_mat(result_matrix, img_array, width, height);

        // Call stbi_write_png using: New path to where you wanna save the file, Width, height, img_array, (width*CHANNEL_NUM)
        stbi_write_png(output_file_path, width, height, CHANNEL_NUM, img_array, (width*CHANNEL_NUM));

        // Free everything that was malloc'd
        free(img_array);
        for(int i=0; i < width; i++){
            free(result_matrix[i]);
            free(img_matrix[i]);
        }
        free(result_matrix);
        free(img_matrix);

        printf("WORKER %d: unlocked queue_lock\n", thread_id);
    }      
}

/*
    Main:
        Get the data you need from the command line argument 
        Open the logfile
        Create the threads needed
        Join on the created threads
        Clean any data if needed. 
*/
int main(int argc, char* argv[]) {
    if(argc != 5) {
        fprintf(stderr, "Usage: %s <image directory> <output directory> <number of worker threads> <rotation angle>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Read arguments
    char *image_dir = argv[1];
    char *output_dir = argv[2];
    int num_workers = atoi(argv[3]);
    int rotation_angle = atoi(argv[4]);

    // Set global variables
    num_worker_threads = num_workers; 
    output_directory = (char*)malloc(strlen(output_dir) + 1);
    if (!output_directory) {
        perror("Failed to allocate memory for output directory");
        exit(EXIT_FAILURE);
    }
    strcpy(output_directory, output_dir);

    // Open the log file
    log_file = fopen("request_log", "a");
    if (log_file == NULL) {
        perror("Error opening log file");
        free(output_directory);
        exit(EXIT_FAILURE);
    }

    // Initiate the queue
    queue_init(&request_queue);

    // Create processing thread
    processing_args_t processing_args = {image_dir, rotation_angle, &request_queue};
    pthread_t processing_thread;
    if (pthread_create(&processing_thread, NULL, processing, &processing_args) != 0) {
        perror("Failed to create processing thread");
        fclose(log_file);
        free(output_directory);
        exit(EXIT_FAILURE);
    }

    // Create worker threads
    pthread_t workers[num_workers];
    int thread_ids[num_workers];
    for (int i = 0; i < num_workers; ++i) {
        thread_ids[i] = i;
        if (pthread_create(&workers[i], NULL, worker, &thread_ids[i]) != 0) {
            perror("Failed to create worker thread");
            // Cleanup already created threads
            for (int j = 0; j < i; j++) {
                pthread_cancel(workers[j]); //found on https://www.geeksforgeeks.org/pthread_cancel-c-example/, cancels thread using thread ID
            }
            pthread_cancel(processing_thread);
            fclose(log_file);
            free(output_directory);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for processing thread to finish
    pthread_join(processing_thread, NULL);

    // Join worker threads
    for (int i = 0; i < num_workers; ++i) {
        pthread_join(workers[i], NULL);
    }

    // Cleanup
    fclose(log_file);
    free(output_directory);
    printf("PROGRAM EXITED SUCCESSFULLY\n");
    return 0;
}
