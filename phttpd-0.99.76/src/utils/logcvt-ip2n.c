/*
** logcvt-ip2n.c
**
** This program will try to look up all IP addresses in a log file and
** convert them into FQDNs (fully qualified domain names).
**
** Copyright (c) 1995 Peter Eriksson <pen@signum.se>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include <stdio.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc,
         char *argv[])
{
    FILE *fp_in, *fp_out;
    char buf[8192];
    char *cp;
    long ia;
    struct hostent *hp;


    if (argc > 1)
        fp_in = fopen(argv[1], "r");
    else
        fp_in = stdin;

    if (argc > 2)
        fp_out = fopen(argv[2], "w");
    else
        fp_out = stdout;


    while (fgets(buf, sizeof(buf), fp_in))
    {
        cp = buf;
        while (*cp && !isspace(*cp))
            ++cp;

        if (!*cp)
            continue;

        *cp++ = '\0';
        while (*cp && isspace(*cp))
            ++cp;

        ia = inet_addr(buf);
        hp = gethostbyaddr((const char *) &ia, sizeof(ia), AF_INET);

        if (hp == NULL)
            fprintf(fp_out, "%s %s", buf, cp);
        else
            fprintf(fp_out, "%s %s", hp->h_name, cp);
    }

    fclose(fp_out);
    fclose(fp_in);

    return 0;
}
