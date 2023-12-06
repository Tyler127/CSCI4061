#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BACKLOG 5
#define STRSZ 100
#define PORT 3000 

/****************************/
// IMPORTANT: ERROR CHECK ALL SOCKET APIs
/****************************/
int main(int argc, char *argv[]) {
    int listen_fd, conn_fd;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0); // create listening socket
    if(listen_fd == -1)
        perror("socket error");

    struct sockaddr_in servaddr;
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Listen to any of the network interface (INADDR_ANY)
    servaddr.sin_port = htons(PORT); // Port number

    int ret = bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)); // bind address, port to socket
    if(ret == -1)
        perror("bind error");

    ret = listen(listen_fd, BACKLOG); // listen on the listen_fd
    if(ret == -1)
        perror("listen error");

    struct sockaddr_in clientaddr;
    socklen_t clientaddr_len = sizeof(clientaddr);
    conn_fd = accept(listen_fd, (struct sockaddr *) &clientaddr, &clientaddr_len); // accept a request from a client
    if(conn_fd == -1)
        perror("accept error");

    char msg[STRSZ];
    bzero(msg, STRSZ); // initialize msg with '\0'
    ret = recv(conn_fd, msg, STRSZ, 0); // receive data from client
    if(ret == -1)
        perror("recv error");
    fprintf(stdout, "Client: %s\n", msg);

    close(conn_fd); // close sockets
    close(listen_fd);
    fprintf(stdout, "Server exiting...\n");
    return 0;

}
