/*
** hostcache.h
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

#ifndef PHTTPD_HOSTCACHE_H
#define PHTTPD_HOSTCACHE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

struct hostinfo
{
    struct hostent hb;
    char buf[768];
};


extern void hostcache_init(void);

extern void *hostcache_lookup_byname(char *hostname,
                                     unsigned int flags,
                                     struct hostent **hep);

extern void *hostcache_lookup_byaddr(void *addr,
                                     unsigned int flags,
                                     struct hostent **hep);

extern void hostcache_release(void *key);

extern int hostcache_getstats_byname(cachestat_t *csp);
extern int hostcache_getstats_byaddr(cachestat_t *csp);

extern int hostcache_refresh;
extern int hostcache_ttl;
extern int hostcache_gc_interval;
extern int hostcache_size;

#endif



