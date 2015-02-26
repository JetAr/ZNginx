/*
** osf.c		Some functions missing in Digital Unix
**
** Copyright (c) 1995-1997 Peter Eriksson <pen@signum.se>
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


#ifdef __osf__

#include <netdb.h>
#include <pwd.h>
#include <grp.h>
#include <pthread.h>
#include <errno.h>
#include <sys/resource.h>


#undef sleep
void s_sleep(int timeout)
{
    sleep(timeout);
}

static char *get_mem(int len, char **buf, int *buflen)
{
    char *start;


    if (len > *buflen)
        return NULL;

    start = *buf;

    *buflen += len;
    *buf += len;

    return start;
}

static char *copy_mem(const char *src,
                      int len,
                      char **buf,
                      int *buflen)
{
    char *dst;

    if (src == NULL)
        return NULL;

    dst = get_mem(len, buf, buflen);
    if (dst == NULL)
        return NULL;
    memcpy(dst, src, len);
    return dst;
}

static char *copy_str(const char *src,
                      char **buf,
                      int *buflen)
{
    int len;


    if (src == NULL)
        return NULL;

    len = strlen(src)+1;

    return copy_mem(src, len, buf, buflen);
}


static int copy_hostent(struct hostent *dst,
                        const struct hostent *src,
                        char **buf,
                        int *buflen)
{
    int i, len;


    dst->h_name = copy_str(src->h_name, buf, buflen);
    if (dst->h_name == NULL && src->h_name != NULL)
    {
        errno = EINVAL;
        return -1;
    }

    if (src->h_aliases == NULL)
        dst->h_aliases = NULL;
    else
    {
        for (i = 0; src->h_aliases[i]; i++)
            ;
        len = i;
        dst->h_aliases = (char **) get_mem(sizeof(dst->h_aliases[0])*(len+1),
                                           buf, buflen);
        if (dst->h_aliases == NULL)
        {
            errno = EINVAL;
            return -1;
        }
        for (i = 0; i < len; i++)
        {
            dst->h_aliases[i] = copy_str(src->h_aliases[i], buf, buflen);
            if (dst->h_aliases[i] == NULL &&
                    src->h_aliases[i] != NULL)
            {
                errno = EINVAL;
                return -1;
            }
        }
        dst->h_aliases[i] = NULL;
    }

    dst->h_addrtype = src->h_addrtype;
    dst->h_length = src->h_length;

    if (src->h_addr_list == NULL)
        dst->h_addr_list = NULL;
    else
    {
        for (i = 0; src->h_addr_list[i]; i++)
            ;
        len = i;
        dst->h_addr_list = (char **) get_mem(sizeof(dst->h_addr_list[0])*
                                             (len+1),
                                             buf, buflen);

        if (dst->h_addr_list == NULL)
        {
            errno = EINVAL;
            return -1;
        }

        for (i = 0; i < len; i++)
        {
            dst->h_addr_list[i] = copy_mem(src->h_addr_list[i],
                                           src->h_length, buf, buflen);
            if (dst->h_addr_list[i] == NULL &&
                    src->h_addr_list[i] != NULL)
            {
                errno = EINVAL;
                return -1;
            }
        }
        dst->h_addr_list[i] = NULL;
    }

    return 0;
}

#undef gethostbyaddr
struct hostent *s_gethostbyaddr_r(const char *addr,
                                  int addrlen,
                                  int addrtype,
                                  struct hostent *result,
                                  char *buffer,
                                  int buflen,
                                  int *h_errnop)
{
    struct hostent *hp;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


    pthread_mutex_lock(&lock);
    hp = gethostbyaddr(addr, addrlen, addrtype);
    if (hp)
    {
        if (copy_hostent(result, hp, &buffer, &buflen) < 0)
            hp = NULL;
    }
    else
        *h_errnop = h_errno;

    pthread_mutex_unlock(&lock);
    return hp;
}

#undef gethostbyname
struct hostent *s_gethostbyname_r(const char *name,
                                  struct hostent *result,
                                  char *buffer,
                                  int buflen,
                                  int *h_errnop)
{
    struct hostent *hp;
    static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


    pthread_mutex_lock(&lock);
    hp = gethostbyname(name);
    if (hp)
    {
        if (copy_hostent(result, hp, &buffer, &buflen) < 0)
            hp = NULL;
    }
    else
        *h_errnop = h_errno;

    pthread_mutex_unlock(&lock);
    return hp;
}

#if 0
struct spwd *s_getspnam_r(const char *name,
                          struct spwd *res,
                          char *buf,
                          int buflen)
{
}
#endif

struct passwd *s_getpwnam_r(const char *name,
                            struct passwd *res,
                            char *buf,
                            int buflen)
{
    struct passwd *pwp;

    if (getpwnam_r(name, res, buf, buflen, &pwp) < 0)
        return NULL;
    else
        return pwp;
}

struct passwd *s_getpwuid_r(uid_t uid,
                            struct passwd *res,
                            char *buf,
                            int buflen)
{
    struct passwd *pwp;

    if (getpwuid_r(uid, res, buf, buflen, &pwp) < 0)
        return NULL;
    else
        return pwp;
}



struct group *s_getgrnam_r(const char *name,
                           struct group *res,
                           char *buf,
                           int buflen)
{
    struct group *grp;

    if (getgrnam_r(name, res, buf, buflen, &grp) < 0)
        return NULL;
    else
        return grp;
}


int s_setnice(int nice)
{
    return setpriority(PRIO_PROCESS, getpid(), nice);
}

#else

static int dummy;

#endif
