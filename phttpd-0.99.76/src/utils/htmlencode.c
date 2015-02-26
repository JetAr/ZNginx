#include <stdio.h>

static void output(int c)
{
    switch (c)
    {
    case '<':
        fputs("&lt;", stdout);
        break;

    case '>':
        fputs("&gt;", stdout);
        break;

    case '"':
        fputs("&quot;", stdout);
        break;

    case '&':
        fputs("&amp;", stdout);
        break;

#ifdef OVER_MY_DEAD_BODY
    case 0xc4:
        fputs("&Auml;", stdout);
        break;

    case 0xe4:
        fputs("&auml;", stdout);
        break;

    case 0xd6:
        fputs("&Ouml;", stdout);
        break;

    case 0xf6:
        fputs("&ouml;", stdout);
        break;

    case 0xdc:
        fputs("&Uuml;", stdout);
        break;

    case 0xfc:
        fputs("&uuml;", stdout);
        break;

    case 0xdf:
        fputs("&szlig;", stdout);
        break;
#endif

    default:
        putchar(c);
    }
}


int main(int argc,
         char *argv[])
{
    int c;


    while ((c = getchar()) != EOF)
        output(c);

    return 0;
}
