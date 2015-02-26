/*
** urlcache.h
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

#ifndef PHTTPD_URLCACHE_H
#define PHTTPD_URLCACHE_H

#define UCF_HANDLER	0x0001
#define UCF_ACCESS	0x0004
#define UCF_AUTH	0x0008
#define UCF_REDIRECT	0x0010
#define UCF_PREDIRECT	0x0020

#define UCF_ALL         0x00FF

#define UCF_RELOAD	0x0100

typedef struct urlinfo_s
{
    mutex_t lock;
    unsigned int flags;
    char *handler;

    struct
    {
        char *url;
        char *request;
    } rewrite;

    struct
    {
        char *path;
    } access;

    struct
    {
        char *source;
        char *realm;
    } auth;

    struct
    {
        char *url;
        char *request;
    } redirect;
    struct
    {
        char *url;
        char *request;
    } predirect;
} urlinfo_t;

typedef struct ucentry_s
{
    cacheentry_t *cep;
    urlinfo_t *uip;
} ucentry_t;


extern void urlcache_init(void);

extern ucentry_t *urlcache_lookup(char *url,
                                  unsigned int flags);

extern void urlcache_release(ucentry_t *ucp);

extern int urlcache_getdata(ucentry_t *ucp,
                            unsigned int flags);

extern int urlcache_getstats(cachestat_t *csp);

extern int urlcache_refresh;
extern int urlcache_ttl;
extern int urlcache_gc_interval;
extern int urlcache_size;

#endif
