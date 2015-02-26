/*
** fscache.c
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
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <limits.h>
#include <syslog.h>

#include "phttpd.h"

#ifndef MAP_FAILED
#define MAP_FAILED ((caddr_t) -1)
#endif

#ifndef MAP_NORESERVE
#define MAP_NORESERVE 0
#endif

int fscache_refresh = 30;
int fscache_ttl = 120;
int fscache_gc_interval = 120;
int fscache_size = 101;


static cache_t fscache;


fsinfo_t *fsinfo_alloc(char *url,
                       char *path,
                       uid_t uid,
                       gid_t gid,
                       struct stat *sp)
{
    fsinfo_t *fip;


    if (debug > 3)
        fprintf(stderr, "fsinfo_alloc(\"%s\", \"%s\")\n",
                url, path);

    fip = s_malloc(sizeof(fsinfo_t));

    fip->url = s_strdup(url);
    fip->path = s_strdup(path);
    fip->uid = uid;
    fip->gid = gid;
    if (sp)
        fip->sb = *sp;

    mutex_init(&fip->data_lock, USYNC_THREAD, NULL);

    return fip;
}



void fsinfo_free(void *data)
{
    fsinfo_t *fip = data;


    if (debug > 2)
        fprintf(stderr, "fsinfo_free(), url=%s, path=%s\n",
                fip->url, fip->path);

    if (S_ISDIR(fip->sb.st_mode))
    {
        struct fsc_dirent *fdp, *next;


        fdp = fip->data.dir.head;
        while (fdp)
        {
            next = fdp->next;

            if (fdp->fep)
                fscache_release(fdp->fep);

            s_free(fdp->dbp);
            s_free(fdp);

            fdp = next;
        }
    }
    else if (S_ISREG(fip->sb.st_mode))
    {
        if (fip->data.file.content)
            munmap(fip->data.file.content, fip->sb.st_size);
    }

    if (fip->url)
        s_free(fip->url);

    if (fip->path)
        s_free(fip->path);

    s_free(fip);
}



static int fsinfo_loaddir(fsinfo_t *fip)
{
    S_DIR *dp;
    struct dirent *dbp;
    struct fsc_dirent *fdp, **prev;
    char buf[2048], *ep, c;
    struct stat sb;


    if (debug > 2)
        fprintf(stderr, "fsinfo_loaddir(), url=%s, path=%s\n",
                fip->url, fip->path);

    s_strcpy(buf, sizeof(buf), fip->path);
    s_strcat(buf, sizeof(buf), "/.nodir");

    if (s_stat(buf, &sb) == 0)
    {
        if (debug > 2)
            fprintf(stderr, "fsinfo_loaddir(): denied ('.nodir' exists)\n");
        return -1;
    }

    dp = s_opendir(fip->path);
    if (dp == NULL)
    {
        if (debug > 2)
            fprintf(stderr, "fsinfo_loaddir(), failing (opendir)\n");
        return -1;
    }

    s_strcpy(buf, sizeof(buf), fip->url);

    c = 0;
    for (ep = buf; *ep; ep++)
        c = *ep;
    if (c != '/')
        *ep++ = '/';
    prev = &fip->data.dir.head;

    while ((dbp = s_readdir(dp)) != NULL)
    {
        if (debug > 2)
            fprintf(stderr, "fsinfo_loaddir(), entry=%s, reclen=%d\n",
                    dbp->d_name, dbp->d_reclen);

        if (strcmp(dbp->d_name, ".") == 0 ||
                strcmp(dbp->d_name, "..") == 0)
            continue;

        fip->data.dir.size++;

        fdp = s_malloc(sizeof(struct fsc_dirent));
        fdp->dbp = dbp;

        s_strcpy(ep, sizeof(buf)-(ep-buf), dbp->d_name);
        fdp->fep = fscache_lookup(buf, 0);

        *prev = fdp;
        prev = &fdp->next;
    }

    s_free(dbp);
    s_closedir(dp);

    fip->data_avail = 1;
    return 0;
}




static int fsinfo_loadfile(fsinfo_t *fip)
{
    int fd;


    fd = s_open(fip->path, O_RDONLY);
    if (fd < 0)
    {
        if (debug > 1)
            fprintf(stderr, "fsinfo_loadfile(): s_open(\"%s\") failed: %d\n",
                    fip->path, errno);
        return -1;
    }

    if (fstat(fd, &fip->sb) < 0)
    {
        if (debug > 1)
            fprintf(stderr, "fsinfo_loadfile(): fstat(\"%s\") failed: %d\n",
                    fip->path, errno);
        s_close(fd);
        return -1;
    }

    if (!S_ISREG(fip->sb.st_mode))
    {
        s_close(fd);
        if (debug > 1)
            fprintf(stderr, "fsinfo_loadfile(): not regular file: %s\n",
                    fip->path);
        return -1;
    }

    if (fip->sb.st_size > 0)
    {
        fip->data.file.content = mmap((caddr_t) NULL,
                                      fip->sb.st_size,
                                      PROT_READ,
                                      MAP_PRIVATE+MAP_NORESERVE,
                                      fd, 0);

        s_close(fd);

        if (fip->data.file.content == MAP_FAILED)
        {
            if (debug > 1)
                fprintf(stderr, "fsinfo_loadfile(): mmap(\"%s\") failed: %d\n",
                        fip->path, errno);
            fip->data.file.content = NULL;
            return -1;
        }

        madvise(fip->data.file.content,
                fip->sb.st_size,
                MADV_SEQUENTIAL);

        madvise(fip->data.file.content,
                fip->sb.st_size,
                MADV_WILLNEED);
    }
    else
        s_close(fd);

    fip->data_avail = 1;
    return 0;
}


static fsinfo_t *fsinfo_load(char *url,
                             char *path,
                             uid_t uid,
                             gid_t gid,
                             struct stat *sbp,
                             unsigned int flags)
{
    fsinfo_t *fip;


    if (debug > 2)
        fprintf(stderr, "fsinfo_load(), url=%s, path=%s, sbp=%08x\n",
                url, path, (unsigned int) sbp);

    fip = fsinfo_alloc(url, path, uid, gid, sbp);

    if (fip == NULL)
    {
        if (debug > 2)
            fprintf(stderr, "fip == NULL\n");
        return NULL;
    }

    if (sbp == NULL)
    {
        if (s_stat(path, &fip->sb) < 0)
        {
            fsinfo_free(fip);
            return NULL;
        }

        sbp = &fip->sb;
    }

    else
        fip->sb = *sbp;

    if (!(flags & FSCF_GETDATA))
        return fip;

    if (S_ISDIR(sbp->st_mode))
    {
        if (fsinfo_loaddir(fip) == -1)
        {
            fsinfo_free(fip);
            return NULL;
        }
    }
    else if (S_ISREG(sbp->st_mode))
        if (fsinfo_loadfile(fip) == -1)
        {
            fsinfo_free(fip);
            return NULL;
        }

    return fip;
}

static int equal_stat(struct stat *s1,
                      struct stat *s2)
{
    return (s1->st_size == s2->st_size &&
            s1->st_mtime == s2->st_mtime &&
            s1->st_ino == s2->st_ino);
}


static int fsinfo_update(void *key,
                         unsigned int keylen,
                         void *data,
                         void **new_data,
                         void *misc)
{
    uid_t uid;
    gid_t gid;
    char path[2048];
    struct stat sb;
    char *url;
    fsinfo_t *fip;
    unsigned int flags;


    flags = (misc ? *(unsigned int *)misc : 0);

    if (debug > 2)
        fprintf(stderr, "fsinfo_update(), key=%s, flags=%d\n",
                (char *) key,
                flags);

    url = key;
    fip = data;

    uid = gid = -1;


    if (fip == NULL)
    {
        /* First time request */

        if (debug > 2)
            fprintf(stderr, "fsinfo_update(): New cache entry\n");

        if (url_expand(url, path, sizeof(path), NULL, &uid, &gid) == NULL)
        {
            if (debug > 2)
                fprintf(stderr, "fsinfo_update(): Failed expanding\n");
            *new_data = NULL;
            return 1;
        }

        if (debug > 2)
            fprintf(stderr, "fsinfo_update(): Loading...\n");

        *new_data = fsinfo_load(url, path, uid, gid, NULL, flags);
        return 1;
    }


    if (s_stat(fip->path, &sb) < 0)
    {
        if (debug > 2)
            perror("fscache_update(): s_stat()");

        *new_data = NULL;
        return 1;
    }

    if (equal_stat(&sb, &fip->sb))
    {
        /* File hasn't changed since last fstat() */

        if (debug > 2)
            fprintf(stderr, "fscache_update(): File hasn't changed\n");
        return 0;
    }

    if (debug > 2)
        fprintf(stderr, "fsinfo_update(): Loading (with stat)...\n");

    *new_data = fsinfo_load(fip->url, fip->path, uid, gid, &sb, flags);
    return 1;
}




