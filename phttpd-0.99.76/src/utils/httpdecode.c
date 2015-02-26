#include <stdio.h>

static int hextoint(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';

    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;

    return -1;
}


static int get_hex(char *cp, int *val)
{
    int d1, d2;

    *val = 0;

    if ((d1 = hextoint(cp[0])) < 0)
        return 0;

    if ((d2 = hextoint(cp[1])) < 0)
        return 0;

    *val = (d1<<4)+d2;

    return 1;
}


static void output(char *cp)
{
    int val;


    while (*cp)
    {
        switch (*cp)
        {
        case '%':
            if (get_hex(cp+1, &val))
            {
                putchar(val);
                cp += 2;
            }
            else
                putchar('%');
            break;

        case '+':
            putchar(' ');
            break;

        default:
            putchar(*cp);
        }

        ++cp;
    }
}


int main(int argc,
         char *argv[])
{
    int i;


    for (i = 1; i < argc; i++)
    {
        output(argv[i]);
        putchar('\n');
    }

    return 0;
}
