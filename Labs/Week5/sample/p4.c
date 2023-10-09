#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

void itot(time_t timestamp, char* str_time_buffer, size_t bufferSize){
    struct tm *timeinfo = localtime(&timestamp);
    strftime(str_time_buffer, bufferSize, "%Y-%m-%d %H:%M:%S", timeinfo);

}

int main(int argc, char *argv[]) {
    const char *file_path = "D1/a.txt";
    struct stat file_stat;
    char tbuff[80];

    if (lstat(file_path, &file_stat) == -1) {
        perror("lstat");
        exit(1);
    }

    printf("File: %s\n", file_path);
    printf("Device ID: %lu\n", (unsigned long)file_stat.st_dev);
    printf("Inode Number: %lu\n", (unsigned long)file_stat.st_ino);
    printf("Mode: %o\n", file_stat.st_mode);
    printf("Hard Links: %lu\n", (unsigned long)file_stat.st_nlink);
    printf("User ID of Owner: %d\n", file_stat.st_uid);
    printf("Group ID of Owner: %d\n", file_stat.st_gid);
    printf("Device ID (if special file): %lu\n", (unsigned long)file_stat.st_rdev);
    printf("File Size (bytes): %ld\n", (long)file_stat.st_size);
    printf("Block Size (bytes): %ld\n", (long)file_stat.st_blksize);
    printf("Number of Blocks allocated: %ld\n", (long)file_stat.st_blocks);

    itot(file_stat.st_atime, tbuff, sizeof(tbuff));
    printf("Last Access Time: %s\n", tbuff);
    itot(file_stat.st_mtime, tbuff, sizeof(tbuff));
    printf("Last Modification Time: %s\n", tbuff);
    itot(file_stat.st_ctime, tbuff, sizeof(tbuff));
    printf("Last Status Change Time: %s\n", tbuff);

    return 0;
}