/*
** url.c
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

#include <stdio.h>
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "phttpd.h"


#define HTOI(c)		(((c) >= 'A') ? (((c) & 0xDF) - 'A') + 10 : (c) - '0')


struct table *url_handlers_table = NULL;
struct table *url_redirects_table = NULL;
struct table *url_predirects_table = NULL;
struct table *url_rewrites_table = NULL;
struct table *read_authenticate_table = NULL;
struct table *write_authenticate_table = NULL;
struct table *host_access_table = NULL;
struct table *url_expand_table = NULL;
struct table *content_language_table = NULL;


char *expand_symlink(const char *path,
                     char *realpath,
                     int maxsize)
{
    char *buf;
    char *cp;
    int len;


    buf = (char *) alloca(maxsize+1);
    if (buf == NULL)
        return NULL;

    if ((len = s_readlink(path, buf, maxsize)) < 0)
    {
        if (errno == EINVAL)
        {
            s_strcpy(realpath, maxsize, path);
            return realpath;
        }
        return NULL;
    }

    buf[len] = '\0';

    if (buf[0] == '/')
        s_strcpy(realpath, maxsize, buf);
    else
    {
        cp = strrchr(path, '/'); /* Find last / in filename */
        if (cp)
        {
            s_strcpy(realpath, maxsize, path);
            s_strcpy(realpath + (cp - path) + 1, maxsize, buf);
        }
        else
            s_strcpy(realpath, maxsize, buf);
    }

    return realpath;
}


char *url_expand(const char *path,
                 char *realpath,
                 int maxsize,
                 struct stat *sp,
                 uid_t *uid,
                 gid_t *gid)
{
    const char *cp;
    char *outpath;
    struct passwd *pwp;
    void *uc_key;
    char buf[2048];
    char name[64];
    char *end;
    int len;
    int i;


    outpath = (char *) alloca(maxsize);
    if (outpath == NULL)
        return NULL;

    /* An URL must begin with a '/' */
    if (*path++ != '/')
        return NULL;

    /* And the second character must NOT be a '/' */
    if (*path == '/')
        return NULL;

    /* "/../" is not allowed in the path */
    for (cp = path-1; *cp; cp++)
        if (cp[0] == '/' && cp[1] == '.' && cp[2] == '.' &&
                (cp[3] == '/' || cp[3] == '\0'))
        {
            return NULL;
        }

    /* Handle ~user expansion */
    if (*path == '~')
    {
        /* Locate end of username */
        end = strchr(++path, '/');
        if (end)
        {
            len = end - path;
            if (len+1 > sizeof(buf))
                len = sizeof(buf)-1;
        }
        else
            len = strlen(path);

        s_strncpy(name, sizeof(name), path, len);

        if ((uc_key = usercache_lookup(name, 0, &pwp)) == NULL)
            return NULL;

        if (pwp == NULL || pwp->pw_dir == NULL)
        {
            usercache_release(uc_key);
            return NULL;
        }

        if (uid)
            *uid = pwp->pw_uid;
        if (gid)
            *gid = pwp->pw_gid;

        if (user_basedir)
        {
            len = s_strcpy(outpath, maxsize, user_basedir);
            if (len > 0 && outpath[len-1] != '/')
                s_strcat(outpath, maxsize, "/");
            len = s_strcat(outpath, maxsize, name);
        }
        else
            len = s_strcpy(outpath, maxsize, pwp->pw_dir);

        if (user_dir && user_dir[0])
        {
            if (len > 0 && outpath[len-1] != '/')
                len = s_strcat(outpath, maxsize, "/");

            s_strcat(outpath, maxsize, user_dir);
        }

        if (end)
            s_strcat(outpath, maxsize, end);

        usercache_release(uc_key);
    }
    else
        s_strcpy(outpath, maxsize, path);

    i = strlen(outpath) - 1;
    while (i >= 0 && outpath[i] == '/')
        i--;

    outpath[i+1] = '\0';

    if (outpath[0] == '\0')
        s_strcpy(outpath, maxsize, ".");

    if (sp && s_stat(outpath, sp) < 0)
        return NULL;

    s_strcpy(realpath, maxsize, outpath);
    return realpath;
}



char *url_getparent(const char *path, char *buf, int bufsize)
{
    char *cp;


    s_strcpy(buf, bufsize, path);

    cp = strrchr(buf, '/');
    if (cp && cp > buf)
    {
        /* Remove trailing '/' */
        *cp = '\0';

        cp = strrchr(buf, '/');
        if (cp)
            *++cp = '\0';
        else
        {
            s_strcpy(buf, bufsize, "/");
            return buf;
        }

        return buf;
    }
    else
    {
        s_strcpy(buf, bufsize, "/");
        return buf;
    }
}


