/*
** clist.h
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

#ifndef __CLIST_H__
#define __CLIST_H__

struct connectioninfo;

extern void clist_init(void);

extern int clist_active(void);

extern int clist_foreach(int (*fcn)(struct connectioninfo *cip,
                                    void *misc), void *misc);

extern int clist_insert(struct connectioninfo *cip);
extern int clist_delete(struct connectioninfo *cip);

#endif
