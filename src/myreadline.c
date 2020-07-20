/*
 * readline functions
 */

#include "acc.h"

static ssize_t
my_read(int fd, char *ptr)
{
    static int	read_cnt = 0;
    static char	*read_ptr;
    static char	read_buf[MAXLINE];

    if (read_cnt <= 0) {
        again:
        if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0) {
            if (errno == EINTR)
                goto again;
            return(-1);
        } else if (read_cnt == 0)
            return(0);
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return(1);
}

ssize_t
readline(int fd, void *vptr, size_t maxlen, int isfile)
{
    /* isfile flag look for EOF as the terminator and ignores newlines */
    int		n, rc;
    char	c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ( (rc = my_read(fd, &c)) == 1) {
            if (c == EOF)
                break;
            *ptr++ = c;
            if (isfile == 0 && c == '\n')  /* don't stop at newline if is a file */
                break;	/* newline is stored, like fgets() */

        } else if (rc == 0) {
            if (n == 1)
                return(0);	/* EOF, no data read */
            else
                break;		/* EOF, some data was read */
        } else
            return(-1);		/* error, errno set by read() */
    }

    *ptr = 0;	/* null terminate like fgets() */
    return(n);
}
/* end readline */

ssize_t
Readline(int fd, void *ptr, size_t maxlen, int isfile)
{
    ssize_t	n;

    if ( (n = readline(fd, ptr, maxlen, isfile)) < 0)
    {
        err_msg("Readline: error");
        return -1;
    }

    return(n);
}