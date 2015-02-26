/*
** access.c
**
** Copyright (c) 1994-1995 Peter Eriksson <pen@signum.se>
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

#include <string.h>
#include <syslog.h>
#include <alloca.h>
#include "phttpd.h"


struct table *auth_handlers_table = NULL;


static int get_acl(int fd,
                   char *pattern,
                   int psize,
                   int *value,
                   const char *filename)
{
    char buf[2048], *cp, *tok;
    int key;


Loop:
    if (fd_gets(buf, sizeof(buf), fd) == NULL)
        return 0;

    cp = strchr(buf, '#');
    if (cp)
        *cp = '\0';

    cp = buf;
    while (s_isspace(*cp))
        ++cp;
    if (*cp == '\0')
        goto Loop;

    tok = strtok_r(buf, " \t\r\n", &cp);
    if (tok == NULL)
    {
        syslog(LOG_ERR, "Invalid ACL file (%s) type entry: %s",
               filename, tok);
        goto Loop;
    }

    if (strcmp(tok, "address") == 0)
        key = 1;
    else if (strcmp(tok, "host") == 0)
        key = 2;
    else
        key = -1;

    tok = strtok_r(NULL, " \t\r\n", &cp);
    if (tok == NULL)
    {
        syslog(LOG_ERR, "Invalid ACL file (%s) pattern entry: %s",
               filename, tok);
        goto Loop;
    }

    s_strcpy(pattern, psize, tok);

    tok = strtok_r(NULL, " \t\r\n", &cp);
    if (tok == NULL ||
            !(strcmp(tok, "allow") == 0 ||
              strcmp(tok, "reject") == 0))
    {
        syslog(LOG_ERR, "Invalid ACL file (%s) value entry: %s",
               filename, tok == NULL ? "<null>" : tok);
        goto Loop;
    }

    *value = (strcmp(tok, "allow") == 0);

    return key;
}


int access_check(const char *path, struct connectioninfo *cip)
{
    char *address = cip->client.addr_s;
    char *host = NULL;
    int fd, value, key;
    char pattern[256];


    fd = fd_open(path, O_RDONLY);
    if (fd == -1)
    {
        syslog(LOG_ERR, "Unable to open ACL file: %s", path);
        return 0;
    }

    while ((key = get_acl(fd, pattern, sizeof(pattern), &value, path)) != 0)
    {
        switch (key)
        {
        case 1:
            if (strmatch(address, pattern))
            {
                fd_close(fd);
                return value;
            }
            break;

        case 2:
            host = get_hostname(&cip->client);
            if (!host)
                goto Fail;

            if (strmatch(host, pattern))
            {
                fd_close(fd);
                return value;
            }
            break;

        default:
            goto Fail;
        }
    }

Fail:
    fd_close(fd);

    return 0;
}


static char b64tab[] =
{
    'A','B','C','D','E','F','G','H','I','J','K','L','M',
    'N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
    'a','b','c','d','e','f','g','h','i','j','k','l','m',
    'n','o','p','q','r','s','t','u','v','w','x','y','z',
    '0','1','2','3','4','5','6','7','8','9','+','/', 0
};

int b64tonum(int c)
{
    char *cp;


    cp = strchr(b64tab, c);
    if (cp == NULL)
        return -1;

    return cp - b64tab;
}


int base64_decode(const char *b64data, char *buf, int bufsize)
{
    int state, i;
    char *end;

    end = buf + bufsize;


    state = 0;
    i = 0;
    while (s_isspace(b64data[i]))
        ++i;

    while (b64data[i] && b64data[i+1] &&
            b64data[i+2] && b64data[i+3] && buf + 4 < end)
    {
        *buf = ((b64tonum(b64data[i]) << 2) +
                (b64tonum(b64data[i+1]) >> 4));

        if (*buf == -1)
            break;
        ++buf;

        *buf = ((b64tonum(b64data[i+1]) << 4) +
                (b64tonum(b64data[i+2]) >> 2));

        if (*buf == -1)
            break;
        ++buf;

        *buf = ((b64tonum(b64data[i+2]) << 6) +
                b64tonum(b64data[i+3]));

        if (*buf == -1)
            break;
        ++buf;

        i += 4;
    }

    *buf = '\0';

    return 1;
}



static const char *auth_gethandler(const char *domain)
{
    int i;
    int len;


    len = strlen(domain);

    if (auth_handlers_table)
    {
        for (i = 0; i < auth_handlers_table->length; i++)
        {
            char **pair = auth_handlers_table->value[i];

            if (debug > 3)
                fprintf(stderr, "       (Checking AUTH %s vs %s)\n",
                        domain, pair[0]);

            if (strmatch(domain, pair[0]))
            {
                if (debug > 1)
                    fprintf(stderr, "      (AUTH %s -> Handler %s)\n",
                            domain, pair[1]);

                return pair[1];
            }
        }
    }

    return NULL;
}



static int auth_call(const char *handler,
                     struct connectioninfo *cip,
                     const char *domain,
                     const char *type,
                     const char *user,
                     const char *password)
{
    struct modinfo *mp;
    void *mkey;
    int status;
    char buf[2048];


    while (*handler && *handler == '/')
        ++handler;

    s_strcpy(buf, sizeof(buf), modules_home);
    s_strcat(buf, sizeof(buf), "/");
    s_strcat(buf, sizeof(buf), handler);

    mp = md_load(buf, &mkey);
    if (mp == NULL)
        return 0;

    if (mp->pm_auth)
        status = mp->pm_auth(cip, domain, type, user, password);
    else
        status = 0;

    md_release(mkey);
    return status;
}



int access_auth(struct connectioninfo *cip,
                const char *domain)
{
    char *authinfo, *type, *data, *cp, *user, *password;
    char *auth, buf[2048];
    const char *handler;
    int result, alen;


    if (cip == NULL || cip->hip == NULL || cip->hip->mip == NULL)
        return 0;

    auth = mime_getheader(cip->hip->mip, "AUTHORIZATION", 1);
    if (auth == NULL)
        return 0;

    S_STRALLOCA(auth, &authinfo, 0, &alen);

    type = strtok_r(authinfo, " \t", &cp);
    cip->auth.type = s_strdup(type);

    cip->auth.xinfo = NULL;
    cip->auth.xfree = NULL;
    cip->auth.xsetenv = NULL;


    data = strtok_r(NULL, " \t", &cp);


    /* We only handle the Basic authentication type for now */
    if (strcasecmp(type, "basic") != 0)
        return 0;

    if (!base64_decode(data, buf, sizeof(buf)))
        return 0;

    user = buf;
    password = strchr(buf, ':');
    if (password)
        *password++ = '\0';


    result = 0;

    handler = auth_gethandler(domain);
    if (handler)
    {
        if (debug > 2)
            fprintf(stderr, "*** Calling AUTH handler %s\n", handler);

        result = auth_call(handler,
                           cip,
                           domain,
                           type,
                           user,
                           password);

        if (result != 1)
            result = 0;
    }

    if (password)
        memset(password, 'X', strlen(password));

    return result;
}

