#include <stdlib.h>
#include <string.h>
#include "../include/utils.h"

// Extracts the filename from the full path
// e.g. path="./root_directories/root1/sub_1/WorldStage_2.txt"
// return value = "WorldStage_2.txt"
char* extract_filename(char* path) {
    char* filename = strrchr(path, '/');
    if(filename == NULL)
        return path;
    return filename + 1;
}

// Extracts the directory name after "root_directories"
// e.g. path="./root_directories/original_root1/sub_1/WorldStage_2.txt"
// return value = "root1/"
char* extract_root_directory(const char* path) {
    char* root_dir = strstr(path, "/root_directories/");
    if (root_dir == NULL){
        return NULL; // "root_directories" not found in path
    }

    char* start_of_root = root_dir + strlen("/root_directories/");
    char* end_of_root = strchr(start_of_root, '/');

    if (end_of_root == NULL){
        return strdup(start_of_root);  // In case the path ends with the root directory
    }
    int length = end_of_root - start_of_root + 1; // +1 to include the trailing slash
    char* result = (char*)malloc(length + 1);  // +1 for the null terminator
    strncpy(result, start_of_root, length);
    result[length] = '\0';  // Null-terminate the result string
    return result;
}

void remove_filepath_duplicate(char **dup_list, char **retain_list, int *size) {
    for (int i = 0; i < *size; i++) {
        for (int j = i + 1; j < *size; ) {
            if (strcmp(dup_list[i], dup_list[j]) == 0) {
                // Duplicate found, free the memory of the duplicate string
                free(dup_list[j]);
                free(retain_list[j]);

                // Shift all the elements after j to the left
                for (int k = j; k < *size - 1; k++) {
                    dup_list[k] = dup_list[k + 1];
                    retain_list[k] = retain_list[k+1];
                }

                (*size)--; // Decrease the size of the array
            } else {
                j++; // Move to the next string if current one is not a duplicate
            }
        }
    }
}

void sanitize_dup_retain(char **dup_list, char **retain_list, int size) {
    for (int m = 0; m < size; m++) {
        for (int n = 0; n < size; n++) {
            if (strcmp(retain_list[m], dup_list[n]) == 0) {
                // Found a match, replace retain_list[m] with retain_list[n]
                // But first, if retain_list[m] was dynamically allocated, free it
                if (retain_list[m] != retain_list[n]) {
                    free(retain_list[m]);
                    retain_list[m] = retain_list[n];
                }
                break; // No need to continue inner loop for this m once a match is found
            }
        }
    }
}

int parse_hash(char * file_hashes, char**dup_list, char** retain_list){
    char delim[] = "|";
    char array[32][128];
    int count = 0;
    memset(&array[0], 0, sizeof(array));// clean the buffer

    char *ptr = strtok(file_hashes, delim);

    while(ptr != NULL)
    {
        strncpy(array[count], ptr, strlen(ptr));
        ptr = strtok(NULL, delim);
        count++;
    }

    int index = 0;
    //even idx is filename and odd index is the hash
    for(int i=1; i<count*2; i+=2){
        for (int j = i + 2; j < count*2; j+=2){
            if (  strcmp(array[i], "") != 0 && strcmp(array[i], array[j]) == 0) {
                // record the filepath of duplicate file
                dup_list[index] = malloc(sizeof(char)* 128);
                retain_list[index] = malloc(sizeof(char)* 128);
                char* file_index_str_1 = &array[j-1][strlen(array[j-1])-5];
                char* file_index_str_2 = &array[i-1][strlen(array[i-1])-5];
                int file_index_1 = atoi(&file_index_str_1[0]);
                int file_index_2 = atoi(&file_index_str_2[0]);
                if(file_index_1 > file_index_2) {
                    strcpy(dup_list[index], array[j - 1]);
                    strcpy(retain_list[index], array[i - 1]);

                }else{
                    strcpy(dup_list[index], array[i - 1]);
                    strcpy(retain_list[index], array[j - 1]);
                }

                index++;
            }
        }
    }
    // remove duplicate filepath in dup_list get from file_hashes
    remove_filepath_duplicate(dup_list,retain_list,&index);
    // files in retain_list might also appear in dup_list,
    // since one file may have more than 2 duplicates
    sanitize_dup_retain(dup_list,retain_list,index);

    return index;
}