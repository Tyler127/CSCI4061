#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// split a given line into tokens
int splitLine(char *line, char **tokens, char *delimiter){
    int token_count = 0;
    char *token;
    token = strtok(line, delimiter);
    while(token != NULL){
        tokens[token_count++] = token;
        token = strtok(NULL, delimiter);
    }

    return token_count;
}

// Find offset of heap from the starting location of virtual memory
int findOffset(char *heap, char *search, int hlen){
    int slen = strlen(search);
    int found = -1;

    for(int i = 0; i < hlen; i++){
        int j;
        char read_data[slen + 1];
        for(int j = i; j - i < slen && j < hlen; j++){
            read_data[j-i] = heap[j];
        }
        read_data[strlen(read_data)] = '\0';
        if(strcmp(read_data, search) == 0){
            found = i;
            break;
        }
    }
    if(found == -1){
        perror("Search string not found\n");
        exit(EXIT_FAILURE);
    }
    return found;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        perror("Incorrect arguments");
        exit(EXIT_FAILURE);
    }
    
    // ./p2 <process_id> <string>

    // generate proc files names for pid passed
    int pid = atoi(argv[1]);
    char mem_file[1024] = "";
    sprintf(mem_file, "/proc/%d/mem", pid);
    char maps_file[1024] = "";
    sprintf(maps_file, "/proc/%d/maps", pid);

    // opens maps file
    FILE *maps_fp = fopen(maps_file, "r");
    if(maps_fp == NULL){
        perror("Cannot open file!!!\n");
        exit(EXIT_FAILURE);
    }

    ssize_t read = 0;
    size_t len = 0;
    char *line = NULL;
    int found_heap = 0;
    while((read = getline(&line, &len, maps_fp)) != -1){
        char *tokens[100];
        char temp[1024] = "";
        strcpy(temp, line);
        int token_count = splitLine(temp, tokens, " ");

        found_heap = 0;
        for(int i = 0; i < token_count; i++){ // Identify the heap info line
            tokens[i][strcspn(tokens[i], "\n")] = 0;
            if(strcmp(tokens[i], "[heap]") == 0){
                found_heap = 1;
                break;
            }
        }
        
        // Retrieve address range of heap and change contentS
        if(found_heap){
            char *addr = tokens[0];
            char *perm = tokens[1];
            char *offset = tokens[2];
            char *device = tokens[3];
            char *inode = tokens[4];

            char *address[2];
            char addr_temp[1024] = "";
            strcpy(addr_temp, addr);
            int addr_count = splitLine(addr_temp, address, "-");

            // heap address range
            long long addr_start = strtoll(address[0], NULL, 16);
            long long addr_end = strtoll(address[1], NULL, 16);

            // Open memory file
            FILE *mem_fp = fopen(mem_file, "r+");
            if(mem_fp == NULL){
                perror("Cannot open memory file");
                exit(EXIT_FAILURE);
            }

            // seak to heap location in memory file
            fseek(mem_fp, addr_start, SEEK_SET);
            long heap_size = addr_end - addr_start;
            char heap[heap_size + 2];
            memset(heap, 0, heap_size + 2);

            long r = fread(heap, sizeof(char), heap_size, mem_fp);
            // fin location of "Hello" in heap 
            int oset = findOffset(heap, argv[2], r);
            fseek(mem_fp, addr_start + oset, SEEK_SET);

            // Overwrite "Hello" with "World"
            fwrite("World", sizeof(char), 8, mem_fp);
            fclose(mem_fp);
            break;
        }
        line = NULL;
    }
    fclose(maps_fp);

    return 0;
}