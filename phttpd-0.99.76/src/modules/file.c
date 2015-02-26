/*
** file.c
**
** Copyright (c) 1994-1997 Peter Eriksson <pen@signum.se>
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
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "phttpd.h"


static hashtable_t *content_types_table = NULL;
static hashtable_t *content_encodings_table = NULL;
static char *default_type = NULL;
static int method_put = 0;
static int method_delete = 0;
static int allow_root_write = 0;
static int write_needs_putdir = 1;
static int allow_rmdir = 0;
static int allow_implied_mkdir =0;

static struct options file_cfg_table[] =
{
    { "content-types",     T_HTABLE, &content_types_table,	NULL },
    { "content-encodings", T_HTABLE, &content_encodings_table,	NULL },

    { "default-type",	   T_STRING, &default_type,             NULL },

    { "method-put",        T_BOOL,   &method_put,		NULL },
    { "method-delete",     T_BOOL,   &method_delete,		NULL },
    { "allow-root-write",  T_BOOL,   &allow_root_write,         NULL },
    { "write-needs-putdir", T_BOOL,  &write_needs_putdir,       NULL },
    { "allow-rmdir",       T_BOOL,   &allow_rmdir,              NULL },
    { "allow-implied-mkdir",T_BOOL,  &allow_implied_mkdir,      NULL },

    { NULL,                -1,       NULL,			NULL }
};



static int get_content_type(const char *path,
                            const struct stat *sp,
                            char *type,
                            int typesize,
                            char *encoding,
                            int encsize)
{
    char *suffix;
    hashentry_t *hep;
    int len;


    type[0] = '\0';
    if (encoding)
        encoding[0] = '\0';

    if (debug > 2)
        fprintf(stderr, "*** get_content_type(\"%s\", ...)\n", path);

    if (content_types_table == NULL)
    {
        if (debug > 2)
            fprintf(stderr, "\t-> Content type unknown (NULL table).\n");

        s_strcpy(type, typesize, default_type);
        return 0;
    }


    /* Get suffix */
    suffix = strrchr(path, '.');
    if (suffix == NULL)
    {
        s_strcpy(type, typesize, default_type);
        return 0;
    }

    len = 0;

    if (encoding && content_encodings_table)
    {
        hep = ht_lookup(content_encodings_table, suffix, len);

        if (hep)
        {
            s_strcpy(encoding, encsize, hep->data);

            if (debug > 2)
                fprintf(stderr, "\t -> Encoding found: %s\n", encoding);

            ht_release(hep);

            if (suffix > path)
            {
                char *cp;

                suffix--;
                for (cp = suffix-1; cp > path && *cp != '.'; cp--)
                    ;

                len = suffix - cp+1 ;
                suffix = cp;

                if (debug > 1)
                    fprintf(stderr, "file: new suffix: len = %d, str=%.*s\n",
                            len, len, suffix);
            }
        }
    }

    hep = ht_lookup(content_types_table, suffix, len);
    if (hep)
    {
        s_strcpy(type, typesize, hep->data);
        ht_release(hep);

        if (debug > 2)
            fprintf(stderr, "\t -> Content-Type found: %s\n", type);

        return 0;
    }

    if (debug > 2)
        fprintf(stderr, "\t-> Content type unknown.\n");

    s_strcpy(type, typesize, default_type);
    return 0;
}

