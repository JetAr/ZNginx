#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>


int main(int argc, char **argv)
{
    int i;
    time_t bt;
    char *cp;


    time(&bt);

    printf("HTTP/1.0 200 OK\n");
    printf("Date: %s", ctime(&bt));

    printf("Mime-Version: 1.0\n");
    printf("Last-Modified: %s", ctime(&bt));
    printf("Content-Type: text/html\n\n");

    printf("<HTML><BODY>\n");
    printf("<H2>Test av externa program</H2><HR>\n");

    printf("<P><B>Klockan:</B> %s\n<HR>\n", ctime(&bt));

    printf("<H3>Process credentials</H3>\n");
    printf("Uid = %d<BR>\n", (int) getuid());
    printf("EUid = %d<BR>\n", (int) geteuid());
    printf("Gid = %d<BR>\n", (int) getgid());
    printf("Egid = %d<BR>\n", (int) getegid());

    printf("<H3>Environmental variables:</H3>\n");

    if ((cp = getenv("REQUEST_METHOD")))
        printf("<B>REQUEST_METHOD</B> = %s<BR>\n", cp);

    if ((cp = getenv("HTTP_FROM")))
        printf("<B>HTTP_FROM</B> = %s<BR>\n", cp);

    if (argv)
    {
        printf("<HR>Argumenten till mig är:\n");
        printf("<UL>\n");
        for (i = 0; argv[i]; i++)
            printf("<LI>%s\n", argv[i]);
    }
    else
        printf("Inga argument till mig idag.\n");

    printf("</BODY></HTML>\n");

    return 0;
}


