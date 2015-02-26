/*
** table.c
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

#include <stdlib.h>
#include <string.h>
#include <synch.h>

#include "phttpd.h"



struct table *tbl_create(int size_hint)
{
    struct table *tp;


    if (size_hint <= 0)
        size_hint = 32;

    tp = s_malloc(sizeof(struct table));
    tp->value = s_malloc((tp->size = size_hint)*sizeof(void *));
    tp->length = 0;

    mutex_init(&tp->lock, USYNC_THREAD, NULL);

    return tp;
}


void tbl_free(struct table *tp,
              void (*val_free)(void *val))
{
    int i;


    for (i = 0; i < tp->length; i++)
        (*val_free)(tp->value[i]);

    mutex_destroy(&tp->lock);

    s_free(tp->value);
    s_free(tp);
}


int tbl_insert(struct table *tp, int pos, void *value)
{
    int i;


    mutex_lock(&tp->lock);

    if (tp->length == tp->size)
    {
        int new_size;


        if (pos >= tp->size)
            new_size = pos + 32;
        else
            new_size = tp->size + 32;

        tp->value = s_realloc(tp->value, new_size*sizeof(void *));
        tp->size  = new_size;
    }

    for (i = tp->length; i > pos; i--)
        tp->value[i] = tp->value[i-1];

    tp->value[pos] = value;

    mutex_unlock(&tp->lock);

    return ++tp->length;
}


int tbl_delete(struct table *tp, int pos)
{
    int i;


    mutex_lock(&tp->lock);

    for (i = pos; i < tp->length - 1; i++)
        tp->value[i] = tp->value[i+1];

    tp->value[i] = NULL;

    mutex_unlock(&tp->lock);

    return --tp->length;
}



int tbl_prepend(struct table *tp, void *value)
{
    return tbl_insert(tp, 0, value);
}



int tbl_append(struct table *tp, void *value)
{
    return tbl_insert(tp, tp->length, value);
}


int tbl_foreach(struct table *tp,
                int (*fcnp)(void *value, void *misc),
                void *misc)
{
    int i, status;


    mutex_lock(&tp->lock);

    status = 0;
    for (i = 0; i < tp->length; i++)
        if ((status = (*fcnp)(tp->value[i], misc)) != 0)
            break;

    mutex_unlock(&tp->lock);

    return status;
}

