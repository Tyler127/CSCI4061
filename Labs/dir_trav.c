// Gianni Guadagno
// CSCI 4061 Lab 5
// dir_trav.c

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>



void traverseDirectory(char *dirname) {
    // open current directory
    DIR *dir;
    struct dirent *entry;
    dir = opendir(dirname);

    if (dir == NULL) {
        perror("opendir");
        return;
    }

    // Traverse through all entries
    while ((entry = readdir(dir)) != NULL) {
        // skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // concatenate dirname with the directory entry
        // use malloc
        char *path = malloc(strlen(entry->d_name) + strlen(dirname) + 2);
        // path[0] = '\0';

        strcat(path, dirname);
        strcat(path, "/");
        strcat(path, entry->d_name);

        struct stat *buf = malloc(sizeof(struct stat));
        if (lstat(path, buf) == -1) {
            perror("lstat");
            free(buf);
            free(path);
            continue;
        }

        // if entry is a directory
        if (S_ISDIR(buf->st_mode)) {
            // print entry name
            printf("%s\n", path);
            traverseDirectory(path);
        }

        // if entry is a regular file
        if (S_ISREG(buf->st_mode)) {
            // print entry name
            printf("%s\n", path);
        }

        if (S_ISLNK(buf->st_mode)) {
            // print entry name
            printf("Symlink found: %s\n", path);
        }


    }

    closedir(dir);
    // close current directory
}


int main(int argc, char *argv[]) {

    // Task 1 : Hard link and Soft link

    const char *f1 = argv[1];
    const char *f2 = argv[2];
    const char *f3 = argv[3];
    //          Create a new file argv[1]
    FILE *new_file = fopen(argv[1], "w");
    //          Create a hard link argv[2] for argv[1]
    int h_link =  link(f2,f1);
    //          Create a soft link argv[3] for argv[1]
    int s_link = symlink(f3,f1);

    // Task 2 : Traverse directory
        traverseDirectory(argv[4]);
    //          Traverse directory argv[4] and report all files and directories
    //          If the directory entry is a soft link, report it

    return 0;
}
