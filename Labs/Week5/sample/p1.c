#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>

int main() {
    DIR *dir;
    struct dirent *entry;

    // Open the directory
    dir = opendir("D1");

    if (dir == NULL) {
        perror("opendir");
        exit(1);
    }

    //File type. 0: unknow file    6: directory    8: regular file    10: symbolic link 
    // Read and print the directory entries
    while ((entry = readdir(dir)) != NULL) {
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
    }

    // Close the directory
    closedir(dir);

    return 0;
}