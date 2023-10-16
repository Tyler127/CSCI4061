#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

void printDirent(const char *directoryPath, const char *targetFileName){
    DIR *dir1 = opendir(directoryPath);
    if (dir1 == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;

    // Read and print the directory entries
    while ((entry = readdir(dir1)) != NULL) {
        if (strcmp(entry->d_name, targetFileName) == 0) {
            printf("Name: %s\n", entry->d_name);
            printf("Inode Number: %lu\n", (unsigned long)entry->d_ino);
            char filetype[10] = "";
            if(entry->d_type == DT_DIR)
                strcpy(filetype, "DT_DIR");
            else if(entry->d_type == DT_REG)
                strcpy(filetype, "DT_REG");
            else
                strcpy(filetype, "OTHER");
            printf("File Type: %s\n", filetype);
            printf("Directory Entry Length: %d\n", entry->d_reclen);
            printf("\n");
            break;
        }
    }

    closedir(dir1);
}

int main(){
    if(link("D2/b.txt", "b_link_hard.txt") != 0){ // hard link
        fprintf(stderr, "Unable to create hard link\n");
        exit(-1);
    }

    if(symlink("D2/b.txt", "b_link_soft.txt") != 0){ // symbolic link
        fprintf(stderr, "Unable to create sym link\n");
        exit(-1);
    }

    //File type. 0: unknow file    6: directory    8: regular file    10: symbolic link 
    printDirent("D2","b.txt");
    printDirent(".","b_link_hard.txt");
    printDirent(".","b_link_soft.txt");


    return 0;
}
