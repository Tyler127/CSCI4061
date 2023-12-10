#ifndef HASH_H
#define HASH_H

#include "sha256.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 1024

void compute_dual_hash(char *result_hash, char *left_hash, char *right_hash);

void hash_data_block(char *result_hash, char *block_filename);

#endif 