char *url_quote(const char *from,
                char *to,
                int size,
                char *quotemap,
                int only_qm)
{
    static const char bin2hex[] = "0123456789ABCDEF";
    char *retval;
    const unsigned char *fp;


    fp = (const unsigned char *) from;
    retval = to;

    if (quotemap == NULL)
        quotemap = "\"+?";

    while (*fp)
    {
        if (only_qm ?
                (strchr(quotemap, *fp) != NULL) :
                (*fp == '%' ||
                 *fp == '"' ||
                 *fp <= ' ' ||
                 strchr(quotemap, *fp) != NULL ||
                 (*fp >= 127 &&
                  *fp <= (unsigned char) (128+(unsigned char)' ')) ||
                 *fp == 255))
        {
            if ((to-retval) < size-3)
            {
                *to++ = '%';
                *to++ = bin2hex[((*fp) >> 4) & 0x0F];
                *to++ = bin2hex[(*fp) & 0x0F];
                ++fp;
            }
            else
                break;
        }
        else if ((to-retval) < size-1)
            *to++ = *fp++;
        else
            break;
    }

    *to = '\0';

    return retval;
}



void url_unquote(char *str, int plus_flag)
{
    char *to;
    int val;


    to = str;

    while (*str)
        switch (*str)
        {
        case '+':
            if (plus_flag)
                *to++ = ' ';
            else
                *to++ = '+';

            ++str;
            break;

        case '%':
            ++str;
            val = 0;

            if (*str && s_isxdigit(*str))
            {
                val = HTOI(*str);
                ++str;
            }

            if (*str && s_isxdigit(*str))
            {
                val *= 16;
                val += HTOI(*str);
                ++str;
            }
            *to++ = val;

            break;

        default:
            *to++ = *str++;
        }

    *to = '\0';
}


const char *url_gethandler(char *url)
{
    int i;
    int len;


    len = strlen(url);

    if (url_handlers_table)
    {
        for (i = 0; i < url_handlers_table->length; i++)
        {
            char **pair = url_handlers_table->value[i];
#if 0
            int slen = strlen(pair[0]);
#endif

            if (debug > 3)
                fprintf(stderr, "       (Checking URL %s vs %s)\n",
                        url, pair[0]);
#if 1
            if (strmatch(url, pair[0]))
            {
                if (debug > 1)
                    fprintf(stderr, "      (URL %s -> Handler %s)\n",
                            url, pair[1]);

                return pair[1];
            }
#else
            if (pair[0][0] == '*')
            {
                slen--;

                if (slen == 0)
                {
                    if (debug > 1)
                        fprintf(stderr, "      (URL %s -> Handler %s)\n",
                                url, pair[1]);

                    return pair[1];
                }

                if (len >= slen &&
                        strcmp(url + len - slen,
                               pair[0] + 1) == 0)
                {
                    if (debug > 1)
                        fprintf(stderr, "      (URL %s -> Handler %s)\n",
                                url, pair[1]);

                    return pair[1];
                }
            }
            else if ((slen > 0 && pair[0][slen-1] == '*' &&
                      strncmp(url, pair[0], slen-1) == 0 &&
                      url[slen-1]) ||
                     strcmp(url, pair[0]) == 0)
            {
                if (debug > 1)
                    fprintf(stderr, "      (URL %s -> Handler %s)\n",
                            url, pair[1]);

                return pair[1];
            }
            else
            {
                if (strcmp(url, pair[0]) == 0)
                {
                    if (debug > 1)
                        fprintf(stderr, "      (URL %s -> Handler %s)\n",
                                url, pair[1]);

                    return pair[1];
                }
            }
#endif
        }
    }

    if (len > 0 && url[len-1] == '/')
    {
        if (debug > 1)
            fprintf(stderr, "      (URL %s -> Handler %s)\n",
                    url, default_dir_handler);

        return default_dir_handler;
    }
    else
    {
        if (debug > 1)
            fprintf(stderr, "      (URL %s -> Handler %s)\n",
                    url, default_file_handler);

        return default_file_handler;
    }
}



char *url_match(struct table *tp,
                const char *url,
                char *result,
                int size)
{
    int i;


    if (tp == NULL)
        return NULL;

    for (i = 0; i < tp->length; i++)
    {
        char **pair = tp->value[i];

        if (strmatch(url, pair[0]))
        {
            s_strcpy(result, size, pair[1]);
            return result;
        }
    }

    return NULL;
}


char *url_getredirect(const char *url,
                      char *out_url,
                      int size)
{
    char rewrite_pattern[256];

    if (url_match(url_redirects_table, url,
                  rewrite_pattern, sizeof(rewrite_pattern)))
    {
        if (url_rewrite(url, rewrite_pattern, out_url, size))
            return out_url;
    }

    return NULL;
}




