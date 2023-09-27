#ifndef PRINT_TREE
#define PRINT_TREE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PATH_MAX 1024

void print_merkle_tree(char *visualization_file, char *hashes_folder, int n);

#endif