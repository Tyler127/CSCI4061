#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <unistd.h>

void traverseDirectory(char *dirname){
    // open current directory
    DIR *dir = opendir(dirname);
    struct dirent *entry;

    // Traverse through all entries
    while((entry = readdir(dir)) != NULL){

        // skip . and ..
        if(strcmp(entry->d_name, ".") == 0){
            //printf("skiped .\n");
            entry = readdir(dir);
        }
        if(strcmp(entry->d_name, "..") == 0){
            //printf("skiped ..\n");
            entry = readdir(dir);
        }
        
        // concatenate dirname with the directory entry
        // use malloc
        char *dirname_and_entry = malloc(2000);
        sprintf(dirname_and_entry, "%s/%s", dirname, entry->d_name);

        // Create a string for the cwd, dirname, and entry name
        char *cwd_buffer = malloc(2000);
        getcwd(cwd_buffer, 2000);
        char full_path[2000];
        sprintf(full_path, "%s/%s", cwd_buffer, dirname_and_entry);

        // if entry is a directory
        //          print entry name
        //          You may have to get the absolute path
        //          recursively call traverseDirectory
        char filetype[10] = "";
        if(entry->d_type == DT_DIR){
            printf("%s\n", full_path); 
            traverseDirectory(dirname_and_entry);           
        }

        // if entry is a regular file
        //          print entry name
        if(entry->d_type == DT_REG){
            printf("%s\n", full_path); 
        }

        // else
        //          if entry is symbolic
        //              print entry name
        if(entry->d_type == 10){
            printf("Symlink found: %s\n", full_path);
        }

        // Free the mallocs
        free(dirname_and_entry);
        free(cwd_buffer);
    }
    closedir(dir);
}

int main(int argc, char *argv[]) {

    // Task 1 : Hard link and Soft link
    //          Create a new file argv[1]
    //          Create a hard link argv[2] for argv[1]
    //          Create a soft link argv[3] for argv[1]
    FILE *file = fopen(argv[1], "w");
    if(link(argv[1], argv[2]) != 0){ // hard link
        fprintf(stderr, "Unable to create hard link\n");
        exit(-1);
    }
    if(symlink(argv[1], argv[3]) != 0){ // symbolic link
        fprintf(stderr, "Unable to create sym link\n");
        exit(-1);
    }

    // Task 2 : Traverse directory
    //          Traverse directory argv[4] and report all files and directories
    //          If the directory entry is a soft link, report it
    traverseDirectory(argv[4]);

    return 0;
}