char *url_getpredirect(const char *url,
                       char *out_url,
                       int size)
{
    char rewrite_pattern[256];

    if (url_match(url_predirects_table, url,
                  rewrite_pattern, sizeof(rewrite_pattern)))
    {
        if (url_rewrite(url, rewrite_pattern, out_url, size))
            return out_url;
    }

    return NULL;
}


char *url_getrewrite(const char *url,
                     char *out_url,
                     int url_size)
{
    char rewrite_pattern[1024];


    if (url_match(url_rewrites_table, url,
                  rewrite_pattern, sizeof(rewrite_pattern)))
    {
        return url_rewrite(url, rewrite_pattern,
                           out_url, url_size);
    }

    return NULL;
}

/* added RK */
char *write_getauthenticate(const char *url,
                            char *out_url,
                            int size)
{
    char auth_buf[1024];


    if (url_match(write_authenticate_table, url,
                  auth_buf, sizeof(auth_buf)))
    {
        s_strcpy(out_url, size, auth_buf);
        return out_url;
    }

    return NULL;
}



char *read_getauthenticate(const char *url,
                           char *out_url,
                           int size)
{
    char auth_buf[1024];


    if (url_match(read_authenticate_table, url,
                  auth_buf, sizeof(auth_buf)))
    {
        s_strcpy(out_url, size, auth_buf);
        return out_url;
    }

    return NULL;
}


char *host_getaccess(const char *url,
                     char *out_url,
                     int size)
{
    return url_match(host_access_table, url, out_url, size);
}


static int get_num(char **cp)
{
    int val = 0;

    while (s_isdigit(**cp))
    {
        val *= 10;
        val += (**cp - '0');

        ++*cp;
    }

    return val;
}

static int get_args(char **cp, int *v1, int *v2)
{
    if (**cp != '{')
        return 0;

    ++*cp;

    if (s_isdigit(**cp))
    {
        *v1 = get_num(cp);

        if (**cp == '}')				/* %{99} */
        {
            *v2 = *v1;

            ++*cp;

            return 1;
        }

        else if (**cp == '-')
        {
            ++*cp;

            if (s_isdigit(**cp))				/* %{1-99} */
            {
                *v2 = get_num(cp);

                if (**cp == '}')
                {
                    ++*cp;

                    return 1;
                }
            }

            else if (**cp == '}')
            {
                *v2 = 99999;

                ++*cp;

                return 1;
            }

            return 0;
        }
        else
        {
            return 0;
        }
    }

    else if ((*cp)[0] == '*' && (*cp)[1] == '}')	/* %{*} */
    {
        *v1 = 0;
        *v2 = 999999; /* Some huge number */

        ++*cp;
        ++*cp;

        return 1;
    }

    else if (**cp == '-')				/* %{-99} */
    {
        *v1 = 0;
        ++*cp;

        if (s_isdigit(**cp))
        {
            *v2 = get_num(cp);

            if (**cp == '}')
            {
                ++*cp;

                return 1;
            }
            else
            {
                return 0;
            }
        }

        else if (**cp == '}')				/* %{-} */
        {
            *v2 = -1;
            ++*cp;

            return 1;
        }

        return 0;
    }
    else
        return 0;
}



/*
** Yuk! This code is _ugly_! Can't someone rewrite it to be nicer
** to read, and have more features?
*/
char *url_rewrite(const char *url,
                  const char *pattern,
                  char *buf,
                  int size)
{
    char **argv;
    char *start = buf;
    const char *ppos = pattern;
    char *cp;
    int i;
    int argc;
    char *tmp;
    int tlen;


    if (debug > 2)
        fprintf(stderr, "url_rewrite(\"%s\", \"%s\", ..., %d)\n",
                url, pattern, size);

    tlen = strlen(url)+1;
    tmp = (char *) alloca(tlen);
    if (tmp == NULL)
    {
        if (debug > 2)
            fprintf(stderr, "\t-> NULL\n");

        return NULL;
    }

    s_strcpy(tmp, tlen, url+1);

    argv = strsplit(tmp, '/', 0);

    for (argc = 0; argv[argc]; argc++)
        ;

    while ((cp = strchr(ppos, '%')) != NULL)
    {
        int len = cp - ppos;
        int v1, v2;


        if (len > 0)
        {
            s_strncpy(buf, size, ppos, len);
            buf += len;
            size -= len;
        }

        ++cp;

        if (get_args(&cp, &v1, &v2))
        {
            for (i = v1; i <= v2 && i < argc; i++)
            {
                s_strcpy(buf, size, argv[i]);

                while (*buf)
                {
                    ++buf;
                    --size;
                }

                if (i < v2 && i + 1< argc)
                {
                    *buf++ = '/';
                    --size;
                }
            }
        }

        ppos = cp;
    }

    s_strcpy(buf, size, ppos);
    s_free(argv);

    if (debug > 2)
        fprintf(stderr, "\t-> \"%s\"\n", start);

    return start;
}



