/*
** usercache.h
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

#ifndef PHTTPD_USERCACHE_H
#define PHTTPD_USERCACHE_H

#include <pwd.h>

struct userinfo
{
    struct passwd pwb;
    char buf[768];
};


extern void usercache_init(void);

extern void *usercache_lookup(char *username,
                              unsigned int flags,
                              struct passwd **pwp);

extern void usercache_release(void *key);

extern int usercache_getstats(cachestat_t *csp);

extern int usercache_refresh;
extern int usercache_ttl;
extern int usercache_gc_interval;
extern int usercache_size;

#endif



