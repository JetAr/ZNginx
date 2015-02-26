/*
** cache.h
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

#ifndef CACHETABLE_H
#define CACHETABLE_H



typedef struct cacheentry_s
{
    int magic;

    mutex_t lock;
    int use;
    time_t itime;

    void (*release_fcn)(void *data);

    void *key;
    unsigned int keylen;
    void *data;

    struct cacheentry_s *next;
} cacheentry_t;

typedef struct cachestat_s
{
    mutex_t lock;
    unsigned long lookups; /* Total lookups */
    unsigned long updates; /* Data was found in cache, but too old */
    unsigned long reloads; /* Data was loaded into cache */
    unsigned long hits;    /* Data was found in cache */
} cachestat_t;


typedef struct cache_s
{
    unsigned int size;
    unsigned int (*hash_fcn)(void *key,
                             unsigned int keylen,
                             unsigned int size);
    void (*release_fcn)(void *data);
    cachestat_t stats;
    int (*update_fcn)(void *key,
                      unsigned int keylen,
                      void *data,
                      void **new_data,
                      void *misc);
    int refresh;
    int ttl;
    int gc_interval;
    cacheentry_t *table;
} cache_t;



extern void cache_init(cache_t *ctp,
                       int refresh,
                       int ttl,
                       int gc_interval,
                       unsigned hashsize,
                       unsigned (*hash_fcn)(void *key,
                               unsigned int keylen,
                               unsigned int hashsize),
                       void (*release_fcn)(void *data),
                       int (*update_fcn)(void *key,
                               unsigned int keylen,
                               void *data,
                               void **new_data,
                               void *misc));

#define CF_RELOAD	0x0001

extern cacheentry_t *cache_lookup(cache_t *ctp,
                                  void *key,
                                  unsigned int keylen,
                                  void *misc,
                                  unsigned int flags);

extern void cache_release(cacheentry_t *hep);

extern int cache_getstats(cache_t *ctp,
                          cachestat_t *csp);

#endif
