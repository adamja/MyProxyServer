/*
 * writen functions
 */

#include "acc.h"

ssize_t						/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n, int isfile)
{
    size_t		nleft;
    ssize_t		nwritten;
    const char	*ptr;
    char fptr[n+1];

    ptr = vptr;
    nleft = n;

    /* if isfile flag is set to 1 it adds an EOF char to the end of the char array to send to indicate end of read */
    if (isfile == 1)
    {
        nleft++;
        strcpy(fptr, ptr);
        fptr[n] = EOF;
        ptr = fptr;
    }

    while (nleft > 0) {
        if ( (nwritten = write(fd, ptr, nleft)) <= 0) {
            if (errno == EINTR)
                nwritten = 0;		/* and call write() again */
            else
                return(-1);			/* error */
        }

        nleft -= nwritten;
        ptr   += nwritten;
    }

    return(n);
}
/* end writen */

void
Writen(int fd, void *ptr, size_t nbytes, int isfile)
{
    if (writen(fd, ptr, nbytes, isfile) != nbytes)
        err_sys("writen error");
}