int pm_init(const char **argv)
{
    char *cfg_path, *cp;
    const char *name = argv[0];
    int cfgsize;


    if (debug > 1)
        fprintf(stderr, "*** file/pm_init(\"%s\") called ***\n", name);

    cfgsize = strlen(name)+6;
    cfg_path = s_malloc(cfgsize);
    s_strcpy(cfg_path, cfgsize, name);

    cp = strrchr(cfg_path, '.');
    if (cp && strcmp(cp, ".so") == 0)
        *cp = '\0';

    s_strcat(cfg_path, cfgsize, ".conf");
    if (config_parse_file(cfg_path, file_cfg_table, 0) < 0)
        return -1;

    if (config_parse_argv(argv+1, file_cfg_table) < 0)
        return -1;

    if (default_type == NULL)
        default_type = s_strdup("unknown");

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** file/pm_exit() called ***\n");

    if (content_types_table)
    {
        ht_destroy(content_types_table);
        s_free(content_types_table);
    }

    if (content_encodings_table)
    {
        ht_destroy(content_encodings_table);
        s_free(content_encodings_table);
    }

    if (default_type)
        s_free(default_type);

    if (debug > 1)
        fprintf(stderr, "*** file/pm_exit() done ***\n");
}



static int http_get_head(struct connectioninfo *cip)
{
    char typebuf[256], buf[256], encbuf[256], ubuf[1024], *s_since;
    int i, result, head_method = 0;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;
    fscentry_t *fep;
    fsinfo_t *fip;
    unsigned int fsc_flags = 0;
    u_long addr;
    struct hostent *hp, hb;
    char hb_buf[1024];
    char bufx[2048];
    int h_errno;
    char *range;
    off_t from = 0, to = 0;


    if (debug > 1)
        fprintf(stderr, "*** file/pm_get() called ***\n");

    if (hip && hip->mip && (hip->mip->pragma_flags & MPF_NOCACHE))
        fsc_flags = FSCF_RELOAD;

    fep = fscache_lookup(hip->url, fsc_flags);
    if (fep == NULL)
        return -1;

    if (fep->fip == NULL)
    {
        fscache_release(fep);
        return -1;
    }

    fip = fep->fip;

    if (S_ISDIR(fip->sb.st_mode))
    {
        /* RK YES I KNOW, IT IS A HACK -- should create own .c file */
        /* Need to rewrite 'SERVER_URL', will use bufx instead of server_url */
        if ( rkmultimode )
        {
            addr=cip->serverif;
            hp = s_gethostbyaddr_r((char *)&addr, sizeof (addr), AF_INET,
                                   &hb, hb_buf, sizeof(hb_buf), &h_errno);

            if ( hp != NULL )
            {
                s_strcpy(bufx, sizeof(bufx), "http://");
                s_strcat(bufx, sizeof(bufx), hp->h_name);

                if (server_port != 80)
                {
                    int len;

                    len = s_strcat(bufx, sizeof(bufx), ":");
                    s_sprintf(bufx+len, sizeof(bufx)-len, "%d", server_port);
                }
            }
        }

        if (!rkmultimode)
            s_strcpy(ubuf, sizeof(ubuf), server_url);
        else
            s_strcpy(ubuf, sizeof(ubuf), bufx);

        s_strcat(ubuf, sizeof(ubuf), hip->url+cip->vspl);
        s_strcat(ubuf, sizeof(ubuf), "/");
        fscache_release(fep);
        return http_redirect(cip, ubuf, hip->request, NULL, 301);
    }

    if (get_content_type(fip->path, &fip->sb, typebuf, sizeof(typebuf),
                         encbuf, sizeof(encbuf)))
    {
        fscache_release(fep);
        return -1;
    }

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

    if ((s_since = mime_getheader(hip->mip, "IF-UNMODIFIED-SINCE", 1)) != NULL)
    {
        int t_since;

        if (debug > 2)
            fprintf(stderr, "Got If-Unmodified-Since: %s\n", s_since);

        t_since = atotm(s_since, NULL);
        if (t_since != -1)
        {
            if (fip->sb.st_mtime > (time_t) t_since)
            {
                fscache_release(fep);
                return http_precondition_failed(cip);
            }
        }
    }

    /*   strtoull() is being used because of Solaris 2.6 brain damage.
     Don't change it unless you know what you're doing.
    */

    result = 200;

    if ((range = mime_getheader(hip->mip, "RANGE", 1)) != NULL)
    {
        if (debug > 2)
            fprintf(stderr, "Got Range: %s\n", range);

        if(!strncmp(range, "bytes", 5))
        {
            char *p;

            for(range += 5; s_isspace(*range); ++range);
            if(*range++ != '=')
                goto norange;
            while(s_isspace(*range)) ++range;
            if(*range == '-')
            {
                from = 0;
                ++range;
            }
            else
            {
                from = (off_t) strtoull(range, &p, 10);
                if (from == 0 && p == range)
                    goto norange;
                while(s_isspace(*p)) ++p;
                if(*p++ != '-')
                    goto norange;
                range = p;
            }
            while(s_isspace(*range)) ++range;
            if(*range == '\0')
                to = fip->sb.st_size;
            else
            {
                to = (off_t) strtoull(range, &p, 10);
                if (to == 0 && range == p)
                    goto norange;
                if(to > fip->sb.st_size)
                    to = fip->sb.st_size;
                while(s_isspace(*p)) ++p;
                range = p;
            }
            if (to < from)
                goto norange;
            if (*range != '\0')
            {
                if (debug > 2)
                    fprintf(stderr, "Can't serve multiple ranges.\n");

                goto norange;
            }

            if ((s_since = mime_getheader(hip->mip, "IF-RANGE", 1)) != NULL)
            {
                time_t t_since;

                if(debug > 2)
                    fprintf(stderr, "Got If-Range: %s\n", s_since);

                t_since = atotm(s_since, NULL);
                if (t_since != -1)
                    if (fip->sb.st_mtime > (time_t) t_since)
                    {
                        if (debug > 2)
                            fprintf(stderr, "Entity changed since %s\n", s_since);

                        goto norange;
                    }
                    else if (debug > 2)
                        fprintf(stderr, "Couldn't parse If-Range value.\n");
            }

            result = 206;
        }
        else if (debug > 2)
            fprintf(stderr, "Unknown range units\n");
    }

norange:

    if (range != NULL && debug > 2)
        fprintf(stderr, "Ignoring Range header...\n");

    if (strcasecmp(hip->method, "HEAD") == 0)
        head_method = 1;
    else if (fscache_getdata(fep) != 1)
    {
        if (debug > 1)
            fprintf(stderr, "fscache_getdata(): failed\n");

        fscache_release(fep);
        return -1;
    }

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);
        http_sendlastmodified(fd, fip->sb.st_mtime);

        if (typebuf[0])
            fd_puts2nl("Content-Type: ", typebuf, fd);

        if (encbuf[0])
            fd_puts2nl("Content-Encoding: ", encbuf, fd);

        if (result == 200)
            fd_putsinl("Content-Length: ", (int) fip->sb.st_size, fd);
        else
        {
            fd_putsinl("Content-Length: ", (int) (to - from + 1), fd);
            fd_printf(fd, "Content-Range: bytes %d-%d/%d\n",
                      (int) from, (int) to, (int) fip->sb.st_size);
        }

        http_sendlang(fd, hip->url);

        fd_putc('\n', fd);
    }

    if (head_method)
    {
        fscache_release(fep);
        return result;
    }

    hip->length = fip->sb.st_size;

    if (fd_write(fd,
                 fip->data.file.content + (result == 200 ? 0 : from),
                 result == 200 ? fip->sb.st_size : (to - from + 1)) < 0)
        if (debug)
        {
            fprintf(stderr, "ERROR on #%d: ", cip->request_no);
            perror("write");
        }

    fscache_release(fep);

    if (debug > 2)
        fprintf(stderr, "*** file/http_get: Returning\n");

    return result;
}


