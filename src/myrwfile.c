/*
 * read write file functions
 */

#include "acc.h"

ssize_t
Writefile(const char *filename, const char *buffer)
{
    /* Returns 0 on success and -1 on failure */
    FILE *fp;

    fp = fopen(filename, "w");
    if(fp == NULL)
    {
        err_msg("Writefile error: cannot open file for write");
        return -1;
    }

    Fputs(buffer, fp);
    fclose(fp);
    return 0;
}

ssize_t
Readfile(const char *filename, char *stream, size_t maxlen)
{
    /* Returns characters read on success and -1 on failure */
    int i, ch;

    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        err_msg("Readfile error: cannot open file for read");
        return -1;
    }

    for (i = 0; ( (i < (int)maxlen) && (ch = fgetc(fp)) != EOF ); i++)
    {
        stream[i] = ch;
    }

    Fclose(fp);

    return (ssize_t)i;
}

ssize_t
Existsfile(char* filename)
{
    /* Returns 0 if file exists and -1 if it does not */
    // You can also use R_OK, W_OK, and X_OK in place of F_OK to check for read permission, write permission,
    // and execute permission (respectively) rather than existence, and you can OR any of them together
    // (i.e. check for both read and write permission using R_OK|W_OK)
    if( access( filename, F_OK ) != -1 ) {
        // file exists
        return 0;
    } else {
        // file doesn't exist
        return -1;
    }
}

ssize_t
Readwritefile(char* filename)
{
    /* Returns 0 if file has read/write available and -1 if not */
    if( access( filename, R_OK|W_OK ) != -1 ) {
        // file has read/write permissions
        return 0;
    } else {
        // file doesn't have read write  permissions
        return -1;
    }
}
