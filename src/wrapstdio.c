/*
 * Standard I/O wrapper functions.
 */

#include	"acc.h"

void
Fclose(FILE *fp)
{
	if (fclose(fp) != 0)
		err_sys("fclose error");
}

FILE *
Fdopen(int fd, const char *type)
{
	FILE	*fp;

	if ( (fp = fdopen(fd, type)) == NULL)
		err_sys("fdopen error");

	return(fp);
}

char *
Fgets(char *ptr, int n, FILE *stream)
{
	char	*rptr;

	if ( (rptr = fgets(ptr, n, stream)) == NULL && ferror(stream))
		err_sys("fgets error");

	return (rptr);
}

FILE *
Fopen(const char *filename, const char *mode)
{
	FILE	*fp;

	if ( (fp = fopen(filename, mode)) == NULL)
		err_sys("fopen error");

	return(fp);
}

void
Fputs(const char *ptr, FILE *stream)
{
	if (fputs(ptr, stream) == EOF)
		err_sys("fputs error");
}

//void Writefile(const char *filename, const char *buffer)
//{
//    FILE *fp;
//    fp = Fopen(filename, "w");
//    Fputs(buffer, fp);
//    Fclose(fp);
//}
//
//int Readfile(const char *filename, char *stream)
//{
//    int i, ch;
//
//    FILE *fp;
//    fp = Fopen(filename, "r");
//
//    for (i = 0; (i < MAXLINE && ((ch = fgetc(fp))) != EOF); i++)
//    {
//        stream[i] = ch;
//    }
//
//    Fclose(fp);
//
//    return i;
//}