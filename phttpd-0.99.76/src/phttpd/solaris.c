/*
** solaris.c				SunOS 5/Solaris 2 specific code
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

#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <synch.h>
#include <sys/mman.h>
#include <netdb.h>

#include <sys/types.h>

#include <synch.h>

#include "phttpd.h"


#undef sleep
void s_sleep(int timeout)
{
#if 1
    mutex_t mp;
    cond_t  cv;
    time_t bt;
    timestruc_t ts;


    time(&bt);
    bt += timeout;
    ts.tv_sec = bt;
    ts.tv_nsec = 0;


    mutex_init(&mp, USYNC_THREAD, NULL);
    mutex_lock(&mp);
    while (cond_timedwait(&cv, &mp, &ts) < 0 && errno == EINTR)
        ;
#else
    /* There is a bug in Solaris 2.4's sleep() which causes
       it to return random garbage if interrupted by some
       signals - so we handle it by calling time() ourself */

    time_t start, now;
    int len;


    time(&start);
    len = timeout;

    while (len > 0)
    {
        sleep(len);
        time(&now);

        len = timeout - (now - start);
    }
#endif
}



/* Workaround to fix problems with getXXXnam_r() not being
   quite as MT-SAFE as the manual claims (problems in the NSS code) */

static mutex_t nss_lock;

struct hostent *s_gethostbyaddr_r(const char *addr,
                                  int length,
                                  int type,
                                  struct hostent *result,
                                  char *buffer,
                                  int buflen,
                                  int *h_errnop)
{
    struct hostent *hep;

    mutex_lock(&nss_lock);
    hep = gethostbyaddr_r(addr,
                          length,
                          type,
                          result,
                          buffer,
                          buflen,
                          h_errnop);

    mutex_unlock(&nss_lock);
    return hep;
}


struct hostent *s_gethostbyname_r(const char *name,
                                  struct hostent *result,
                                  char *buffer,
                                  int buflen,
                                  int *h_errnop)
{
    struct hostent *hep;


    mutex_lock(&nss_lock);
    hep = gethostbyname_r(name, result, buffer, buflen, h_errnop);
    mutex_unlock(&nss_lock);

    return hep;
}



struct spwd *s_getspnam_r(const char *name,
                          struct spwd *res,
                          char *buf,
                          int buflen)
{
    struct spwd *retval;


    mutex_lock(&nss_lock);

    retval = getspnam_r(name, res, buf, buflen);

    mutex_unlock(&nss_lock);

    return retval;
}


struct passwd *s_getpwnam_r(const char *name,
                            struct passwd *res,
                            char *buf,
                            int buflen)
{
    struct passwd *retval;


    mutex_lock(&nss_lock);
    retval = getpwnam_r(name, res, buf, buflen);
    mutex_unlock(&nss_lock);

    return retval;
}


struct passwd *s_getpwuid_r(uid_t uid,
                            struct passwd *res,
                            char *buf,
                            int buflen)
{
    struct passwd *retval;


    mutex_lock(&nss_lock);
    retval = getpwuid_r(uid, res, buf, buflen);
    mutex_unlock(&nss_lock);

    return retval;
}



struct group *s_getgrnam_r(const char *name,
                           struct group *res,
                           char *buf,
                           int buflen)
{
    struct group *retval;


    mutex_lock(&nss_lock);
    retval = getgrnam_r(name, res, buf, buflen);
    mutex_unlock(&nss_lock);

    return retval;
}


int s_nice(int niceval)
{
    return nice(niceval);
}
