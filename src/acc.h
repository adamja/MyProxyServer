#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <dirent.h>
#include <sys/select.h>


#ifdef	HAVE_PTHREAD_H
#include	<pthread.h>
#endif


#define	MAXLINE	4096
#define	LISTENQ	1024
#define PROXY_TCP_ADDR  "127.0.0.1"
#define PROXY_TCP_PORT  51020
#define	SERV_TCP_PORT   51025
#define	SA	struct sockaddr
#define MAXCACHESIZE 1000
#define PROXYCLIENTTIMEOUT 5
#define h_addr h_addr_list[0]

typedef struct cachefield {
    char name[20];  /* size of 20 because max file size was specified to be 20 characters */
    char ip[255];  /* size of 255 because dns name can be up to 253 characters */
    int port;
    char cachetime[64];
    char lastreqtime[64];
    int totalreq;
    int valid;  /* a flag to state if the cache field is valid 0 if valid, -1 if invalid*/
} CF;

typedef struct cachetable {
    CF cfs[MAXCACHESIZE];  /* array of cachefields */
    int inputs;  /* the amount of entries in the cache table for quicker searching */
    int inuse;  /* if file is being used set this flag to 0, if not set to -1 */
} CT;

typedef  struct clientvalues {
    int sockfd;
    CT* ct;
    struct sockaddr_in* cliaddr;
} CV;

/* Our own header for the programs that use threads.
   Include this file, instead of "unp.h". */

#ifndef		__unp_pthread_h
#define		__unp_pthread_h

void	Pthread_create(pthread_t *, const pthread_attr_t *, void * (*)(void *), void *);
/*void * (*)(void *), void *);*/
void	Pthread_join(pthread_t, void **);
void	Pthread_detach(pthread_t);
void	Pthread_kill(pthread_t, int);

void	Pthread_mutexattr_init(pthread_mutexattr_t *);
void	Pthread_mutexattr_setpshared(pthread_mutexattr_t *, int);
void	Pthread_mutex_init(pthread_mutex_t *, pthread_mutexattr_t *);
void	Pthread_mutex_lock(pthread_mutex_t *);
void	Pthread_mutex_unlock(pthread_mutex_t *);

void	Pthread_cond_broadcast(pthread_cond_t *);
void	Pthread_cond_signal(pthread_cond_t *);
void	Pthread_cond_wait(pthread_cond_t *, pthread_mutex_t *);
void	Pthread_cond_timedwait(pthread_cond_t *, pthread_mutex_t *,
                               const struct timespec *);

void	Pthread_key_create(pthread_key_t *, void (*)(void *));
void	Pthread_setspecific(pthread_key_t, const void *);
void	Pthread_once(pthread_once_t *, void (*)(void));

#endif	/* __unp_pthread_h */
