# include "acc.h"

ssize_t getfilefromserver(CF* cf);
ssize_t fileincachecf(CF* cf);
ssize_t fileincache(char* filename);
ssize_t addfiletocachecf(CF* cf, const char* buffer);
ssize_t addfiletocache(char* filename, const char* buffer);
ssize_t deletefilefromserver(CF* cf);
ssize_t removefilefromcachecf(CF* cf);
ssize_t removefilefromcache(char* filename);


ssize_t proxylookprocess(int sockfd, CT* ct)
{
    /* Returns 0 on success and -1 on failure */

    if (sendcachetable(sockfd, ct) < 0)
    {
        return -1;
    }
    return 0;
}

ssize_t proxygetprocess(int sockfd, CF* cf, CT* ct)
{
    /* Returns 0 on success and -1 on failure */
    int sendproxyfile = 0;  /* Can use proxy file copy if 0, cannot if -1 */
    char f[MAXLINE], sendline[MAXLINE], time[MAXLINE];
    sprintf(f, "./my_cache/%s", cf->name);

    if (fileincache(cf->name) == -1)  /* check if the file is saved in the my_cache folder */
    {
        sendproxyfile = -1;
    }
    if (validfieldincachetable(cf, ct, 1) == -1)  /* check that the file has a valid cache entry */
    {
        sendproxyfile = -1;
    }

    if (sendproxyfile == 0)  /* cache is valid, send proxy file to client */
    {
        if (Sendfile(sockfd, f) == -1)  /* send the file from my_cache folder to client */
        {
            err_msg("proxygetprocess: failed to send cache file");
            return -1;
        }
        getcurrenttimestr(time, MAXLINE);  /* update time string */
        if (updatecachetablefield(cf, ct, time) == -1)  /* update cache entry */
        {
            err_msg("proxygetprocess: error updating file in cache table");
            return -1;
        }
        return 0;  /* SUCCESS - File sent from cache */
    }
    else  /* cache is invalid, get file from server */
    {
        if (getfilefromserver(cf) == -1)
        {
            err_msg("proxygetprocess: failed to get file from server");
            sprintf(sendline, "file does not exist\n");
            Writen(sockfd, sendline, strlen(sendline), 0);
            return -1;
        }
        if (Sendfile(sockfd, f) == -1)
        {
            err_msg("proxygetprocess: failed to send cache file");
            return -1;
        }
        if (fieldnameincachetable(cf, ct) == 0)  /* check if there is a file entry with the same name already in cache table */
        {
            if (removefromcachetable(cf, ct) == -1)  /* remove the cache entry from the table */
            {
                err_msg("proxygetprocess: error removing old cf from cache table");
                return -1;
            }
        }
        getcurrenttimestr(time, MAXLINE);  /* update time string */
        updatecachefield(cf, NULL, NULL, NULL, time, time, 1, 0);  /* add cache time and last request time to file */
        if (addtocachetable(cf, ct))  /* if file is not in cache table, add a new entry */
        {
            err_msg("proxygetprocess: error adding cf to cache table");
            return -1;
        }

        return 0;  /* SUCCESS - File received from server */
    }
}

ssize_t proxydeleteprocess(int sockfd, CF* cf, CT* ct)
{
    /* Returns 0 on success and -1 on failure */
    int success = 0;
    char sendline[MAXLINE];

    if (deletefilefromserver(cf) == -1)  /* remove file from server */
    {
        sprintf(sendline, "'%s' does not exist\n", cf->name);
        Writen(sockfd, sendline, strlen(sendline), 0);
        err_msg("proxydeleteprocess: failed to delete file from server");
        success = -1;
    }
    else
    {
        sprintf(sendline, "OK; '%s' has been deleted\n", cf->name);
        Writen(sockfd, sendline, strlen(sendline), 0);
    }
    if (fileincache(cf) == 0)  /* check if the file is in my_cache */
    {
        if (removefilefromcache(cf) == -1)  /* remove file from proxy */
        {
            err_msg("proxydeleteprocess: failed to delete file from cache");
            success = -1;
        }
    }
    if (validfieldincachetable(cf, ct, 0) == 0)
    {
        if (removefromcachetable(cf, ct) == -1)  /* remove cache field from proxy */
        {
            err_msg("proxydeleteprocess: failed to delete file from cache");
            success = -1;
        }
    }

    return success;
}

