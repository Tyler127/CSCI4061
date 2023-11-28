#include "common.h"

#define BACKLOG 5

/****************************/
// IMPORTANT: ERROR CHECK ALL SOCKET APIs
/****************************/

void strrev(char *data){
    int l = strlen(data);
    int i = 0, j = l - 1;
    while(i < j) {
        char c = data[i];
        data[i] = data[j];
        data[j] = c;
        i++;
        j--;
    }
}

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

    // Server receiving packet from client
    char recvdata[PACKETSZ];
    memset(recvdata, 0, PACKETSZ);
    ret = recv(conn_fd, recvdata, PACKETSZ, 0);
    if(ret == -1)
        perror("recv error");

    // Deerialize the received data, check common.h and sample/server.c
    Packet *recvpacket = deserializeData(recvdata);

    fprintf(stdout, "String to reverse: %s\n", recvpacket->data);

    // reverse string
    strrev(recvpacket->data);

    // Send ack packet to client
    Packet packet;
    packet.operation = htons(PROTO_ACK);
    strcpy(packet.data, recvpacket->data);
    // Fill the content of packet, check sample/server.c


    // Serialize the packet, check common.h and sample/server.c
    char *serializedData = serializePacket(&packet);

    // send the serialized data to client
    ret = send(conn_fd, serializedData, PACKETSZ, 0);
    if(ret == -1)
    perror("send error");
    
    free(serializedData);
    serializedData = NULL;

    close(conn_fd); // close sockets
    close(listen_fd);
    fprintf(stdout, "Server exiting...\n");
    return 0;
}
