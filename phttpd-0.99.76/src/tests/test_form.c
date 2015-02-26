#include <stdio.h>
#include <time.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    int i;
    time_t bt;
    char *cp;
    char buf[256];
    int len;


    time(&bt);

    printf("Content-Type: text/html\n\n");
    printf("<HTML><BODY>\n");
    printf("<B>Test av forms/post</B><P>\n");

    if ((cp = getenv("REQUEST_METHOD")))
        printf("<B>REQUEST_METHOD</B> = %s<BR>\n", cp);

    if (cp && (strcmp(cp, "PUT") == 0 || strcmp(cp, "POST") == 0))
    {
        printf("<HR>\n");

        cp = getenv("CONTENT_LENGTH");
        if (cp)
        {
            len = atoi(cp);
            fread(buf, 1, len, stdin);
            buf[len] = '\0';
            printf("<B>Data:</B> %s\n", buf);
        }
        else
            printf("No content-length defined\n");

        printf("<HR>\n");
    }

    {
        char *cp = getenv("QUERY_STRING");

        if (cp)
            printf("<BR>QUERY_STRING är: %s<BR>\n", cp);
    }

    if (argv)
    {
        printf("Argumenten till mig är:\n");
        printf("<UL>\n");
        for (i = 0; argv[i]; i++)
            printf("<LI>%s\n", argv[i]);
    }
    else
        printf("Inga argument till mig idag.\n");

    printf("<FORM METHOD=GET ACTION=\"%s\">\n", getenv("SCRIPT_NAME"));
    printf("<UL>\n");
    printf("<LI><INPUT TYPE=\"test\" NAME=\"foo\" MAXLENGTH=\"10\">\n");
    printf("</UL>\n");
    printf("<INPUT TYPE=\"submit\">\n");
    printf("</FORM>\n");

    printf("</BODY></HTML>\n");

    return 0;
}


