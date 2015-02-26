/*
** cache.c
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
#include <string.h>
#include <stdlib.h>
#include <synch.h>
#include <syslog.h>

#include "phttpd.h"


static unsigned int hash(void *key,
                         unsigned keylen,
                         unsigned int hashsize)
{
    unsigned char *keyp;
    unsigned int res = 0;
    unsigned int hs = 0;

    keyp = (unsigned char *) key;

    if (keylen == 0)
    {
        while (*keyp)
        {
            res += (*keyp++) << hs;
            hs = (hs+1)&7;
        }
    }
    else
    {
        while (keylen-- > 0)
        {
            res += (*keyp++) << hs;
            hs = (hs+1)&7;
        }
    }

    return res;
}



static int keycmp(void *p1,
                  void *p2,
                  unsigned int len)
{
    if (len == 0)
        return strcmp((char *) p1, (char *) p2);
    else
        return memcmp(p1, p2, len);
}



cacheentry_t *cacheentry_alloc(void *key,
                               unsigned int keylen,
                               void *data,
                               void (*release_fcn)(void *data))
{
    cacheentry_t *new;


    new = s_malloc(sizeof(cacheentry_t));

    new->magic = 0x1231246;

    mutex_init(&new->lock, USYNC_THREAD, NULL);
    new->use = 1;

    new->release_fcn = release_fcn;

    new->keylen = keylen;
    if (keylen == 0)
        new->key = s_strdup(key);
    else
    {
        new->key = s_malloc(keylen);
        memcpy(new->key, key, keylen);
    }
    new->data = data;

    return new;
}


static void *garb_thread(void *data)
{
    cache_t *cp = data;
    unsigned int i;
    cacheentry_t *prev, *current;
    time_t current_time;


Loop:
    s_sleep(cp->gc_interval);
    time(&current_time);

    if (debug > 1)
        fprintf(stderr, "cache:garb_thread(): Start\n");

    for (i = 0; i < cp->size; i++)
    {
        prev = &cp->table[i];

        mutex_lock(&prev->lock);

        while ((current = prev->next) != NULL)
        {
            mutex_lock(&current->lock);

            if (difftime(current_time, current->itime) > cp->ttl)
            {
                if (debug > 1)
                    fprintf(stderr, "garb_cache: Deleting, key=%s\n",
                            current->keylen ? "<binary>" : (char *) current->key);

                prev->next = current->next;
                current->next = NULL;
                mutex_unlock(&current->lock);

                cache_release(current);
            }
            else
            {
                mutex_unlock(&prev->lock);
                prev = current;
            }
        }

        mutex_unlock(&prev->lock);
    }

    if (debug > 1)
        fprintf(stderr, "cache:garb_thread(): Stop\n");

    goto Loop;
}



void cache_init(cache_t *cp,
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
                                  void *misc))
{
    unsigned int i;


    cp->size = hashsize ? hashsize : 101;
    cp->hash_fcn = hash_fcn ? hash_fcn : hash;
    cp->release_fcn = release_fcn;
    cp->update_fcn = update_fcn;

    cp->refresh = refresh;
    cp->ttl = ttl;
    cp->gc_interval = gc_interval;

    cp->table = s_malloc(cp->size * sizeof(cacheentry_t));

    for (i = 0; i < cp->size; i++)
        mutex_init(&cp->table[i].lock, USYNC_THREAD, NULL);

    cp->stats.lookups = 0;
    cp->stats.hits = 0;

    if (cp->gc_interval >= 0)
    {
        if (thr_create(NULL,
                       0,
                       (void *(*)(void *)) garb_thread,
                       (void *) cp,
                       THR_DETACHED+THR_DAEMON,
                       NULL))
        {
            syslog(LOG_ERR, "thr_create(garb_thread) failed: %m");
            exit(1);
        }
    }
}



cacheentry_t *cache_lookup(cache_t *cp,
                           void *key,
                           unsigned int keylen,
                           void *misc,
                           unsigned int flags)
{
    unsigned int i;
    cacheentry_t *prev, *current, *new;
    time_t current_time;
    void *new_data;


    if (debug > 2)
        if (keylen == 0)
            fprintf(stderr, "cache_lookup(), key=%s\n", (char *) key);
        else
            fprintf(stderr, "cache_lookup(), binary key\n");

    time(&current_time);

    mutex_lock(&cp->stats.lock);
    cp->stats.lookups++;
    mutex_unlock(&cp->stats.lock);

    i = cp->hash_fcn(key, keylen, cp->size) % cp->size;

    if (debug > 2)
        fprintf(stderr, "\thash index=%d, modulo=%d (for %s)\n",
                i, cp->size, keylen ? "<binary>" : (char *) key);

    prev = &cp->table[i];

    mutex_lock(&prev->lock);

    while ((current = prev->next) != NULL)
    {
        mutex_lock(&current->lock);
        if (keylen == current->keylen &&
                keycmp(current->key, key, keylen) == 0)
        {
            if ((flags & CF_RELOAD) == 0)
            {
                if (difftime(current_time, current->itime) <= cp->refresh)
                {
                    mutex_lock(&cp->stats.lock);
                    cp->stats.hits++;
                    mutex_unlock(&cp->stats.lock);

                    current->use++;
                    mutex_unlock(&current->lock);
                    mutex_unlock(&prev->lock);
                    return current;
                }
            }

            mutex_lock(&cp->stats.lock);
            cp->stats.updates++;
            mutex_unlock(&cp->stats.lock);

            break;
        }

        mutex_unlock(&prev->lock);
        prev = current;
    }

    if (debug > 2)
        fprintf(stderr, "cache_lookup(), updating\n");

    if (cp->update_fcn(key, keylen, current ? current->data : NULL,
                       &new_data, misc) == 0)
    {
        if (current)
        {
            current->use++;
            mutex_unlock(&current->lock);
            mutex_unlock(&prev->lock);
            return current;
        }
        else
            new_data = NULL;
    }

    mutex_lock(&cp->stats.lock);
    cp->stats.reloads++;
    mutex_unlock(&cp->stats.lock);

    new = cacheentry_alloc(key, keylen, new_data, cp->release_fcn);

    /* Data has changed, replace with new info */

    if (debug > 2)
        fprintf(stderr, "cache_lookup(), new info\n");

    time(&new->itime);
    new->use++;

    prev->next = new;
    if (current)
    {
        new->next = current->next;
        current->next = NULL;
        mutex_unlock(&current->lock);
    }

    mutex_unlock(&prev->lock);

    if (current)
        cache_release(current);

    return new;
}



void cache_release(cacheentry_t *cep)
{
    if (cep->magic != 0x1231246)
        s_abort();

    mutex_lock(&cep->lock);
    cep->use--;

    if (debug > 2)
        fprintf(stderr, "cache_release(), p-key=%08x, key=%s, use=%d\n",
                (unsigned int) cep,
                cep->keylen ? "<binary>" : (char *) cep->key, cep->use);

    if (cep->use > 0)
    {
        mutex_unlock(&cep->lock);
        return;
    }

    if (cep->release_fcn && cep->data)
        cep->release_fcn((void *) cep->data);

    cep->magic = 0x563431;

    s_free(cep->key);
    s_free(cep);
}


int cache_getstats(cache_t *ctp,
                   cachestat_t *csp)
{
    if (ctp == NULL)
        return -1;

    mutex_lock(&ctp->stats.lock);
    if (csp)
        *csp = ctp->stats;
    mutex_unlock(&ctp->stats.lock);

    return 1;
}
