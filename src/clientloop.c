#include "acc.h"

void clientloop(int sockfd)
{
    const int vals = 4;
    char cmds[vals][MAXLINE], input[MAXLINE], recvline[MAXLINE], sendline[MAXLINE];

    while(1) {
        printf("Client > ");
        Fgets(input, MAXLINE, stdin);

        int val_rec = split_str_by_space(input, cmds, vals);

        if (val_rec == 2)
        {
            str_tolower(cmds[0]);
            str_tolower(cmds[1]);

            if ((strcmp(cmds[0], "look") != 0) || (strcmp(cmds[1], "cache") != 0))
            {
                err_msg("clientloop: invalid command received from client");
                continue;
            }

            Writen(sockfd, input, strlen(input), 0);  /* send look cache command to proxy */
            if (Readline(sockfd, recvline, MAXLINE, 0) == 0)  /* get the number of cache files being sent */
            {
                err_quit("clientloop: server terminated prematurely");
                break;
            }

            int n = getnumberfromread(recvline, strlen(recvline));

            if (n > 0)
            {
                printf("*** %d cache entries found ***\n\n", n);
            }
            else
            {
                printf("Proxy cache is empty\n");
            }

            for (int i = 0; i < n; i++)
            {
                if (Readline(sockfd, recvline, MAXLINE, 1) == 0)
                    err_quit("clientloop: server terminated prematurely");

                Fputs(recvline, stdout);  /* print to console */
                printf("\n");  /* add a new line between cache entries */
            }
        }
        else if (val_rec == 4)
        {
            if ((strlen(cmds[3]) > 20))
            {
                err_msg("getsendcommand: filename must be smaller than 20 characters");
                continue;
            }

            char saveloc[MAXLINE];
            sprintf(saveloc, "./download-file/%s", cmds[3]);

            str_tolower(cmds[0]);
            str_tolower(cmds[1]);

            if ((strcmp(cmds[0], "get") == 0))
            {
                if(getsendcommand(sendline, cmds) == -1)
                {
                    err_msg("getsendcommand: invalid address provided");
                    continue;
                }

                Writen(sockfd, sendline, strlen(input), 0);  /* send command to proxy */

                if ((Readline(sockfd, recvline, MAXLINE, 0)) == 0)  /* get response  from proxy */
                {
                    err_msg("clientloop: client connection closed\n");
                    return;		/* connection closed by other end */
                }

                if (strcmp(recvline, "sending file\n") != 0)
                {
                    printf("clientloop: %s does not exist\n", cmds[3]);
                }
                else
                {
                    if (Recvfile(sockfd, saveloc) > 0)
                    {
                        printf("%s is downloaded\n", cmds[3]);
                    }
                    else
                    {
                        printf("%s does not exist\n", cmds[3]);
                    }
                }
            }
            else if (strcmp(cmds[0], "delete") == 0)
            {
                if(getsendcommand(sendline, cmds) == -1)
                {
                    err_msg("getsendcommand: invalid address provided");
                    continue;
                }

                Writen(sockfd, sendline, strlen(input), 0);  /* send command to proxy */

                if (Readline(sockfd, recvline, MAXLINE, 0) == 0)
                    err_quit("clientloop: server terminated prematurely");
                printf(recvline);
            }
            else
            {
                err_msg("clientloop: invalid input");
                continue;
            }
        }
        else if (val_rec == 1)
        {
            str_tolower(cmds[0]);
            if (strcmp(cmds[0], "exit") == 0)
            {
                sprintf(sendline, "goodbye\n");
                Writen(sockfd, sendline, strlen(sendline), 0);
                if ((Readline(sockfd, recvline, MAXLINE, 0)) == 0)
                {
                    err_msg("clientloop: server connection closed");
                    break;
                }
                printf("%s", recvline);
                break;
            }
            else
            {
                err_msg("clientloop: invalid input");
                continue;
            }
        }
        else
        {
            err_msg("clientloop: invalid input");
            continue;
        }
    }
    close(sockfd);
}