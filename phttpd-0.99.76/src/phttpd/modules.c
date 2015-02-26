/*
** modules.c
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
#include <thread.h>
#include <synch.h>
#include <dlfcn.h>
#include <errno.h>
#include <sys/stat.h>

#include "phttpd.h"


static hashtable_t modlist_table;


void md_init(void)
{
    ht_init(&modlist_table, 0, NULL);
}


static void _md_unload(struct modinfo *mp)
{
    if (debug > 2)
        fprintf(stderr, "_md_unload(\"%s\")\n", mp->name);

    dlclose(mp->libp);
    mp->libp = NULL;

    mp->mtime = 0;
    mp->update = 0;
    mp->use = 0;

    mp->pm_init = NULL;
    mp->pm_exit = NULL;
    mp->pm_request = NULL;
    mp->pm_auth = NULL;
}


static int _md_load(struct modinfo *mp)
{
    int status;
    struct stat sb;
    const char *argv[2];


    if (debug > 2)
        fprintf(stderr, "_md_load(\"%s\")\n", mp->name);

    while ((status = s_stat(mp->name, &sb)) < 0 &&
            errno == EINTR)
        ;

    if (status < 0)
        return -1;

    mp->libp = dlopen(mp->name, RTLD_NOW);
    if (mp->libp == NULL)
        return -1;

    mp->mtime = sb.st_mtime;
    mp->update = 0;
    mp->use = 0;

    mp->pm_init = (int (*)(const char **))
                  dlsym(mp->libp, "pm_init");

    mp->pm_exit = (void (*)(void))
                  dlsym(mp->libp, "pm_exit");

    mp->pm_request = (int (*)(struct connectioninfo *cip))
                     dlsym(mp->libp, "pm_request");

    mp->pm_auth = (int (*)(struct connectioninfo *cip,
                           const char *,
                           const char *,
                           const char *,
                           const char *))
                  dlsym(mp->libp, "pm_auth");

    argv[0] = mp->name;
    argv[1] = NULL;

    if (mp->pm_init)
    {
        if (mp->pm_init(argv) < 0)
        {
            /* Module init failed, do something */
            if (debug > 2)
                fprintf(stderr, "%s:pm_init() failed\n", argv[0]);

            _md_unload(mp);
            return -2;
        }
    }

    return 0;
}


int md_release(void *key)
{
    if (debug > 2)
        fprintf(stderr, "md_release()\n");

    ht_release((hashentry_t *) key);

    return 0;
}



struct modinfo *md_load(const char *name, void **key)
{
    struct modinfo *mp;
    hashentry_t *hep;


    if (debug > 2)
        fprintf(stderr, "md_load(\"%s\")\n", name);


    hep = ht_lookup(&modlist_table, name, 0);
    if (hep)
    {
        mp = (struct modinfo *) hep->data;
        *key = (void *) hep;
        return mp;
    }


    mp = s_malloc(sizeof(struct modinfo));
    mp->name = s_strdup(name);

    if (_md_load(mp) < 0)
    {
        s_free(mp->name);
        s_free(mp);
        return NULL;
    }


    hep = ht_insert(&modlist_table, mp->name, 0, mp, HTF_REPLACE, NULL);

    *key = hep;
    return (struct modinfo *) hep->data;
}


struct ht_foreach_s
{
    int (*fcnp)(struct modinfo *mp, void *misc);
    void *misc;
};


static int ht_foreach_fun(hashentry_t *hep,
                          void *misc)
{
    struct ht_foreach_s *hfsp;

    hfsp = misc;

    return hfsp->fcnp(hep->data, hfsp->misc);
}


int md_foreach(int (*fcnp)(struct modinfo *mp, void *misc), void *misc)
{
    struct ht_foreach_s hfs;

    hfs.fcnp = fcnp;
    hfs.misc = misc;

    return ht_foreach(&modlist_table, ht_foreach_fun, &hfs);
}


const char *md_error(void)
{
    const char *msg;

    msg = dlerror();
    if (msg == NULL)
        msg = strerror(errno);

    return msg;
}