void fscache_init(void)
{
    cache_init(&fscache,
               fscache_refresh, fscache_ttl, fscache_gc_interval,
               fscache_size, NULL, fsinfo_free, fsinfo_update);
}



fscentry_t *fscache_lookup(char *url,
                           unsigned int flags)
{
    cacheentry_t *cep;
    fscentry_t *fep;


    if (debug > 2)
        fprintf(stderr, "fscache_lookup(\"%s\", %u)\n", url, flags);

    cep = cache_lookup(&fscache, url, 0, NULL,
                       ((flags & FSCF_RELOAD) ? CF_RELOAD : 0));
    if (cep == NULL)
        return NULL;

    fep = s_malloc(sizeof(fscentry_t));

    fep->cep = cep;
    fep->fip = cep->data;

    if (flags & FSCF_GETDATA)
        if (fscache_getdata(fep) != 1)
        {
            fscache_release(fep);
            return NULL;
        }

    return fep;
}



void fscache_release(fscentry_t *fep)
{
    if (fep == NULL)
        return;

    cache_release(fep->cep);
    s_free(fep);
}


int fscache_getdata(fscentry_t *fep)
{
    fsinfo_t *fip;


    if (fep == NULL)
        return -1;

    fip = fep->fip;

    if (fip == NULL)
        return -1;

    mutex_lock(&fip->data_lock);
    if (fip->data_avail == 1)
    {
        mutex_unlock(&fip->data_lock);
        return 1;
    }

    /* Load data */
    if (S_ISREG(fip->sb.st_mode))
        fsinfo_loadfile(fip);
    else if (S_ISDIR(fip->sb.st_mode))
        fsinfo_loaddir(fip);

    mutex_unlock(&fip->data_lock);
    return fip->data_avail == 1;
}


int fscache_getstats(cachestat_t *csp)
{
    return cache_getstats(&fscache, csp);
}
