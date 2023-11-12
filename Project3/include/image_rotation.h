#ifndef IMAGE_ROTATION_H_
#define IMAGE_ROTATION_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <stdint.h>
#include "utils.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define CHANNEL_NUM 1

#include "stb_image.h"
#include "stb_image_write.h"

/********************* [ Helpful Macro Definitions ] **********************/
#define BUFF_SIZE 1024 
#define LOG_FILE_NAME "request_log"               //Standardized log file name
#define INVALID -1                                  //Reusable int for marking things as invalid or incorrect
#define MAX_THREADS 100                             //Maximum number of threads
#define MAX_QUEUE_LEN 100                           //Maximum queue length

/********************* [ Helpful Typedefs ] ************************/

// Structure for queue elements
typedef struct request {
    char* file_path;  // File path for the image to process
    int rotation_angle;  // The angle to rotate the image
} request_t; 

// Structure for the queue
typedef struct request_queue {
    request_t requests[MAX_QUEUE_LEN];  // Array of queue items
    int size;                           // Current size of the queue
    int front;                          // Index of the front item
    int rear;                           // Index of the rear item
} request_queue_t; 

// Structure for processing thread arguments
typedef struct processing_args {
    char* directory_path;  // Path to the directory containing images
    int rotation_angle;    // The angle to rotate the images
    request_queue_t* request_queue;  // Pointer to the request queue
    FILE* log_file;        // File pointer for logging
} processing_args_t;

/********************* [ Function Prototypes ] **********************/
void *processing(void *args); 
void *worker(void *args); 
void log_pretty_print(FILE* to_write, int threadId, int requestNumber, char * file_name);
void queue_init(request_queue_t *q);
void queue_enqueue(request_queue_t *q, request_t request);
request_t queue_dequeue(request_queue_t *q);

#endif // IMAGE_ROTATION_H_
