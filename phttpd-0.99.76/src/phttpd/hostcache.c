/*
** hostcache.c
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

int hostcache_refresh = 120;
int hostcache_ttl = 120;
int hostcache_gc_interval = 600;
int hostcache_size = 101;

static cache_t hostname_cache;
static cache_t hostaddr_cache;


static void hostinfo_free(void *data)
{
    struct hostinfo *hip = data;

    if (debug > 2)
        fprintf(stderr, "hostinfo_free(), hostname=%s\n", hip->hb.h_name);

    s_free(data);
}


static int hostinfo_update_byname(void *key,
                                  unsigned int keylen,
                                  void *data,
                                  void **new_data,
                                  void *misc)
{
    struct hostinfo *hip;
    unsigned int flags;
    int h_errno;


    flags = (misc ? *(unsigned int *)misc : 0);

    if (debug > 2)
        fprintf(stderr, "hostinfo_update_byname(), name=%s, flags=%d\n",
                (char *) key, flags);

    hip = s_malloc(sizeof(struct hostinfo));

    if (s_gethostbyname_r((const char *) key, &hip->hb,
                          hip->buf, sizeof(hip->buf), &h_errno) == NULL)
    {
        s_free(hip);
        *new_data = NULL;
    }
    else
        *new_data = hip;

    return 1;
}


static int hostinfo_update_byaddr(void *key,
                                  unsigned int keylen,
                                  void *data,
                                  void **new_data,
                                  void *misc)
{
    struct hostinfo *hip;
    struct in_addr *iap;
    unsigned int flags;
    int h_errno;


    iap = key;
    flags = (misc ? *(unsigned int *)misc : 0);

    if (debug > 2)
        fprintf(stderr, "hostinfo_update_byaddr(), addr=%s, flags=%d\n",
                inet_ntoa(*iap), flags);

    hip = s_malloc(sizeof(struct hostinfo));

    if (s_gethostbyaddr_r((const char *) iap,
                          sizeof(*iap), AF_INET, &hip->hb,
                          hip->buf, sizeof(hip->buf), &h_errno) == NULL)
    {
        s_free(hip);
        *new_data = NULL;
    }
    else
        *new_data = hip;

    return 1;
}


void hostcache_init(void)
{
    cache_init(&hostname_cache,
               hostcache_refresh, hostcache_ttl, hostcache_gc_interval,
               hostcache_size, NULL,
               hostinfo_free, hostinfo_update_byname);

    cache_init(&hostaddr_cache,
               hostcache_refresh, hostcache_ttl, hostcache_gc_interval,
               hostcache_size, NULL,
               hostinfo_free, hostinfo_update_byaddr);
}


void hostcache_release(void *cache_key)
{
    if (debug > 2)
        fprintf(stderr, "hostcache_release(), p-key=%08x\n",
                (unsigned int) cache_key);

    cache_release(cache_key);
}

void *hostcache_lookup_byname(char *hostname,
                              unsigned int flags,
                              struct hostent **hep)
{
    cacheentry_t *cep;
    struct hostinfo *hip;


    if (debug > 2)
        fprintf(stderr, "hostcache_lookup_byname(), host=%s\n", hostname);

    cep = cache_lookup(&hostname_cache,
                       hostname, 0,
                       NULL, flags);

    hip = cep->data;
    *hep = &hip->hb;

    return cep;
}


void *hostcache_lookup_byaddr(void *addr,
                              unsigned int flags,
                              struct hostent **hep)
{
    cacheentry_t *cep;
    struct hostinfo *hip;


    if (debug > 2)
        fprintf(stderr, "hostcache_lookup_byaddr(), addr=%s\n",
                inet_ntoa(*(struct in_addr *) addr));

    cep = cache_lookup(&hostaddr_cache,
                       addr, sizeof(struct in_addr),
                       NULL, flags);

    hip = cep->data;
    *hep = &hip->hb;

    return cep;
}


int hostcache_getstats_byname(cachestat_t *csp)
{
    return cache_getstats(&hostname_cache, csp);
}

int hostcache_getstats_byaddr(cachestat_t *csp)
{
    return cache_getstats(&hostaddr_cache, csp);
}
