#include    "acc.h"

void proxyloop(int sockfd, CT* ct, struct sockaddr_in* cliaddr)
{
    const int vals = 4;
    char cmds[vals][MAXLINE], recvline[MAXLINE], sendline[MAXLINE];

    while(1)
    {
        int n = Readline(sockfd, recvline, MAXLINE, 0);
        if (n == 0)
        {
            err_msg("clientloop: client connection closed\n");
            break;		/* connection closed by other end */
        }
        else if (n == -1)  /* socket timeout reached */
        {
            err_msg("clientloop: client socket timeout\n");
            break;		/* connection closed by other end */
        }

        int val_rec = split_str_by_space(recvline, cmds, vals);

        if (val_rec == 2)  /* cache table */
        {
            str_tolower(cmds[0]);  /* command to lowercase */
            str_tolower(cmds[1]);  /* command to lowercase */

            if ((strcmp(cmds[0], "look") != 0) || (strcmp(cmds[1], "cache") != 0))
            {
                err_msg("clientloop: invalid command received from client");
                continue;
            }
            if (proxylookprocess(sockfd, ct) != 0)
            {
                err_msg("clientloop: look cache process failure");
                continue;
            }
        }
        else if (val_rec == 4)  /* get or delete */
        {
            str_tolower(cmds[0]);  /* command to lowercase */
            str_tolower(cmds[1]);  /* ip or dns to lowercase */

            if ((strcmp(cmds[0], "get") == 0))  /* get command */
            {
                CF cf;
                if (updatecachefield(&cf, cmds[3], cmds[1], atoi(cmds[2]), NULL, NULL, NULL, 0) == -1)
                {
                    err_msg("error creating cache file. Invalid input?");
                    continue;
                }
                if (proxygetprocess(sockfd, &cf, ct) != 0)
                {
                    err_msg("clientloop: get process failure");
                    continue;
                }
                printf("Successfully sent '%s' to client\n", cmds[3]);  /* SUCCESS */
            }
            else if (strcmp(cmds[0], "delete") == 0)  /* delete command */
            {
                CF cf;
                if (updatecachefield(&cf, cmds[3], cmds[1], atoi(cmds[2]), NULL, NULL, NULL, -1) == -1)
                {
                    err_msg("error creating cache file. Invalid input?");
                    continue;
                }
                if (proxydeleteprocess(sockfd, &cf, ct) != 0)
                {
                    err_msg("clientloop: delete process failure");
                    continue;
                }
                printf("Successfully deleted '%s' from proxy and server\n", cmds[3]);  /* SUCCESS */
            }
            else
            {
                err_msg("clientloop: invalid command received from client");
                continue;
            }
        }
        else if(val_rec == 1)  /* exit */
        {
            str_tolower(cmds[0]);
            if (strcmp(cmds[0], "goodbye") == 0)  /* exit command */
            {
                char* ip = inet_ntoa(cliaddr->sin_addr);
                int port = cliaddr->sin_port;
                sprintf(sendline, "Goodbye Client (%s,%d). Thank you for using MSP!\n", ip, port);
                Writen(sockfd, sendline, strlen(sendline), 0);
                break;
            }
            else
            {
                err_msg("clientloop: invalid command received from client");
                continue;
            }
        }
        else
        {
            err_msg("clientloop: invalid command received from client");
            continue;
        }
    }
}