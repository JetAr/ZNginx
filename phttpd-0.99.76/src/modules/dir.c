/*
** dir.c
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
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include "phttpd.h"

#define DEFAULT_INDEX_PATH "index.html:Welcome.html"


#define DEFAULT_ICON_UNKNOWN \
	"<IMG SRC=\"/icons/unknown.xbm\" ALT=\" \" BORDER=0 HSPACE=5>"

#define DEFAULT_ICON_BACK \
	"<IMG SRC=\"/icons/back.xbm\" ALT=\" \" BORDER=0 HSPACE=5>"

#define DEFAULT_ICON_MENU \
	"<IMG SRC=\"/icons/menu.xbm\" ALT=\" \" BORDER=0 HSPACE=5>"

#define DEFAULT_ICON_BINARY \
	"<IMG SRC=\"/icons/binary.xbm\" ALT=\" \" BORDER=0 HSPACE=5>"



static hashtable_t *directory_icons_table = NULL;
static hashtable_t *add_href_table = NULL;
static char *index_file_path = NULL;
static int allow_dir_listing = 0;


static struct options dir_cfg_table[] =
{
    { "directory-icons",   T_HTABLE, &directory_icons_table,	NULL },
    { "add-href",          T_HTABLE, &add_href_table,	        NULL },

    { "index-file-path",   T_STRING, &index_file_path,		NULL },

    { "allow-dir-listing", T_BOOL,   &allow_dir_listing,	NULL },

    { NULL,                -1,       NULL,			NULL }
};


int pm_init(const char **argv)
{
    char *cfg_path, *cp;
    const char *name;
    int clen;


    name = argv[0];

    if (debug > 1)
        fprintf(stderr, "*** libdir/pm_init(\"%s\") called ***\n", name);

    clen = strlen(name)+6;
    cfg_path = s_malloc(clen);
    s_strcpy(cfg_path, clen, name);

    cp = strrchr(cfg_path, '.');
    if (cp && strcmp(cp, ".so") == 0)
        *cp = '\0';

    s_strcat(cfg_path, clen, ".conf");
    if (config_parse_file(cfg_path, dir_cfg_table, 0) < 0)
        return -1;

    if (config_parse_argv(argv+1, dir_cfg_table) < 0)
        return -1;

    if (index_file_path == NULL)
        index_file_path = s_strdup(DEFAULT_INDEX_PATH);

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** libdir/pm_exit() called ***\n");


    if (directory_icons_table)
    {
        ht_destroy(directory_icons_table);
        s_free(directory_icons_table);
    }

    if (add_href_table)
    {
        ht_destroy(add_href_table);
        s_free(add_href_table);
    }

    if (index_file_path)
        s_free(index_file_path);

    if (debug > 1)
        fprintf(stderr, "*** libdir/pm_exit() done ***\n");
}



static  int dircompare(a, b)
char **a, **b;
{
    struct fsc_dirent *fdp1, *fdp2;
    struct dirent *dp1, *dp2;

    fdp1 = *(struct fsc_dirent **) a;
    fdp2 = *(struct fsc_dirent **) b;

    dp1 = fdp1->dbp;
    dp2 = fdp2->dbp;

    return strcmp(dp1->d_name, dp2->d_name);
}



static int do_dirlisting(int fd,
                         struct connectioninfo *cip,
                         struct httpinfo *hip,
                         int browse_flag)
{
    char buf[1024];
    int result, len, i, j, k, maxlen, n_cols;
    fscentry_t *fep;
    fsinfo_t *fip;
    struct fsc_dirent *fdp, **fdpl;
    unsigned int fsc_flags = 0;


    if (debug > 4)
        fprintf(stderr, "do_dirlisting()\n");

    if (hip && hip->mip && (hip->mip->pragma_flags & MPF_NOCACHE))
        fsc_flags = FSCF_RELOAD;

    fep = fscache_lookup(hip->url, FSCF_GETDATA+fsc_flags);
    if (fep == NULL)
        return -1;

    if (debug > 4)
        fprintf(stderr, "  doing listing, step 1\n");

    fip = fep->fip;
    if (fip == NULL)
    {
        fscache_release(fep);
        return -1;
    }

    if (!S_ISDIR(fip->sb.st_mode))
    {
        fscache_release(fep);
        return -1;
    }

    result = 200;

    if (hip->mip != NULL)
    {
        char *s_since;

        if ((s_since = mime_getheader(hip->mip, "IF-MODIFIED-SINCE", 1)) != NULL)
        {
            struct tm tm_since;
            int t_since;


            if (debug > 2)
                fprintf(stderr, "Got If-Modified-Since: %s\n", s_since);

            t_since = atotm(s_since, &tm_since);
            if (t_since != -1)
            {
                if (fip->sb.st_mtime <= (time_t) t_since)
                {
                    fscache_release(fep);
                    return http_not_modified(cip);
                }
            }
            else
            {
                /* Could not parse the date format - do a string compare */
                http_time_r(&fip->sb.st_mtime, buf, sizeof(buf));
                i = strlen(buf);
                buf[i-1] = '\0';

                if (strcmp(s_since, buf) == 0)
                {
                    fscache_release(fep);
                    return http_not_modified(cip);
                }
            }
        }

        http_sendheaders(fd, cip, result, NULL);
        http_sendlastmodified(fd, fip->sb.st_mtime);

        if (browse_flag)
            fd_puts("Content-Type: application/x-navibrowse\n", fd);
        else
            fd_puts("Content-Type: text/html\n", fd);
    }

    if (hip->mip != NULL)
        fd_putc('\n', fd);

    if (strcasecmp(hip->method, "HEAD") == 0)
    {
        fscache_release(fep);
        return result;
    }

    len = fd_written(fd);

    if (!browse_flag)
        html_sysheader(fd, "H2", "Index of %s%s", server_url, hip->url);

    if (browse_flag)
        fd_puts("application/x-navidir .\n", fd);

    if (strcmp(hip->url, "/") != 0)
    {
        if (browse_flag)
            fd_puts("application/x-navidir ..\n", fd);
        else
            fd_printf(fd, "%s <A HREF=\"%s\">Up to previous directory</A>\n",
                      DEFAULT_ICON_BACK,
                      url_getparent(hip->url, buf, sizeof(buf)));
        /* XXX quote HREF XXX */
    }

    if (fip->data.dir.size > 0)
    {
        fdpl = (struct fsc_dirent **)
               alloca(fip->data.dir.size * sizeof(struct fsc_dirent *));

        i = 0;
        maxlen = 0;
        for (fdp = fip->data.dir.head; fdp; fdp = fdp->next)
        {
            struct dirent *dp;

            dp = fdp->dbp;

            if (dp->d_name[0] == '.')
                continue;

            if (fdp->fep == NULL)
            {
                if (debug)
                    fprintf(stderr, "dir: fdp->fep == NULL, name=%s\n", dp->d_name);
                continue;
            }

            if (fdp->fep->fip == NULL)
            {
                if (debug)
                    fprintf(stderr, "dir: fdp->fep->fip == NULL, name=%s\n", dp->d_name);
                continue;
            }

            if (!S_ISDIR(fdp->fep->fip->sb.st_mode))
                continue;

            len = strlen(dp->d_name);
            if (len > maxlen)
                maxlen = len;

            fdpl[i++] = fdp;
        }


        if (i > 0)
        {
            if (!browse_flag)
            {
                if (i == 1)
                    fd_printf(fd, "<H3>1 Directory:</H3>\n");
                else
                    fd_printf(fd, "<H3>%d Directories:</H3>\n", i);

                fd_puts("<PRE>\n", fd);
            }

            qsort((char **)fdpl, i, sizeof(struct fsc_dirent *),
                  (int (*)(const void *, const void *)) dircompare);

            n_cols = 80/(maxlen+10);

            for (j = k = 0; j < i; j++)
            {
                if (browse_flag)
                {
                    fd_puts("application/x-navidir ", fd);
                    fd_puts(fdpl[j]->dbp->d_name, fd);
                    fd_puts("\n", fd);
                }
                else
                {
                    fd_puts(DEFAULT_ICON_MENU, fd);

                    /* XXX quote HREF XXX */

                    fd_printf(fd, " <A HREF=\"%s/\">%s</A>",
                              fdpl[j]->dbp->d_name,
                              fdpl[j]->dbp->d_name);

                    if (++k < n_cols)
                        fd_printf(fd, "%*s    ",
                                  maxlen-strlen(fdpl[j]->dbp->d_name), "");
                    else
                    {
                        fd_putc('\n', fd);
                        k = 0;
                    }
                }
            }

            if (!browse_flag)
                fd_puts("</PRE>\n", fd);
        }

        i = 0;
        maxlen = 0;
        for (fdp = fip->data.dir.head; fdp; fdp = fdp->next)
        {
            struct dirent *dp;


            dp = fdp->dbp;

            if (dp->d_name[0] == '.')
                continue;

            if (fdp->fep == NULL)
            {
                if (debug)
                    fprintf(stderr, "dir: fdp->fep == NULL, name=%s\n", dp->d_name);
                continue;
            }

            if (fdp->fep->fip == NULL)
            {
                if (debug)
                    fprintf(stderr, "dir: fdp->fep->fip == NULL, name=%s\n", dp->d_name);
                continue;
            }

            if (S_ISDIR(fdp->fep->fip->sb.st_mode))
                continue;

            len = strlen(dp->d_name);
            if (len > maxlen)
                maxlen = len;

            fdpl[i++] = fdp;
        }

        if (i > 0)
        {
            char buf[128];


            if (!browse_flag)
            {
                if (i == 1)
                    fd_printf(fd, "<H3>1 File:</H3>\n");
                else
                    fd_printf(fd, "<H3>%d Files:</H3>\n", i);

                fd_puts("<PRE>\n", fd);
            }

            qsort((char **)fdpl, i, sizeof(struct fsc_dirent *),
                  (int (*)(const void *, const void *)) dircompare);

            for (j = 0; j < i; j++)
            {
                hashentry_t *hep_i, *hep_h;
                char *cp;
                struct stat *sp;


                cp = strrchr(fdpl[j]->dbp->d_name, '.');

                if (browse_flag)
                {
                    /* XXX move the .html -> text/html to a separate module, callable
                       from both file.so and dir.so... */

                    if (cp && strcmp(cp, ".html") == 0)
                        fd_puts("text/html ", fd);
                    else
                        fd_puts("image/gif ", fd);

                    fd_puts(fdpl[j]->dbp->d_name, fd);
                    fd_puts("\n", fd);
                }
                else
                {
                    cp = strrchr(fdpl[j]->dbp->d_name, '.');

                    if (cp)
                        hep_h = ht_lookup(add_href_table, cp, 0);
                    else
                        hep_h = NULL;

                    /* XXX quote HREF XXX */

                    if (hep_h)
                        fd_printf(fd, "<A HREF=\"%s%s%s\">",
                                  hep_h->data,
                                  hip->url,
                                  fdpl[j]->dbp->d_name);

                    if (cp)
                    {
                        hep_i = ht_lookup(directory_icons_table, cp, 0);
                        if (hep_i)
                        {
                            fd_puts(hep_i->data, fd);
                            ht_release(hep_i);
                        }
                        else
                            fd_puts(DEFAULT_ICON_UNKNOWN, fd);
                    }
                    else
                        fd_puts(DEFAULT_ICON_UNKNOWN, fd);

                    if (hep_h)
                        fd_puts("</A>", fd);

                    /* XXX quote HREF XXX */

                    sp = &fdpl[j]->fep->fip->sb;
                    fd_printf(fd, " <A HREF=\"%s\">%s</A>   %*s %10d     %s",
                              fdpl[j]->dbp->d_name,
                              fdpl[j]->dbp->d_name,
                              maxlen-strlen(fdpl[j]->dbp->d_name),
                              "",
                              sp->st_size,
                              ctime_r(&sp->st_mtime, buf, sizeof(buf)));

                    if (hep_h)
                        ht_release(hep_h);
                }
            }

            if (!browse_flag)
                fd_puts("</PRE>\n", fd);
        }
    }

    if (!browse_flag)
        html_sysfooter(fd);

    fscache_release(fep);

    return result;
}