ssize_t getfilefromserver(CF* cf)
{
    /* Returns 0 on success and -1 on failure */
    int sockfd;  /* server sockfd */
    char sendline[MAXLINE], recvline[MAXLINE],f[MAXLINE];
    sprintf(f, "./my_cache/%s", cf->name);
    sprintf(sendline, "get %s\n", cf->name);

    sockfd = createclientconnectiontoserver(cf->ip, cf->port);  /* create a sockfd to server */
    if (sockfd == -1)
    {
        err_msg("getfilefromserver: error creating socket connection");
        return -1;
    }

    Writen(sockfd, sendline, strlen(sendline), 0);
    if ((Readline(sockfd, recvline, MAXLINE, 0)) == 0)
    {
        err_msg("getfilefromserver: server connection closed");
        close(sockfd);  /* close the server connection */
        return -1;
    }
    if (strcmp(recvline, "sending file\n") != 0)  /* server does not have file or is unable to send file */
    {
        err_msg("getfilefromserver: unable to get file from server");
        close(sockfd);  /* close the server connection */
        return -1;
    }
    if (Recvfile(sockfd, f) < 0)
    {
        err_msg("getfilefromserver: unable to get file from server");
        close(sockfd);  /* close the server connection */
        return -1;
    }
    printf("getfilefromserver: %s has been saved\n", f);
    close(sockfd);  /* close the server connection */

    return 0;  /* SUCCESS */
}

ssize_t deletefilefromserver(CF* cf)
{
    /* Returns 0 on success and -1 on failure */

    int sockfd;  /* server sockfd */
    char sendline[MAXLINE], recvline[MAXLINE],f[MAXLINE];
    sprintf(f, "./my_cache/%s", cf->name);
    sprintf(sendline, "delete %s\n", cf->name);

    sockfd = createclientconnectiontoserver(cf->ip, cf->port);  /* create a sockfd to server */
    if (sockfd == -1)
    {
        err_msg("deletefilefromserver: error creating socket connection");
        return -1;
    }

    Writen(sockfd, sendline, strlen(sendline), 0);
    if (Readline(sockfd, recvline, MAXLINE, 0) == 0)
    {
        err_msg("deletefilefromserver: server connection closed");
        close(sockfd);
        return -1;
    }
    if (strncmp(recvline, "OK;", 3) != 0)
    {
        err_msg("deletefilefromserver: failed to delete file from server");
        return -1;
    }

    return 0;
}

ssize_t fileincachecf(CF* cf)
{
    return fileincache(cf->name);
}

ssize_t fileincache(char* filename)
{
    /* Returns 0 on success and -1 on failure */
    char f[MAXLINE];
    sprintf(f, "./my_cache/%s", filename);

    if (Existsfile(f) == -1)
        return -1;
    return 0;
}

ssize_t addfiletocachecf(CF* cf, const char* buffer)
{
    return addfiletocache(cf->name, buffer);
}

ssize_t addfiletocache(char* filename, const char* buffer)
{
    /* Returns 0 if file is added and -1 on failure */
    char f[MAXLINE];
    sprintf(f, "./my_cache/%s", filename);

    if (fileincache(filename) == -1)
    {
        err_msg("addfiletocache: file does not exist");
        return -1;
    }
    if (Writefile(f, buffer) == -1)
    {
        err_msg("addfiletocache: cannot write file to cache");
        return -1;
    }
    return 0;
}

ssize_t removefilefromcachecf(CF* cf)
{
    return removefilefromcache(cf->name);
}

ssize_t removefilefromcache(char* filename)
{
    /* Returns 0 on success and -1 on failure */
    char f[MAXLINE];
    sprintf(f, "./my_cache/%s", filename);

    if (fileincache(filename) == -1)
    {
        err_msg("removefilefromcache: file does not exist");
        return -1;
    }
    if (remove(f) != 0)  /* Returns 0 on success */
    {
        err_msg("removefilefromcache: failed to remove file from cache");
        return -1;
    }
    return 0;
}

void clearcachefolder()
{
    DIR *folder = opendir("./my_cache");
    struct dirent* next_file;
    char filepath[256];

    while ( (next_file = readdir(folder)) != NULL )
    {
        // build the path for each file in the folder
        sprintf(filepath, "%s/%s", "./my_cache", next_file->d_name);

    }
    closedir(folder);
}