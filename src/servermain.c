#include    "acc.h"

int main(int argc, char **argv)
{
    if (argc != 2)
        err_quit("usage: proxy <server port>");

    printf("TCP server starting on port %s...\n", argv[1]);

    int	listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    while(1) {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);

        if ( (childpid = Fork()) == 0) {	 /*child process*/
            Close(listenfd);	 /*close listening socket*/
            serverloop(connfd);	 /*process the request*/
            exit(0);
        }
        Close(connfd);			 /*parent closes connected socket*/
    }
}