/*
** table.h
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

#ifndef __TABLE_H__
#define __TABLE_H__

#include <synch.h>

struct table
{
    char *name;
    int size;
    int length;
    void **value;
    mutex_t lock;
};

extern struct table *tbl_create(int size_hint);
extern void tbl_free(struct table *tp,
                     void (*val_free)(void *val));
extern int tbl_insert(struct table *tp, int pos, void *value);
extern int tbl_delete(struct table *tp, int pos);

extern int tbl_prepend(struct table *tp, void *value);
extern int tbl_append(struct table *tp, void *value);

extern int tbl_foreach(struct table *tp,
                       int (*fcnp)(void *value, void *misc),
                       void *misc);

#endif
