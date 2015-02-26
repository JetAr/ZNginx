/*
** hashtable.h
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

#ifndef HASHTABLE_H
#define HASHTABLE_H


#define HTF_REPLACE	0x0001
#define HTF_NOINSERT	0x0002



typedef struct hashentry_s
{
    mutex_t lock;
    int use;
    void *key;
    unsigned int keylen;
    void *data;
    struct hashentry_s *next;
    void (*release_fcn)(void *data);
} hashentry_t;


typedef struct hashtable_s
{
    unsigned int size;
    unsigned int (*hash_fcn)(const void *key,
                             unsigned int keylen,
                             unsigned int size);

    hashentry_t *table;
} hashtable_t;



extern void ht_init(hashtable_t *htp,
                    unsigned hashsize,
                    unsigned (*hash_fcn)(const void *key,
                            unsigned int keylen,
                            unsigned int hashsize));

extern int ht_clean(hashtable_t *htp);
extern void ht_destroy(hashtable_t *htp);

extern hashentry_t *ht_lookup(hashtable_t *htp,
                              const void *key,
                              unsigned int keylen);

extern void ht_release(hashentry_t *hep);


extern hashentry_t *ht_insert(hashtable_t *htp,
                              const void *key,
                              unsigned int keylen,
                              void *data,
                              unsigned int flags,
                              void (*release_fcn)(void *data));

extern int ht_delete(hashtable_t *htp,
                     hashentry_t *hep);

extern int ht_foreach(hashtable_t *htp,
                      int (*foreach_fcn)(hashentry_t *hep,
                              void *misc),
                      void *misc);


#endif
