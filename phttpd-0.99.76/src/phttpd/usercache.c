/*
** usercache.c
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

#include "phttpd.h"


int usercache_refresh = 120;
int usercache_ttl = 120;
int usercache_gc_interval = 600;
int usercache_size = 101;

static cache_t username_cache;


static void userinfo_free(void *data)
{
    struct userinfo *uip = data;


    if (debug > 2)
        fprintf(stderr, "userinfo_free(), user=%s\n", uip->pwb.pw_name);

    s_free(data);
}


static int userinfo_update(void *key,
                           unsigned int keylen,
                           void *data,
                           void **new_data,
                           void *misc)
{
    struct userinfo *uip;
    unsigned int flags;


    flags = (misc ? *(unsigned int *)misc : 0);

    if (debug > 2)
        fprintf(stderr, "userinfo_update(), user=%s, flags=%d\n",
                (char *) key, flags);

    uip = s_malloc(sizeof(struct userinfo));

    if (s_getpwnam_r((const char *) key,
                     &uip->pwb,
                     uip->buf, sizeof(uip->buf)) == NULL)
    {
        s_free(uip);
        *new_data = NULL;
    }
    else
        *new_data = uip;

    return 1;
}



void usercache_init(void)
{
    cache_init(&username_cache,
               usercache_refresh, usercache_ttl, usercache_gc_interval,
               usercache_size, NULL, userinfo_free, userinfo_update);
}


void usercache_release(void *cache_key)
{
    if (debug > 2)
        fprintf(stderr, "usercache_release(), p-key=%08x\n",
                (unsigned int) cache_key);

    cache_release(cache_key);
}


void *usercache_lookup(char *username,
                       unsigned int flags,
                       struct passwd **pwp)
{
    cacheentry_t *cep;
    struct userinfo *uip;


    if (debug > 2)
        fprintf(stderr, "usercache_lookup(), user=%s\n", username);

    cep = cache_lookup(&username_cache,
                       username, 0,
                       NULL, flags);

    uip = cep->data;
    *pwp = &uip->pwb;

    return cep;
}



int usercache_getstats(cachestat_t *csp)
{
    return cache_getstats(&username_cache, csp);
}
