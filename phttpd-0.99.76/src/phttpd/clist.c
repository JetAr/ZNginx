/*
** clist.c
**
** Copyright (c) 1994 Peter Eriksson <pen@signum.se>
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

#include <thread.h>

#include "phttpd.h"



static rwlock_t cil_lock;
static int cil_size = 0;
static struct connectioninfo **cil = NULL;


void clist_init(void)
{
    rwlock_init(&cil_lock, USYNC_THREAD, NULL);
    cil_size = 0;
    cil = NULL;
}



int clist_active(void)
{
    int ac, i;

    rw_rdlock(&cil_lock);

    for (i = 0, ac = 0; i < cil_size; i++)
        if (cil[i])
            ac++;

    rw_unlock(&cil_lock);

    return ac;
}


int clist_foreach(int (*fcn)(struct connectioninfo *cip,
                             void *misc), void *misc)
{
    int i, val;

    rw_rdlock(&cil_lock);

    val = 0;
    for (i = 0; i < cil_size; i++)
        if (cil[i])
            if ((val = (*fcn)(cil[i], misc)))
                break;

    rw_unlock(&cil_lock);

    return val;
}


int clist_insert(struct connectioninfo *cip)
{
    int i;


    rw_wrlock(&cil_lock);

    for (i = 0; i < cil_size && cil[i] != NULL; i++)
        ;

    if (i == cil_size)
    {
        cil_size += 32;

        cil = s_realloc(cil, cil_size * sizeof(struct connectioninfo *));
    }

    cil[i] = cip;

    rw_unlock(&cil_lock);

    return i;
}


int clist_delete(struct connectioninfo *cip)
{
    int i;


    rw_wrlock(&cil_lock);

    for (i = 0; i < cil_size && cip != cil[i]; i++)
        ;

    if (i < cil_size)
        cil[i] = NULL;

    if (i == cil_size)
        i = -1;

    rw_unlock(&cil_lock);
    return i;
}


