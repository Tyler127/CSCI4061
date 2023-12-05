#ifndef UTILS_H_
#define UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Convert linear array to two dimensional image
void linear_to_image(uint8_t* linearArray, uint8_t** twoDArray, int width, int height);

// flatten the input matrix to linear array
void flatten_mat(uint8_t** matrix, uint8_t* img_array, int width, int height);

// flip image left to right
void flip_left_to_right(uint8_t** img_mat, uint8_t** res_mat, int width, int height);

// flip image upside down
void flip_upside_down(uint8_t** img_mat,  uint8_t** res_mat, int width, int height);

// Function to extract the file name from a path
const char* get_filename_from_path(char* path);

#endif