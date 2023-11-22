#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define STRSZ 100
#define PORT 3000

/****************************/
// IMPORTANT: ERROR CHECK ALL SOCKET APIs
/****************************/

int main(int argc, char *argv[]) {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // create socket to establish connection
    if(sockfd == -1)
        perror("socket error");

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // server IP, since the server is on same machine, use localhost IP
    servaddr.sin_port = htons(PORT); // Port the server is listening on

    int ret = connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)); // establish connection to server
    if(ret == -1)
        perror("connect error");

    char msg[STRSZ];
    bzero(msg, STRSZ); // initialize msg with '\0'

    fprintf(stdout, "Client: ");
    fscanf(stdin, "%[^\n]s", msg); // Read input with space until a newline
    setbuf(stdin, NULL); // Important, clears stdin buffer
    
    ret = send(sockfd, msg, STRSZ, 0); // send message to server
    if(ret == -1)
        perror("send error");

    close(sockfd); // close socket
    fprintf(stdout, "Client exiting...\n");
    return 0;
}