#include    "acc.h"

void serverloop(int sockfd)
{
    const int vals = 2;
    char cmds[vals][MAXLINE], recvline[MAXLINE], sendline[MAXLINE];

    while(1)
    {
        if ((Readline(sockfd, recvline, MAXLINE, 0)) == 0)
        {
            err_msg("serverloop: client connection closed\n");
            return;		/* connection closed by other end */
        }

        int val_rec = split_str_by_space(recvline, cmds, vals);

        if (val_rec == 2)
        {
            str_tolower(cmds[0]);

            char filename[MAXLINE];
            sprintf(filename, "./my_file/%s", cmds[1]);

            if ((strcmp(cmds[0], "get") == 0))
            {
                Sendfile(sockfd, filename, MAXLINE);
            }
            else if (strcmp(cmds[0], "delete") == 0)
            {
                if (Deletefile(sockfd, filename) == -1)
                {
                    sprintf(sendline, "%s does not exist\n", cmds[1]);
                }
                else
                {
                    sprintf(sendline, "OK; %s has been deleted\n", cmds[1]);
                }
                Writen(sockfd, sendline, strlen(sendline), 0);
            }
            else
            {
                err_msg("serverloop: invalid message from server");
            }
        }
    }

}