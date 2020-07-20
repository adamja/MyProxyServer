#include    "acc.h"

void* doit(void *arg);

int main(int argc, char **argv)
{
    if (argc != 2)
        err_quit("usage: proxy <client timeout>");

    printf("Proxy starting on port %d with a timeout of %s seconds...\n\n", PROXY_TCP_PORT, argv[1]);

    int	listenfd, connfd;
    pthread_t tid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;
    struct timeval tv;
    tv.tv_sec = atoi(argv[1]);
    tv.tv_usec = 0;

    /* cache table init */
    CT* ct_ptr = (CT*) malloc(sizeof(CT));
    ct_ptr->inputs = 0;
    ct_ptr->inuse = -1;
    for (int i = 0; i < MAXCACHESIZE; i++)
        ct_ptr->cfs[i].valid = -1;  /* initialise all cachefields to invalid */

    clearcachefolder();  /* clear the cache folder */

    listenfd = Socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(PROXY_TCP_PORT);

    Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

    Listen(listenfd, LISTENQ);

    while(1)
    {
        clilen = sizeof(cliaddr);
        connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
        setsockopt(connfd, SOL_SOCKET, SO_RCVTIMEO, (const char *) &tv,
                   sizeof tv);  /* add timeout to the client connection */

        /* Client Values pointer to send thread function commands and variables to  each thread */
        CV* cv_ptr = (CV*) malloc(sizeof(CV));  /* create a pointer on the heap to send values to client threads */
        cv_ptr->sockfd = connfd;
        cv_ptr->ct = ct_ptr;
        cv_ptr->cliaddr = &cliaddr;

        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_create(&tid, &attr, doit, cv_ptr);

//        clientloop(connfd, ct_ptr, &cliaddr);
    }
        Close(connfd);  /* parent closes connected socket */
        free(ct_ptr);  /* free cache table from the heap */
}

void* doit(void *arg)
{
    /* Client thread function */
    CV* cv = (CV*)arg;
    int connfd = cv->sockfd;
    CT* ct = cv->ct;
    struct sockaddr_in* cliaddr = cv->cliaddr;

    free(cv);  /* free the client values from the heap */
    Pthread_detach(pthread_self());  /* detach the thread from the main program */
    proxyloop(connfd, ct, cliaddr);  /* setup_files.sh the client loop */

    close(connfd);  /* close the connection to the client */
    return(NULL);  /* return nothing back to main */
}
