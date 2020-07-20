#include	"acc.h"

ssize_t Recvfile(const int sockfd, const char* filename)
{
    /* Returns character in file on success and -1 on failure */
    char buffer[MAXLINE];
    ssize_t n;

    if ((n = Readline(sockfd, buffer, MAXLINE, 1)) == 0)
    {
        err_msg("Recvfile: server terminated prematurely");
        return -1;
    }
    if (n < 0)
    {
        err_msg("Recvfile: error");
        return -1;
    }
    Writefile(filename, buffer);  /* save the file to hard drive location */

    return n;
}

ssize_t Sendfile(const int sockfd, const char *filename)
{
    /* Returns > 0 on success and exits program on failure */
    char buffer[MAXLINE];
    char sendline[MAXLINE];
    ssize_t n;

    if (Existsfile(filename) == -1)
    {
        sprintf(sendline, "file does not exist\n");
        Writen(sockfd, sendline, strlen(sendline), 0);  /* notify that file is incoming */
        err_msg("Sendfile: the file does no exist");
        return -1;
    }

    n = Readfile(filename, buffer, MAXLINE);  /* get the number of bytes in the file */
    if (n > 0)
    {
        sprintf(sendline, "sending file\n");
        Writen(sockfd, sendline, strlen(sendline), 0);  /* notify that file is incoming */
        Writen(sockfd, buffer, n, 1);  /* send file */
        printf("Sendfile: %s has been sent\n", filename);
    }
    else
    {
        sprintf(sendline, "sendfile error\n");
        Writen(sockfd, sendline, strlen(sendline), 0);
        err_sys("Sendfile error");
        return -1;
    }
    return n;
}

int createclientconnectiontoserver(char* ipaddr, int port)
{
    /* return sockfd address on success and -1 on failure */
    int sockfd;
    struct sockaddr_in servaddr;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    Inet_pton(AF_INET, ipaddr, &servaddr.sin_addr);
    sockfd = Socket(AF_INET, SOCK_STREAM, 0);
    if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) == -1)
        return -1;

    return sockfd;
}

ssize_t getcurrenttimestr(char* timestr, int size)
{
    /* Returns 0 on success and -1 on failure */
    /* Inserts current time string into char array of size 64
     * format looks like this: Fri Oct 18 18:43:10 2019 */
    char timestrtemp[size];

    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    strftime(timestrtemp, size, "%c", tm);
    strcpy(timestr, timestrtemp);
    return 0;  /* SUCCESS */
}

ssize_t
Deletefile(const int sockfd, const char *filename)
{
    /* Returns 0 on success and -1 on failure */
    /* deletes a file from passed filename location */

    if (Existsfile(filename) == -1)  /* if the file doesn't exist, return failure */
    {
        err_msg("Deletefile: the file does no exist");
        return -1;
    }
    if (remove(filename) != 0)
    {
        err_msg("Deletefile: Error deleting file");
        return -1;
    }
    printf("Deleted %s.\n", filename);
    return 0;
}

void str_tolower(char* str)
{
    /* converts a string to all lower case */
    int i = 0;

    while(str[i] != '\0') {
        str[i] = (char)tolower(str[i]);
        i++;
    }
}

int split_str_by_space(char* ptrstr, char A[][MAXLINE], int values)
{
    /* Returns the number of args found */
    /* Gets args from user input and stores them in array A */
    char delim[] = " ";
    char str[MAXLINE];
    strcpy(str, ptrstr);
    char* ptr = strtok(str, delim);

    int i = 0;
    while(ptr != NULL && i < values)
    {
        strcpy(A[i], ptr);

        int len = strlen(A[i]);
        if (A[i][len-1] == '\n') /* Remove newline character */

            A[i][len-1] = '\0';

        ptr = strtok(NULL, delim); /* Get next value */

        i++;
    }

    return i; /* returns the amount of values found in array A */
}

int getnumberfromread(char* line, int len)
{
    if(line[len-1] == '\n')
    {
        line[len-1] = '\0';
    }
    return atoi(line);
}

int	inet_host2addr (char *host, struct in_addr *address)
{
    /* Returns 0 on success and -1 on failure */
    if ( isdigit(*host) )  /* Number is is in internet address dot notation */
    {
        address->s_addr = inet_addr(host);
    }
    else  /* hostname.domainname form */
    {
        struct hostent *HostData;

        HostData = gethostbyname(host);
        if ( HostData == NULL )
            return -1;
        *address = *((struct in_addr *) HostData->h_addr);
    }

    return 0;  /* No error */
}

int getsendcommand(char* sendline, char cmds[][MAXLINE])
{
    /* Returns 0 on success and -1 on failure */

    struct in_addr	address;  /* Socket Address  */

    if (inet_host2addr(cmds[1],&address) == -1)  /* get the dns or ip address string */
    {
        return -1;  /* invalid address provided */
    }

    sprintf(sendline, "%s %s %s %s\n", cmds[0], inet_ntoa(address), cmds[2], cmds[3]);

    return 0;
}