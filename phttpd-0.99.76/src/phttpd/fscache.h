/*
** fscache.h
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

#ifndef PHTTPD_FSCACHE_H
#define PHTTPD_FSCACHE_H

#include <sys/stat.h>
#include <dirent.h>

typedef struct fsinfo_s
{
    char *url;
    char *path;
    struct stat sb;

    uid_t uid;
    gid_t gid;


    mutex_t data_lock;
    unsigned int data_avail;

    union
    {
        struct
        {
            char *content;
        } file;
        struct
        {
            unsigned int size;
            struct fsc_dirent
            {
                struct dirent *dbp;
                struct fscentry_s *fep;
                struct fsc_dirent *next;
            } *head;
        } dir;
    } data;
} fsinfo_t;


typedef struct fscentry_s
{
    cacheentry_t *cep;
    fsinfo_t *fip;
} fscentry_t;


#define FSCF_GETDATA    0x0001
#define FSCF_RELOAD     0x0002

extern void fscache_init(void);

extern fscentry_t *fscache_lookup(char *url,
                                  unsigned int flags);

extern void fscache_release(fscentry_t *fep);

extern int fscache_getdata(fscentry_t *fep);

extern int fscache_getstats(cachestat_t *csp);

extern int fscache_refresh;
extern int fscache_ttl;
extern int fscache_gc_interval;
extern int fscache_size;

#endif
