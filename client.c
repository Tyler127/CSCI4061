#include "common.h"

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

    // Packet to send to server
    char msg[STRSZ];
    bzero(msg, STRSZ); // initialize msg with '\0'
    fprintf(stdout, "Input string to reverse: ");
    fscanf(stdin, "%[^\n]s", msg); // Read input with space until a newline
    setbuf(stdin, NULL);


    Packet packet;
    // Fill the content of packet, check sample/client.c
    packet.operation = htons(PROTO_REV);
    strcpy(packet.data, msg);

    // Serialize the packet, check common.h and sample/client.c
    char *serializedData = serializePacket(&packet);

    // send the serialized data to server
    ret = send(sockfd, serializedData, PACKETSZ, 0);
    if(ret == -1)
        perror("send error");

    // Receive acknowledgment packet with reversed string from server
    char recvdata[PACKETSZ];
    memset(recvdata, 0, PACKETSZ);
    ret = recv(sockfd, recvdata, PACKETSZ, 0);
    if(ret == -1)
        perror("recv error");

    // Deserialize the received data, check common.h and sample/client.c
    Packet *ackpacket = deserializeData(recvdata);

    fprintf(stdout, "Reversed string: %s\n", ackpacket->data);

    free(ackpacket);
    ackpacket = NULL;    
    
    close(sockfd); // close socket
    fprintf(stdout, "Client exiting...\n");
    return 0;
}