static int http_get_head(struct connectioninfo *cip)
{
    int len, status;
    char url[2048];
    char *orig_url;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
        fprintf(stderr, "*** libdir/http_get_head() called ***\n");

    len = strlen(hip->url);

    if (len == 0 || hip->url[len-1] != '/')
    {
        s_strcpy(url, sizeof(url), server_url);
        s_strcat(url, sizeof(url), hip->url);
        s_strcat(url, sizeof(url), "/");

        return http_redirect(cip, url, hip->request, NULL, 301);
    }

    orig_url = hip->url;

    {
        char *if_path, *c_index_file, *cfp;
        int ilen = strlen(index_file_path)+1;

        if_path = (char *) alloca(ilen);
        s_strcpy(if_path, ilen, index_file_path);

        cfp = if_path;

        c_index_file = strtok_r(if_path, ":", &cfp);
        while (c_index_file)
        {
            s_strcpy(url, sizeof(url), orig_url);
            s_strcat(url, sizeof(url), c_index_file);

            hip->url = url;

            if (debug > 2)
                fprintf(stderr, "Dir: Trying Indexfile '%s'...\n", hip->url);

            /* Try the index.html-file in the directory */
            status = phttpd_request(cip);
            if (status > 0)
            {
#if 0
                hip->url = s_strdup(url);
                s_free(orig_url);
#else
                hip->url = orig_url;
#endif
                return status;
            }

            c_index_file = strtok_r(NULL, ":", &cfp);
        }
    }

    hip->url = orig_url;

    if (allow_dir_listing)
        return do_dirlisting(fd, cip, hip, 0);
    else
        return -1;

}


static int http_browse(struct connectioninfo *cip)
{
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;

    if ( allow_dir_listing == 1 ) /* RK */
        return do_dirlisting(fd, cip, hip, 1);
    else
        return -1;
}


int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;

    if (strcasecmp(hip->method, "GET") == 0 ||
            strcasecmp(hip->method, "HEAD") == 0)
        return http_get_head(cip);
    else if (strcasecmp(hip->method, "BROWSE") == 0)
        return http_browse(cip);
    else
        return -2;
}
