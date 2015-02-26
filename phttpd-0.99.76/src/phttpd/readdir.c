/*
** readdir.c
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
#include <errno.h>

#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <syslog.h>

#include "phttpd.h"

S_DIR *s_opendir(const char *path)
{
    S_DIR *cb;

#ifdef USE_GETDENTS
    int fd;

    fd = s_open(path, O_RDONLY);
    if (fd < 0)
        return NULL;

    cb = s_malloc(sizeof(S_DIR));
    cb->fd = fd;

    return cb;
#else
    DIR *dp;

    while ((dp = opendir(path)) == NULL && errno == EINTR)
        ;

    cb = s_malloc(sizeof(S_DIR));
    cb->dp = dp;

    return cb;
#endif
}

struct dirent *s_readdir(S_DIR *dp)
{
#ifdef USE_GETDENTS
    struct dirent *rdep;
    int len;


    if (dp->len == 0)
    {
        while ((dp->len = getdents(dp->fd,
                                   (struct dirent *) dp->buf,
                                   sizeof(dp->buf))) < 0 && errno == EINTR)
            ;
        dp->dep = (struct dirent *) dp->buf;
    }

    if (dp->len <= 0)
        return NULL;

    len = strlen(dp->dep->d_name);
    rdep = s_malloc(sizeof(struct dirent) + len);
    *rdep = *(dp->dep);

    s_strcpy(rdep->d_name, len+1, dp->dep->d_name);

    rdep->d_reclen = sizeof(struct dirent) + len;

    dp->len -= dp->dep->d_reclen;

    dp->dep = (struct dirent *) (((char *) dp->dep) + dp->dep->d_reclen);

    return rdep;
#else
    struct dirent *dep;

    while (readdir_r(dp->dp, &dp->deb, &dep) < 0 && errno == EINTR)
        ;

    return dep;
#endif
}

void s_closedir(S_DIR *dp)
{
#ifdef USE_GETDENTS
    s_close(dp->fd);
    s_free(dp);
#else
    while (closedir(dp->dp) < 0 && errno == EINTR)
        ;
    s_free(dp);
#endif
}
