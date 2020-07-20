# include "acc.h"

int findcachefieldposition(CF* cf, CT* ct);
int findavailablecachefieldposition(CT* ct);
ssize_t updatecachefield(CF* cf, char* name, char* ip, int port, char* cachetime, char* lastreqtime, int totalreq, int valid);
void clonecachefield(CF* dest, CF* src);

ssize_t removecachetablefile()
{
    /* Returns 0 on success and -1 on failure */
    char f[] = "./cache_table";

    if (Existsfile(f) != 0)  /* cache table doesn't exists */
    {
        err_msg("removecachetablefile: cache_table doesn't exist");
        return -1;
    }
    if (remove(f) != 0)  /* Returns 0 on success */
    {
        err_msg("removecachetablefile: failed to remove cache_table");
        return -1;
    }
        return 0;
}

ssize_t sendcachetable(int sockfd, CT* ct)
{
    /* Returns 0 on success, -1 on failure and -2 if cachefile is in use */
    /* Sends a copy of the cache table to the sockfd passed */

    char send[MAXLINE];
    int n = 0;  /* the number of entries sent */
    int total_cfs = ct->inputs;

    if (ct->inuse == 0)
    {
        return -2;
    }
    if (ct->inputs == 0)
    {
        sprintf(send, "%d\n", 0);
        Writen(sockfd, send, strlen(send), 0);  /* send that there are 0 inputs being sent to the client */
        return 0;  /* cachefile is empty, return 0 entries */
    }

    ct->inuse = 0;  /* set cache table to in use */

    sprintf(send, "%d\n", total_cfs);
    Writen(sockfd, send, strlen(send), 0);  /* send the number of cfs you are going to send to the client */

    for(int i = 0; i < MAXCACHESIZE; i++)
    {
        if (ct->cfs[i].valid != 0)
            continue;

        sprintf(send,
                "%s\nLocation: %s, %d\nCached time: %s\nLast requested time: %s\nTotalrequests: %d\n",
                ct->cfs[i].name,
                ct->cfs[i].ip,
                ct->cfs[i].port,
                ct->cfs[i].cachetime,
                ct->cfs[i].lastreqtime,
                ct->cfs[i].totalreq
                );
        Writen(sockfd, send, strlen(send), 1);
        n += 1;

        if (n >= total_cfs)
            break;
    }

    ct->inuse = -1;  /* set cache table to not in use */
    return n;
}

ssize_t fieldnameincachetable(CF* cf, CT*  ct)
{
    /* returns 0 if cf.name is found, -1 if not found in cache table */
    if (ct->inuse == 0)
    {
        return -2;
    }
    if (ct->inputs == 0)
    {
        return -1;  /* cachefile is empty */
    }

    int pos;
    int valid = -1;
    ct->inuse = 0;  /* set cache table to in use */

    if ((pos = findcachefieldposition(cf, ct)) >= 0)
    {
        valid = 0;
    }

    ct->inuse = -1;  /* set cache table to not in use */

    return valid;
}

ssize_t validfieldincachetable(CF* cf, CT* ct, int checktotalreq)
{
    /* Returns 0 if file is valid, -1 if file is invalid and -2 if cachefile is in use */
    /* Confirms that the current cache is valid. It is not valid if total req is
     * no larger than 2 */

    if (ct->inuse == 0)
    {
        return -2;
    }
    if (ct->inputs == 0)
    {
        return -1;  /* cachefile is empty */
    }

    int pos;
    int valid = -1;
    ct->inuse = 0;  /* set cache table to in use */

    if ((pos = findcachefieldposition(cf, ct)) >= 0)
    {
        /* check that file is from same server (matching ip and port) */
        if ( (strcmp(ct->cfs[pos].ip, cf->ip) == 0) && (ct->cfs[pos].port == cf->port) )
        {
            if (checktotalreq == 1)  /* if check total requests flag is 1 */
            {
                if (ct->cfs[pos].totalreq <= 2)   /* if the total requests is no larger than 3, cache is valid */
                    valid = 0;
            }
            else  /* do not check total requests, and return that there is a valid file */
                valid = 0;
        }
    }

    ct->inuse = -1;  /* set cache table to not in use */

    return valid;
}

ssize_t addtocachetable(CF* cf, CT* ct)
{
    /* Returns 0 on success, -1 on failure and -2 if cachefile is in use */
    /* reqtime is equal to cache time for new entries, and totalreq = 1 for new entries */

    if (ct->inuse == 0)
    {
        return -2;
    }
    if (ct->inputs >= MAXCACHESIZE)
    {
        return -1;  /* cachefile is full */
    }

    int pos;
    int added = -1;

    ct->inuse = 0;  /* set cache table to in use */

    if ((pos = findavailablecachefieldposition(ct)) != -1)
    {
        clonecachefield(&(ct->cfs[pos]), cf);
        ct->inputs += 1;
        added = 0;
    }

    ct->inuse = -1;  /* set cache table to not in use */

    return added;
}

