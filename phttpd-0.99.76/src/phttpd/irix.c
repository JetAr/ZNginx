/*
** irix.c		Some functions missing in SGI Irix.
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


#if defined(__sgi)


#include <netdb.h>
#include <pwd.h>
#include <shadow.h>
#include <thread.h>

static char *copy_str(char *str, char **buf, int *size)
{
    char *start;


    if (str == NULL)
        return NULL;

    start = *buf;

    while (*size > 1 && *str)
    {
        **buf = *str++;
        ++*buf;
        --*size;
    }

    if (*size > 1)
    {
        **buf = '\0';
        ++*buf;
        --*size;
    }
    else
        return NULL;

    return start;
}


static char **copy_strarr(char **strarr, char **buf, int *size)
{
    char **start;
    int arrsize, i, len;


    if (strarr == NULL)
        return NULL;

    for (arrsize = 0; strarr[arrsize]; arrsize++)
        ;

    len = arrsize * sizeof(char *);

    /* Make sure the pointers are aligned on a nice 4-byte boundary */
    while ( (((unsigned long) (*buf)) & 3) && (*size > 1) )
    {
        ++*buf;
        --*size;
    }

    if (*size < len)
        return NULL;

    start = (char **) *buf;
    *size -= len;
    *buf += len;

    for (i = 0; i < arrsize; i++)
        start[i] = copy_str(strarr[i], buf, size);

    return start;
}


static mutex_t gethost_lock;

struct hostent *gethostbyname_r(const char *name,
                                struct hostent *res,
                                char *buf,
                                int bufsize,
                                int *h_errnop)
{
    struct hostent *hp;


    mutex_lock(&gethost_lock);
    hp = gethostbyname(name);
    if (h_errnop)
        *h_errnop = h_errno;
    if (hp == NULL)
        goto End;

    res->h_name = copy_str(hp->h_name, &buf, &bufsize);
    res->h_aliases = copy_strarr(hp->h_aliases, &buf, &bufsize);
    res->h_addrtype = hp->h_addrtype;
    res->h_length = hp->h_length;
    res->h_addr_list = copy_strarr(hp->h_addr_list, &buf, &bufsize);

    hp = res;

End:
    mutex_unlock(&gethost_lock);

    return hp;
}


struct hostent *gethostbyaddr_r(const char *addr,
                                int length, int type,
                                struct hostent *res,
                                char *buf,
                                int bufsize,
                                int *h_errnop)
{
    struct hostent *hp;


    mutex_lock(&gethost_lock);
    hp = gethostbyaddr(addr, length, type);
    if (h_errnop)
        *h_errnop = h_errno;
    if (hp == NULL)
        goto End;

    res->h_name = copy_str(hp->h_name, &buf, &bufsize);
    res->h_aliases = copy_strarr(hp->h_aliases, &buf, &bufsize);
    res->h_addrtype = hp->h_addrtype;
    res->h_length = hp->h_length;
    res->h_addr_list = copy_strarr(hp->h_addr_list, &buf, &bufsize);

    hp = res;

End:
    mutex_unlock(&gethost_lock);

    return hp;
}


static mutex_t getserv_lock;

struct servent *getservbyname_r(const char *name,
                                const char *proto,
                                struct servent *res,
                                char *buf,
                                int bufsize)
{
    struct servent *sp;


    mutex_lock(&getserv_lock);
    sp = getservbyname(name, proto);
    if (sp == NULL)
        goto End;

    res->s_name = copy_str(sp->s_name, &buf, &bufsize);
    res->s_aliases = copy_strarr(sp->s_aliases, &buf, &bufsize);
    res->s_port = sp->s_port;
    res->s_proto = copy_str(sp->s_proto, &buf, &bufsize);

    sp = res;

End:
    mutex_unlock(&getserv_lock);

    return sp;
}

#ifndef _POSIX1C

/*
 *  For Irix 6.2 this is already defined
 *
 */

static mutex_t getpw_lock;

struct passwd *getpwnam_r(const char *name,
                          struct passwd *res,
                          char *buf,
                          int bufsize)
{
    struct passwd *pwp;


    mutex_lock(&getpw_lock);
    pwp = getpwnam(name);
    if (pwp == NULL)
        goto End;

    res->pw_name = copy_str(pwp->pw_name, &buf, &bufsize);
    res->pw_passwd = copy_str(pwp->pw_passwd, &buf, &bufsize);
    res->pw_uid = pwp->pw_uid;
    res->pw_gid = pwp->pw_gid;
    res->pw_age = copy_str(pwp->pw_age, &buf, &bufsize);
    res->pw_comment = copy_str(pwp->pw_comment, &buf, &bufsize);
    res->pw_gecos = copy_str(pwp->pw_gecos, &buf, &bufsize);
    res->pw_dir = copy_str(pwp->pw_dir, &buf, &bufsize);
    res->pw_shell = copy_str(pwp->pw_shell, &buf, &bufsize);

    pwp = res;
End:
    mutex_unlock(&getpw_lock);
    return pwp;
}

#endif


static mutex_t getsp_lock;

struct spwd *getspnam_r(const char *name,
                        struct spwd *res,
                        char *buf,
                        int bufsize)
{
    struct spwd *sp;


    mutex_lock(&getsp_lock);
    sp = getspnam(name);
    if (sp == NULL)
        goto End;

    res->sp_namp = copy_str(sp->sp_namp, &buf, &bufsize);
    res->sp_pwdp = copy_str(sp->sp_pwdp, &buf, &bufsize);
    res->sp_lstchg = sp->sp_lstchg;
    res->sp_min = sp->sp_min;
    res->sp_max = sp->sp_max;
    res->sp_warn = sp->sp_warn;
    res->sp_inact = sp->sp_inact;
    res->sp_expire = sp->sp_expire;
    res->sp_flag = sp->sp_flag;

    sp = res;
End:
    mutex_unlock(&getsp_lock);
    return sp;
}

#else

static int dummy;

#endif
