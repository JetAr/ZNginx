/*
** modules.h
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

#ifndef PHTTPD_MODULES_H
#define PHTTPD_MODULES_H

#include <time.h>
#include <synch.h>

struct connectioninfo;


struct modinfo
{
    char *name;

    void *libp;
    time_t mtime;
    int update;
    int use;
    mutex_t lock;

    int (*pm_init)(const char **argv);

    void (*pm_exit)(void);

    int (*pm_request)(struct connectioninfo *cip);

    int (*pm_auth)(struct connectioninfo *cip,
                   const char *domain,
                   const char *type,
                   const char *user,
                   const char *password);
};

extern void md_init(void);

extern int md_release(void *key);
extern struct modinfo *md_load(const char *name, void **key);

extern int md_foreach(int (*fcnp)(struct modinfo *mp, void *misc), void *misc);
extern const char *md_error(void);


/* Module interfaces */
extern int pm_init(const char **argv);
extern void pm_exit(void);
extern int  pm_request(struct connectioninfo *cip);

#endif
