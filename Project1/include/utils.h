#ifndef UTILS_H
#define UTILS_H 

#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>

// Length of longest possible file path that we will test
#define PATH_MAX 1024

// Useful constant size for buffer used for reading/writing files
#define BUFFER_SIZE 1024

void setup_output_directory(char *block_folder, char *hash_folder);

void partition_file_data(char *input_file, int n, char *blocks_folder);

#endif