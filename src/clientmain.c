#include    "acc.h"

int main(int argc, char **argv)
{
    if (argc != 3)
        err_quit("usage: client <proxy_ipaddress> <proxy_port>");

    printf("TCP client starting with connection to proxy on %s,%s\n\n", argv[1], argv[2]);

    int sockfd;
    struct sockaddr_in servaddr;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(atoi(argv[2]));
    Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);


    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
    clientloop(sockfd);
    exit(0);
}