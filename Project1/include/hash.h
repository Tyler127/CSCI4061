#ifndef HASH_H
#define HASH_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "sha256.h"

// Concats left and right hash, hashes them, and stores the result in result_hash
void compute_dual_hash(char *result_hash, char *left_hash, char *right_hash);

// Hashes the data in block_filename and stores the result in result_hash
void hash_data_block(char *result_hash, char *block_filename);


/******** Example usage of these two functions: ********

compute_dual_hash example:

char left_hash[SHA256_BLOCK_SIZE * 2 + 1];
// Fill in left_hash with the hash of the left child (+ null terminator)
char right_hash[SHA256_BLOCK_SIZE * 2 + 1];
// Fill in right_hash with the hash of the right child (+ null terminator)
char result_hash[SHA256_BLOCK_SIZE * 2 + 1];
compute_dual_hash(result_hash, left_hash, right_hash);

hash_data_block example:

char block_hash[SHA256_BLOCK_SIZE * 2 + 1];
hash_data_block(block_hash, "output/blocks/0.txt");

Q: Why do we use SHA256_BLOCK_SIZE * 2 + 1 for the size of the hash arrays 
instead of SHA256_BLOCK_SIZE + 1?

A: Each byte of the hash is represented by two characters in hex, so we need 
twice as much space to store the hash as the number of bytes in the hash.
*******************************************************/

#endif 