static int http_delete(struct connectioninfo *cip)
{
    char path[MAXPATHLEN+1];
    struct stat sb;
    int result;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
        fprintf(stderr, "*** file/pm_delete() called ***\n");

    if (url_expand(hip->url, path, sizeof(path), &sb, NULL, NULL) == NULL)
        return -1;

    if (debug > 6)
        fprintf(stderr, "url-found on %s\n",path);

    if (sb.st_uid==0 && !allow_root_write)
    {
        fprintf(stderr, "SECURITY-ERROR: unlink try on %s\n",path);
        return -403;
    }

    /* added virtual user access/check here .... */
    if (write_needs_putdir)
    {
        if (cip->auth.xtype != AUTH_XTYPE_FILE ||
                cip->auth.xinfo == NULL)
        {
            fprintf(stderr,"Authfile incomplete: no putdir\n");
            return -403;
        }
        if (strmatch(path,cip->auth.xinfo) == 0)
        {
            fprintf(stderr,"Error: putdir does not match %s %s\n",
                    path, (char *) cip->auth.xinfo);
            return -403;
        }
    }

    if (debug > 3)
        fprintf(stderr, "UNLINK: %s\n",path);

    /* do not allow rmdir ! -- may change ??? */
    if ((!S_ISDIR(sb.st_mode)) && allow_rmdir == 0)
        return -1;

    if (S_ISDIR(sb.st_mode))
    {
        if (rmdir(path) != 0)
            return -403;
    }
    else
    {
        if (s_unlink(path)!=0)
            return -403;
    }

    result=200;

    if (hip->mip != NULL)
    {
        http_sendheaders(cip->fd, cip, result, NULL);

        http_sendlastmodified(cip->fd, sb.st_mtime);
        fd_puts("Content-Type: text/html\n\n", cip->fd);
    }

    html_sysheader(cip->fd, "H2", "Succeded - File removed");
    fd_puts("OK\n", cip->fd);
    html_sysfooter(cip->fd);

    if (debug > 2)
        fprintf(stderr, "*** file/http_delete: Returning\n");

    return result;
}


