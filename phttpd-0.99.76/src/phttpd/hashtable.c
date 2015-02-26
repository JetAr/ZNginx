/*
** hashtable.c
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

#include "phttpd.h"


static unsigned int hash(const void *key,
                         unsigned keylen,
                         unsigned int hashsize)
{
    const unsigned char *keyp;
    unsigned int res = 0;
    unsigned int hs = 0;

    keyp = (const unsigned char *) key;

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



static int keycmp(const void *p1,
                  unsigned int len1,
                  const void *p2,
                  unsigned int len2)
{
    if (len1 == 0 && len2 == 0)
        return strcmp((char *) p1, (char *) p2);
    else if (len1 == 0 && len2 != 0)
        return strncmp(p1, p2, len2);
    else if (len1 == len2)
        return memcmp(p1, p2, len2);
    else
        return -1;
}



void ht_init(hashtable_t *htp,
             unsigned hashsize,
             unsigned (*hash_fcn)(const void *key,
                                  unsigned int keylen,
                                  unsigned int hashsize))
{
    unsigned int i;


    htp->size = hashsize ? hashsize : 101;
    htp->hash_fcn = hash_fcn ? hash_fcn : hash;

    htp->table = s_malloc(htp->size * sizeof(hashentry_t));

    for (i = 0; i < htp->size; i++)
        mutex_init(&htp->table[i].lock, USYNC_THREAD, NULL);
}




hashentry_t *ht_lookup(hashtable_t *htp,
                       const void *key,
                       unsigned keylen)
{
    unsigned int i;
    hashentry_t *prev, *current;


    if (htp == NULL)
        return NULL;

    i = htp->hash_fcn(key, keylen, htp->size) % htp->size;

    prev = &htp->table[i];
    mutex_lock(&prev->lock);

    while ((current = prev->next) != NULL)
    {
        mutex_unlock(&prev->lock);

        mutex_lock(&current->lock);
        current->use++;
        mutex_unlock(&current->lock);

        if (keycmp(current->key, current->keylen, key, keylen) == 0)
            return current;

        mutex_lock(&current->lock);
        current->use--;

        prev = current;
    }

    mutex_unlock(&prev->lock);
    return NULL;
}



void ht_release(hashentry_t *hep)
{
    if (hep == NULL)
        return;

    mutex_lock(&hep->lock);
    hep->use--;

    if (debug > 4)
        fprintf(stderr, "ht_release(), use=%d\n", hep->use);

    if (hep->use > 0)
    {
        mutex_unlock(&hep->lock);
        return;
    }



    if (hep->release_fcn)
        hep->release_fcn(hep->data);

    s_free(hep->key);
    s_free(hep);
}



hashentry_t *ht_insert(hashtable_t *htp,
                       const void *key,
                       unsigned int keylen,
                       void *data,
                       unsigned int flags,
                       void (*release_fcn)(void *data))
{
    unsigned int i;
    hashentry_t *prev, *current, *new;


    if (htp == NULL)
        return NULL;

    i = htp->hash_fcn(key, keylen, htp->size) % htp->size;


    new = s_malloc(sizeof(hashentry_t));
    mutex_init(&new->lock, USYNC_THREAD, NULL);
    if (keylen)
    {
        new->key = s_malloc(keylen);
        memcpy(new->key, key, keylen);
    }
    else
        new->key = s_strdup(key);

    new->data = data;
    new->use = 1;
    new->release_fcn = release_fcn;

    prev = &htp->table[i];
    mutex_lock(&prev->lock);

    while ((current = prev->next) != NULL)
    {
        mutex_lock(&current->lock);
        if (keycmp(current->key, current->keylen, key, keylen) == 0)
        {
            /* Already in table */

            if (flags & HTF_REPLACE)
            {
                if (debug > 2)
                    fprintf(stderr, "ht_insert(): Replacing %s\n", (char *) key);

                prev->next = new;
                new->next = current->next;
                new->use++;

                mutex_unlock(&current->lock);
                mutex_unlock(&prev->lock);

                ht_release(current);

                return new;
            }
            else
            {
                mutex_unlock(&current->lock);
                mutex_unlock(&prev->lock);

                ht_release(new);

                return NULL;
            }
        }

        mutex_unlock(&prev->lock);
        prev = current;
    }

    if (flags & HTF_NOINSERT)
    {
        /* Only replace already existing data */

        mutex_unlock(&prev->lock);

        ht_release(new);

        return NULL;
    }

    new->use++;

    prev->next = new;
    mutex_unlock(&prev->lock);

    return new;
}




