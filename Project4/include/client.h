#ifndef IMAGE_CLIENT_ROTATION_H_
#define IMAGE_CLIENT_ROTATION_H_

#define _XOPEN_SOURCE
#define _DEFAULT_SOURCE // https://stackoverflow.com/questions/69689586/dt-reg-undeclared-even-when-using-dirent-h-header-file-in-function

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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "hash.h"


/********************* [ Helpful Macro Definitions ] **********************/
#define BUFF_SIZE 1024 
#define LOG_FILE_NAME "request_log"               //Standardized log file name
#define INVALID -1                                  //Reusable int for marking things as invalid or incorrect
#define MAX_THREADS 100                             //Maximum number of threads
#define MAX_QUEUE_LEN 100                           //Maximum queue length

// Operations
#define IMG_OP_ACK      (1 << 0)
#define IMG_OP_NAK      (1 << 1)
#define IMG_OP_ROTATE   (1 << 2)
#define IMG_OP_EXIT     (1 << 3)

// Flags
#define IMG_FLAG_ROTATE_180     (1 << 0)
#define IMG_FLAG_ROTATE_270     (1 << 1)
#define IMG_FLAG_ENCRYPTED      (1 << 2)
#define IMG_FLAG_CHECKSUM       (1 << 3)

/********************* [ Helpful Typedefs        ] ************************/

typedef struct packet {
    unsigned char operation : 4;
    unsigned char flags : 4;
    unsigned int size;
    unsigned char checksum[SHA256_BLOCK_SIZE];
} packet_t; 

typedef struct processing_args {
    int number_worker;
    char *file_name;
} processing_args_t;

/********************* [ Request Queue        ] ************************/

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

/********************* [ Function Prototypes ] **********************/
void queue_init(request_queue_t *q);
void queue_enqueue(request_queue_t *q, request_t request);
request_t queue_dequeue(request_queue_t *q);

#endif
