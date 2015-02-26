/*
** auth_syspwd.c
**
** Copyright (c) 1994-1996 Peter Eriksson <pen@signum.se>
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
#include <crypt.h>

#include "phttpd.h"

int pm_init(const char **argv)
{
    const char *name = argv[0];

    if (debug > 1)
        fprintf(stderr, "*** auth_syspwd/pm_init(\"%s\") called ***\n", name);

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** auth_syspwd/pm_exit() called ***\n");
}


int pm_auth(struct connectioninfo *cip,
            const char *domain,
            const char *type,
            const char *user,
            const char *password)
{
    char pbuf[1024], *sys_pwd, *cp;
#ifndef __osf__
    struct spwd spwb;
#endif
    struct passwd pwb;


    cip->auth.xinfo = NULL;
    cip->auth.xfree = NULL;
    cip->auth.xsetenv = NULL;


    if (debug > 1)
        fprintf(stderr, "*** auth_syspwd/pm_auth(%s,%s,%s,%s)\n",
                domain, type, user, password);
#ifndef __osf__
    if (s_getspnam_r(user, &spwb, pbuf, sizeof(pbuf)) == NULL)
#endif
        if (s_getpwnam_r(user, &pwb, pbuf, sizeof(pbuf)) == NULL)
            return 0;
        else
            sys_pwd = pwb.pw_passwd;
#ifndef __osf__
    else
        sys_pwd = spwb.sp_pwdp;
#endif

    if (password)
    {
        if ((cp = crypt(password, sys_pwd)) == NULL)
            return 0;

        if (strcmp(cp, sys_pwd) == 0)
        {
            cip->auth.user = s_strdup(user);
            return 1;
        }
    }
    else if (sys_pwd[0] == '\0')
    {
        cip->auth.user = s_strdup(user);
        return 1;
    }

    return 0;
}