ssize_t removefromcachetable(CF* cf, CT* ct)
{
    /* Returns 0 on success, -1 on failure and -2 if cachefile is in use */
    if (ct->inuse == 0)
    {
        return -2;
    }
    if (ct->inputs == 0)
    {
        return -1;  /* cachefile is empty */
    }

    int pos;
    int deleted = -1;
    ct->inuse = 0;  /* set cache table to in use */

    if ((pos = findcachefieldposition(cf, ct)) >= 0)
    {
        ct->cfs[pos].valid = -1;
        ct->inputs -= 1;
        deleted = 0;
    }

    ct->inuse = -1;  /* set cache table to not in use */

    return deleted;
}

ssize_t updatecachetablefield(CF* cf, CT* ct, char* lastreqtime)
{
    /* Returns 0 on success, -1 on failure and -2 if cachefile is in use */

    if (ct->inuse == 0)
    {
        return -2;
    }
    if (ct->inputs == 0)
    {
        return -1;  /* cachefile is empty */
    }

    int pos;
    int updated = -1;
    ct->inuse = 0;  /* set cache table to in use */

    if ((pos = findcachefieldposition(cf, ct)) >= 0)
    {
        strcpy(ct->cfs[pos].lastreqtime, lastreqtime);  /* copy updated lastreqtime to cachefile */
        ct->cfs[pos].totalreq += 1;  /* increment the total requests by 1 */
        updated = 0;
    }

    ct->inuse = -1;  /* set cache table to not in use */

    return updated;
}

/* ONLY CALL FROM ABOVE FUNCTIONS - does not check if the cache file is in use */
int findcachefieldposition(CF* cf, CT* ct)
{

    /* Returns the position of a cache field in cachefile cfs array if the name matches the
     * cf passed to the function (0 to MAXCACHESIZE). Else it returns -1 if no match */

    int n = 0;  /* the number of valid entries found */
    int pos = -1;
    int total_cfs = ct->inputs;

    for(int i = 0; i < MAXCACHESIZE; i++)
    {
        if (ct->cfs[i].valid != 0)  /* if the input is not valid, go to the next one */
            continue;

        n += 1;  /* increment valid inputs found counter */

        if (strcmp(cf->name, ct->cfs[i].name) == 0)  /* check if the file name is the same */
        {
            pos = i;
            break;
        }
        if (n >= total_cfs)  /* if all inputs have been found stop looking */
            break;
    }

    return pos;
}

/* ONLY CALL FROM ABOVE FUNCTIONS - does not check if the cache file is in use */
int findavailablecachefieldposition(CT* ct)
{
    /* Returns the first position in the cache table where valid == -1 so that it can be used to
     * store a new cachefield entry on success and -1 on failure to find a position */

    int pos = -1;

    for(int i = 0; i < MAXCACHESIZE; i++)
    {
        if (ct->cfs[i].valid == -1)
        {
            pos = i;
            break;
        }
    }

    return pos;
}

ssize_t updatecachefield(CF* cf, char* name, char* ip, int port, char* cachetime, char* lastreqtime, int totalreq, int valid)
{
    /* Returns 0 if at least one field has been updated, -1 on failure to update any fields */
    /* Must pass a valid struct cachefield and at least on other field. If a field is not required
     * to be updated, pass NULL to field */
    int update = -1;

    if (cf == NULL)
    {
        return -1;
    }
    if (name != NULL)
    {
        strcpy(cf->name, name);
        update = 0;
    }
    if (ip != NULL)
    {
        strcpy(cf->ip, ip);
        update = 0;
    }
    if (port != NULL)
    {
        cf->port = port;
        update = 0;
    }
    if (cachetime != NULL)
    {
        strcpy(cf->cachetime, cachetime);
        update = 0;
    }
    if (lastreqtime != NULL)
    {
        strcpy(cf->lastreqtime, lastreqtime);
        update = 0;
    }
    if (totalreq != NULL)
    {
        cf->totalreq = totalreq;
        update = 0;
    }

    cf->valid = valid;  /* can't check for NULL because int 0 is equal to null */

    return update;
}

void clonecachefield(CF* dest, CF* src)
{
    /* clones a cf from one to the another */
    strcpy(dest->name, src->name);
    strcpy(dest->ip, src->ip);
    dest->port = src->port;
    strcpy(dest->cachetime, src->cachetime);
    strcpy(dest->lastreqtime, src->lastreqtime);
    dest->totalreq = src->totalreq;
    dest->valid = src->valid;
}