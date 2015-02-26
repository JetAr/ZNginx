/*
** mime.c
**
** Copyright (c) 1994-1995 Peter Eriksson <pen@signum.se>
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
#include <string.h>
#include <time.h>
#include <limits.h>
#include <stdlib.h>
#include <syslog.h>

#include "phttpd.h"

int mime_getline(char *buf, int size, int fd)
{
    int c, i;


    c = EOF;
    for (i = 0; i < size-1 && (c = fd_getc(fd)) != EOF; i++)
    {
        buf[i] = c;
        if (c == '\n')
        {
            if (i == 0)
            {
                buf[i] = '\0';
                return 0;
            }

            c = fd_getc(fd);
            if (c == EOF)
                break;

            if (c == '\n' || !s_isspace(c))
            {
                fd_ungetc(c, fd);
                break;
            }
        }
    }

    buf[i] = '\0';

    if (c == EOF)
        return -1;

    return i;
}



struct mimeinfo *mime_getheaders(int fd)
{
    struct mimeinfo *mip;
    hashentry_t *hep;
    char buf[2048];
    char *cp, *cp2;
    int i, status;


    NEW(mip);

    mip->version = 2;

    ht_init(&mip->table, 0, NULL);


    while ((status = mime_getline(buf, sizeof(buf), fd)) > 0)
    {
        i = strlen(buf) - 1;
        while (i >= 0 && s_isspace(buf[i]))
            i--;
        buf[i+1] = '\0';

        for (cp = buf; *cp && s_isspace(*cp); cp++)
            ;

        if (*cp == '\0')
            break;

        cp2 = cp;
        while (*cp2 && *cp2 != ':')
        {
            *cp2 = s_toupper(*cp2);
            ++cp2;
        }

        if (*cp2 == 0)
            continue;

        *cp2++ = '\0';
        while (*cp2 && s_isspace(*cp2))
            ++cp2;

        if (debug > 4)
            fprintf(stderr, "mime_getheaders(): got key=%s, value=%s\n",
                    cp, cp2);

        /*
        ** Some special handling of certain flags
        */
        if (strcmp(cp, "CONNECTION") == 0)
        {
            if (strcasecmp(cp2, "KEEP-ALIVE") == 0)
                mip->connection_flags |= MCF_KEEPALIVE;
        }
        else if (strcmp(cp, "PRAGMA") == 0)
        {
            if (strcasecmp(cp2, "NO-CACHE") == 0)
                mip->pragma_flags |= MPF_NOCACHE;
        }

        hep = ht_lookup(&mip->table, cp, 0);
        if (hep == NULL)
        {
            hep = ht_insert(&mip->table, cp, 0, s_strdup(cp2),
                            0, s_free);
        }
        else
        {
            char *new_str;
            int nlen;


            /* This code could be optimized :-) */
            nlen = (strlen(hep->data) + strlen(cp2) + 3);

            new_str = s_malloc(nlen);
            s_strcpy(new_str, nlen, hep->data);
            s_strcat(new_str, nlen, "\n");
            s_strcat(new_str, nlen, cp2);

            s_free(hep->data);
            hep->data = new_str;
        }

        ht_release(hep);
    }

    return mip;
}

void mime_freeheaders(struct mimeinfo *mip)
{
    if (debug > 3)
        fprintf(stderr, "mime_freeheaders(): Start\n");

    ht_destroy(&mip->table);
    s_free(mip);

    if (debug > 3)
        fprintf(stderr, "mime_freeheaders(): Stop\n");
}



static int mwr_header(hashentry_t *hep, void *misc)
{
    int fd = * (int *) misc;
    char *start, *end;


    if (debug > 4)
        fprintf(stderr, "mwr_header: key=\"%s\", value=\"%s\"\n",
                (char *) hep->key, (char *) hep->data);

    start = hep->data;
    while ((end = strchr(start, '\n')) != NULL)
    {
        if (end == start)
        {
            /* Ignore empty lines */
            start = end+1;
            continue;
        }

        fd_puts(hep->key, fd);
        fd_puts(": ", fd);
        fd_write(fd, start, end-start+1);
        start = end+1;
    }

    if (*start)
    {
        fd_puts(hep->key, fd);
        fd_puts(": ", fd);
        fd_puts(start, fd);
        fd_putc('\n', fd);
    }

    return 0;
}


void mime_writeheaders(int fd, struct mimeinfo *mip)
{
    if (mip == NULL)
        return;

    ht_foreach(&mip->table, mwr_header, &fd);

    return;
}



void *mime_getheader(struct mimeinfo *mip,
                     const char *header,
                     int str_flag)
{
    hashentry_t *hep;
    char *str;


    if (debug > 3)
        fprintf(stderr, "mime_getheader(\"%s\")\n", header);

    if (mip == NULL)
        return NULL;

    hep = ht_lookup(&mip->table, header, 0);
    if (hep == NULL)
        return NULL;

    if (debug > 4)
        fprintf(stderr, "\tuse=%d\n", hep->use);

    str = hep->data;
    ht_release(hep);

    return str;
}