static int http_put(struct connectioninfo *cip)
{
    char buf[2048], path[MAXPATHLEN+1];
    char newpath[MAXPATHLEN+1],newfile[MAXPATHLEN+1];
    char newsubpath[MAXPATHLEN+1];
    char *pclen, *p;
    struct stat sb;
    int fd_out, len, result, clen;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
        fprintf(stderr, "*** file/pm_put() called ***\n");

    if (url_expand(hip->url, path, sizeof(path), &sb, NULL, NULL) == NULL)
    {
        /* Lets try to create a new file ....*/
        s_strcpy(newpath, sizeof(newpath), hip->url);

        p=newpath+strlen(newpath)-1;
        if (p <= newpath)
            return -1;

        while (p > newpath && *p != '/')
            p--;
        if (p < newpath)
            return -1;
        p++;
        s_strcpy(newfile, sizeof(newfile), p);
        *p=0x00;

        if (debug > 4)
            fprintf(stderr, "Checking DIR %s\n",newpath);

        /* If this it NOT a dir, FAIL..... */

        /* add allow_implied_mkdir here ...RK */
        /* TODO..... */

        if (url_expand(newpath, path, sizeof(path), &sb, NULL, NULL) == NULL)
        {
            if ( allow_implied_mkdir==0 )
                return -1;
            /* Lets try to create that dir, maybe later add here mkdir -p */
            /* for now, only single mkdir is enabled..... */
            s_strcpy(newsubpath, sizeof(newsubpath),newpath);
            p=newsubpath+strlen(newsubpath)-2;
            if (p <= newsubpath)
                return -1;

            while (p > newsubpath && *p != '/')
                p--;
            if (p < newsubpath)
                return -1;
            *p=0x00;
            p++;
            /* try again */
            if (debug>4)
                fprintf(stderr, "Checking 2.Dir %s\n",newsubpath);

            if (url_expand(newsubpath, path, sizeof(path), &sb, NULL, NULL) == NULL)
                return -1;

            if (!S_ISDIR(sb.st_mode))
                return -1;

            if (sb.st_uid==0 && allow_root_write==0)
            {
                fprintf(stderr, "SECURITY-ERROR: create try on %s\n",path);
                return -403;
            }
            /* now try to create directory....*/
            s_sprintf(path,sizeof(path),"%s/%s",path,p);
            if (mkdir(path, S_IRWXU+S_IRGRP+S_IXGRP+S_IROTH+S_IXOTH )!=0 )
                return -1;
            fprintf(stderr, "mkdir succeeded...\n");
            if (url_expand(newpath, path, sizeof(path), &sb, NULL, NULL) == NULL)
                return -1;
        } /* end of sub-mkdir */

        if (!S_ISDIR(sb.st_mode))
            return -1;

        if (sb.st_uid==0 && allow_root_write==0)
        {
            fprintf(stderr, "SECURITY-ERROR: create try on %s\n",path);
            return -403;
        }

        s_strcat(path, sizeof(path), "/");
        s_strcat(path, sizeof(path), newfile);

        if (debug > 4)
            fprintf(stderr, "file/pm_put(): creating %s %s\n",newpath, path);
    }

    if (sb.st_uid==0 && !allow_root_write)
    {
        fprintf(stderr, "SECURITY-ERROR: write try on %s\n",path);
        return -403;
    }

    /* added virtual user access/check here .... */
    if (write_needs_putdir)
    {
        if (cip->auth.xtype != AUTH_XTYPE_FILE ||
                cip->auth.xinfo == NULL)
        {
            fprintf(stderr,"Authfile incomplete: no putdir\n");
            return -403;
        }

        if (strmatch(path,cip->auth.xinfo) == 0)
        {
            fprintf(stderr,"Error: putdir does not match %s %s\n",
                    path, (char *) cip->auth.xinfo);
            return -403;
        }
    }

    fd_out = fd_open(path, O_WRONLY+O_CREAT+O_TRUNC, 0644);
    if (fd_out < 0)
    {
        fprintf(stderr, "Unable top open/create: %s\n", path);
        return -403;
    }


    pclen=mime_getheader(hip->mip, "CONTENT-LENGTH", 1);
    clen= ( pclen != NULL ) ? atoi(pclen) : 0 ;

    if (debug > 4)
        fprintf(stderr, "*** file/pm_put() Bytes %d \n", clen );

    if ( clen > 0 )
        while ( clen > 0 )
        {
            len = fd_read(fd, buf, (sizeof(buf)<clen)? sizeof(buf) : clen );
            if (fd_write(fd_out, buf, len) != 0) /* BUG: must be 0, !len RK */
            {
                fd_close(fd_out);
                return error_system(cip, "do_putpost(): cwrite()");
            }
            clen=clen-len;
        }
    else
        while ((len = fd_read(fd, buf, sizeof(buf))) > 0 )
        {
            if (fd_write(fd_out, buf, len) != len)
            {
                fd_close(fd_out);
                return error_system(cip, "do_putpost(): write()");
            }
        }

    hip->length = fd_close(fd_out);

    result = 200;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);

        http_sendlastmodified(fd, sb.st_mtime);
        fd_puts("Content-Type: text/html\n\n", fd);
    }

    html_sysheader(fd, "H2", "Succeded");
    fd_puts("OK\n", fd);
    html_sysfooter(fd);

    if (debug > 2)
        fprintf(stderr, "*** file/http_put: Returning\n");

    return result;
}


int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;

    if (strcasecmp(hip->method, "GET") == 0 ||
            strcasecmp(hip->method, "HEAD") == 0)
        return http_get_head(cip);
    else if (strcasecmp(hip->method, "PUT") == 0 )
    {
        if (method_put==1)
            return http_put(cip);
        else
            return -405;
    }
    else if (strcasecmp(hip->method, "DELETE") == 0 )
    {
        if (method_put && method_delete)
            return http_delete(cip);
        else
            return -405;
    }
    else
        return -2;
}
