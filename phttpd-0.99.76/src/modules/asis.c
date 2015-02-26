/*
** asis.c
**
** Copyright (c) 1995 Marcus E. Hennecke <marcush@leland.stanford.edu>
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
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "phttpd.h"



int pm_init(const char **argv)
{
    if (debug > 1)
        fprintf(stderr, "*** asis/pm_init(\"%s\") called ***\n", argv[0]);

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** asis/pm_exit() called ***\n");
}


static int http_get_head(struct connectioninfo *cip)
{
    int result;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;
    fscentry_t *fep;
    fsinfo_t *fip;
    unsigned int fsc_flags = 0;
    char *data, *p, buf[256];
    time_t t;


    if (debug > 1)
        fprintf(stderr, "*** asis/pm_get() called ***\n");

    result = 200;

    if (hip && hip->mip && (hip->mip->pragma_flags & MPF_NOCACHE))
        fsc_flags = FSCF_RELOAD;

    fep = fscache_lookup(hip->url, FSCF_GETDATA+fsc_flags);
    if (fep == NULL)
        return -1;

    fip = fep->fip;
    if (fip == NULL)
    {
        fscache_release(fep);
        return -1;
    }

    data = fip->data.file.content;
    if (fip->sb.st_size >= 7 && !memcmp("HTTP/1.", data, 7))
    {
        result = atoi(strchr(data+7, ' '));

        if((p = strchr(data+7, '\n')) != NULL)
        {
            if(*(p-1) != '\r')
            {
                fd_write(fd, data, p-data);
                fd_putc('\n', fd);
            }
            else
                fd_write(fd, data, p-data+1);

            time(&t);
            fd_puts("Date: ", fd);
            fd_puts(http_time_r(&t, buf, sizeof(buf)), fd);
            fd_puts("Server: phttpd/", fd);
            fd_puts(server_version, fd);
            fd_putc('\n', fd);

            for(data = p+1; (p = strchr(data, '\n')) != NULL; data = p+1)
            {
                if(*data == '\n' || (*data == '\r' && *(data+1) == '\n'))
                {
                    fd_putc('\n', fd);
                    if(!strcasecmp(hip->method, "HEAD"))
                        return result;
                    data = p+1;
                    break;
                }
                if(*(p-1) != '\r')
                {
                    fd_write(fd, data, p-data);
                    fd_putc('\n', fd);
                }
                else
                    fd_write(fd, data, p-data+1);
            }
        }
    }

    hip->length = fip->sb.st_size - (data - (char *)fip->data.file.content);
    if (fd_write(fd, data, hip->length) < 0)
        if (debug)
        {
            fprintf(stderr, "ERROR on #%d: ", cip->request_no);
            perror("write");
        }

    fscache_release(fep);

    if (debug > 2)
        fprintf(stderr, "*** asis/pm_get: Returning\n");

    return result;
}



int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;

    if (strcasecmp(hip->method, "GET") == 0 ||
            strcasecmp(hip->method, "HEAD") == 0)
        return http_get_head(cip);
    else
        return -2;
}
