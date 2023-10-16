#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char line[256];
    // Open the file in the current folder
    FILE *file_in_current_folder = fopen("a.txt", "r");
    if (file_in_current_folder == NULL) {
        perror("fopen");
        exit(1);
    }

    // Read and print the contents of the file in the current folder
    fgets(line, sizeof(line), file_in_current_folder);
    printf("Content of a.txt in the same folder: %s\n", line);
    
    fclose(file_in_current_folder);

    // current directory
    char current_dir[1024] = "";
    getcwd(current_dir, 1024);
    printf("Current directory before change is %s\n", current_dir);


    // Change the current working directory to a different folder
    if (chdir("D1") != 0) {
        perror("chdir");
        exit(1);
    }

    getcwd(current_dir, 1024);
    printf("Current directory after change is %s\n", current_dir);

    // Open the file in the different folder
    FILE *file_in_other_folder = fopen("a.txt", "r");
    if (file_in_other_folder == NULL) {
        perror("fopen");
        exit(1);
    }

    // Read and print the contents of the file in the different folder
    fgets(line, sizeof(line), file_in_other_folder);
    printf("Content of a.txt in the D1 folder: %s\n", line);


    // Close the files
    fclose(file_in_other_folder);

    return 0;
}