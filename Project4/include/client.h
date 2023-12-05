#ifndef IMAGE_CLIENT_ROTATION_H_
#define IMAGE_CLIENT_ROTATION_H_

#define _XOPEN_SOURCE

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

typedef struct request_queue {
    int rotation_angle;
    char *file_name;
} request_t; 

typedef struct processing_args {
    int number_worker;
    char *file_name;
} processing_args_t;

#endif