int ht_delete(hashtable_t *htp,
              hashentry_t *hep)
{
    unsigned int i;
    hashentry_t *prev, *current;


    if (htp == NULL)
        return -1;

    i = htp->hash_fcn(hep->key, hep->keylen, htp->size) % htp->size;

    prev = &htp->table[i];
    mutex_lock(&prev->lock);

    while ((current = prev->next) != NULL)
    {
        mutex_lock(&current->lock);
        if (current == hep)
        {
            prev->next = current->next;
            current->next = NULL;

            mutex_unlock(&current->lock);
            mutex_unlock(&prev->lock);

            ht_release(current);
            return 0;
        }

        mutex_unlock(&prev->lock);
        prev = current;
    }

    mutex_unlock(&prev->lock);
    return -1;
}



int ht_foreach(hashtable_t *htp,
               int (*foreach_fcn)(hashentry_t *hep, void *misc),
               void *misc)
{
    int retval;
    unsigned int i;
    hashentry_t *prev, *current;


    if (htp == NULL)
        return -1;

    for (i = 0; i < htp->size; i++)
    {
        prev = &htp->table[i];

        mutex_lock(&prev->lock);
        while ((current = prev->next) != NULL)
        {
            mutex_unlock(&prev->lock);

            mutex_lock(&current->lock);
            current->use++;
            mutex_unlock(&current->lock);

            retval = foreach_fcn(current, misc);

            mutex_lock(&current->lock);
            current->use--;

            if (retval < 0)
            {
                mutex_unlock(&current->lock);
                return retval;
            }

            prev = current;
        }

        mutex_unlock(&prev->lock);
    }

    return 0;
}


void ht_destroy(hashtable_t *htp)
{
    ht_clean(htp);
    s_free(htp->table);
}


int ht_clean(hashtable_t *htp)
{
    unsigned int i;
    hashentry_t *prev, *current;


    if (htp == NULL)
        return -1;

    for (i = 0; i < htp->size; i++)
    {
        prev = &htp->table[i];

        mutex_lock(&prev->lock);
        while ((current = prev->next) != NULL)
        {
            mutex_lock(&current->lock);

            prev->next = current->next;
            current->next = NULL;

            mutex_unlock(&current->lock);

            ht_release(current);
        }

        mutex_unlock(&prev->lock);
    }

    return 0;
}



#ifdef TESTING
int debug = 0;

int print_entry(hashentry_t *hep, void *misc)
{
    if (hep->data)
        printf("key=%s, value=%s (use=%d)\n", hep->key, hep->data, hep->use);
    else
        printf("key=%s, no value (use=%d)\n", hep->key, hep->use);

    return 0;
}


main(int argc, char *argv[])
{
    hashtable_t ht;
    hashentry_t *hep;
    int i;
    char *val, *cp;


    ht_init(&ht, 17, NULL);

    i = 1;

    puts("Inserting:");

    /* Insert entries into table */
    for (; i < argc && strcmp(argv[i], "-") != 0; i++)
    {
        printf("\t%s:\n", argv[i]);

        val = s_strdup(argv[i]);
        cp = strchr(val, ':');
        if (cp == NULL)
        {
            hep = ht_insert(&ht, val, 0, NULL, 0, NULL);

            if (hep == NULL)
                printf("\t\tFailed\n");
            else
            {
                ht_release(hep);
                printf("\t\tOK\n");
            }
        }
        else
        {
            *cp++ = '\0';
            hep = ht_insert(&ht, val, 0, cp, 0, NULL);
            if (hep == NULL)
                printf("\t\tFailed\n");
            else
            {
                ht_release(hep);
                printf("\t\tOK\n");
            }
        }
    }


    puts("Table dump:");
    ht_foreach(&ht, print_entry, NULL);

    ++i;

    puts("Deleting:");

    /* Delete entries from table */
    for (; i < argc && strcmp(argv[i], "-") != 0; i++)
    {
        printf("\t%s:\n", argv[i]);

        hep = ht_lookup(&ht, argv[i], 0);
        if (hep)
        {
            printf("\t\t(entry found)\n");

            if (ht_delete(&ht, hep) != 0)
                printf("\t\tDelete failed\n");
            else
                printf("\t\tDelete OK\n");

            ht_release(hep);
        }
        else
            printf("\t\tEntry not found\n");
    }


    puts("Table dump:");
    ht_foreach(&ht, print_entry, NULL);

    puts("Looking up:");

    ++i;
    /* Lookup entries */
    for (; i < argc && strcmp(argv[i], "-") != 0; i++)
    {
        printf("\t%s:\n", argv[i]);
        hep = ht_lookup(&ht, argv[i], 0);
        if (hep)
        {
            if (hep->data)
                printf("\t\tfound (value=%s)\n", hep->data);
            else
                printf("\t\tfound (no value)\n");

            ht_release(hep);
        }
        else
            printf("\t\tnot found (key=%s)\n", argv[i]);
    }

    puts("Table dump:");
    ht_foreach(&ht, print_entry, NULL);

    exit(0);
}
#endif
