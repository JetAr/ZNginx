/*
** ncsa-html.c
**
** Copyright (c) 1995 Marcus E. Hennecke <marcush@leland.stanford.edu>
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
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <alloca.h>
#include <syslog.h>

#include "phttpd.h"
#include "ncsa-html.h"

#define DEFAULT_TIMEFMT "%A, %d-%b-%y %T %Z"

#ifndef MAP_FAILED
#define MAP_FAILED ((caddr_t) -1)
#endif


static char *exec_user = NULL;
static char *exec_group = NULL;
static int max_environ = 256;
static char *exec_path = NULL;

static char *exec_newroot = NULL;
static rlim_t exec_niceval = 5;
static rlim_t exec_vmem = 0;
static rlim_t exec_fdmax = 0;
static rlim_t exec_maxcputime = 0;

static int exec_enabled = 0;
static int exec_user_enabled = 0;
static uid_t default_exec_uid = 60001; /* Nobody */
static gid_t default_exec_gid = 60001; /* Nobody */


/* These functions implement server side includes */
static void cmd_config(struct segment *,
                       struct attr_pair *,
                       struct includeinfo *);

static void cmd_user(struct segment *,
                     struct attr_pair *,
                     struct includeinfo *);

static void cmd_echo(struct segment *,
                     struct attr_pair *,
                     struct includeinfo *);

static void cmd_include(struct segment *,
                        struct attr_pair *,
                        struct includeinfo *);

static void cmd_fsize(struct segment *,
                      struct attr_pair *,
                      struct includeinfo *);

static void cmd_flastmod(struct segment *,
                         struct attr_pair *,
                         struct includeinfo *);

static void cmd_count(struct segment *,
                      struct attr_pair *,
                      struct includeinfo *);

static void cmd_exec(struct segment *,
                     struct attr_pair *,
                     struct includeinfo *);



static struct command commands[] =
{
    { "config",		cmd_config,	1 },
    { "echo",		cmd_echo,	1 },
    { "include",	cmd_include,	1 },
    { "fsize",		cmd_fsize,	1 },
    { "flastmod",	cmd_flastmod,	1 },
    { "count",		cmd_count,	1 },
    { "exec",		cmd_exec,	1 },
    { "user",		cmd_user,	1 },
    { NULL,		NULL,		0 }
};


/* (very simple) state machines for the searches */
static int start_fsm[256];
static int end_fsm[256];

/* This lookup table is used for escaping shell characters */
#define SHELL_ESCAPE 1
static char ch_type[256];
#define isshesc(c)	(ch_type[((unsigned int)(c))&255] & SHELL_ESCAPE)

static struct options shtml_cfg_table[] =
{
    { "default-exec-user",	T_STRING, &exec_user, NULL },
    { "default-exec-group",	T_STRING, &exec_group, NULL },
    { "exec-path",		T_STRING, &exec_path, NULL },

    { "exec-environ-size",	T_NUMBER, &max_environ, NULL },

    { "allow-exec",		T_BOOL,   &exec_enabled, NULL },
    { "allow-user-exec",	T_BOOL,   &exec_user_enabled, NULL },
    { "newroot",		T_STRING, &exec_newroot, NULL },
    { "nicevalue",      	T_NUMBER, &exec_niceval, NULL },
    { "maxcputime",      	T_NUMBER, &exec_maxcputime, NULL },
    { "max-vmem",       	T_NUMBER, &exec_vmem, NULL },
    { "max-fd",         	T_NUMBER, &exec_fdmax, NULL },

    { NULL, 0, NULL, NULL }
};


int pm_init(const char **argv)
{
    struct passwd *pwp, pwb;
    char pbuf[1024];
    char *cfg_path, *cp;
    const char *name;
    int i, cfg_size;


    name = argv[0];

    if (debug > 1)
        fprintf(stderr, "*** ncsa-html/pm_init(\"%s\") called ***\n", name);

    cfg_path = s_strxdup(name, 6, &cfg_size);

    cp = strrchr(cfg_path, '.');
    if (cp && strcmp(cp, ".so") == 0)
        *cp = '\0';

    s_strcat(cfg_path, cfg_size, ".conf");
    if (config_parse_file(cfg_path, shtml_cfg_table, 0) < 0)
        return -1;

    if (config_parse_argv(argv+1, shtml_cfg_table) < 0)
        return -1;

    if (uidgid_get(exec_user, exec_group,
                   &default_exec_uid, &default_exec_gid,
                   &pwp, &pwb, pbuf, sizeof(pbuf)) < 0)
    {
        syslog(LOG_ERR,
               "ncsa-html: uidgid_get(\"%s\", \"%s\") failed, using default",
               exec_user, exec_group);
    }

    if (default_exec_uid == -1)
        default_exec_uid = server_uid;

    if (default_exec_gid == -1)
        default_exec_gid = server_gid;

    if (exec_path == NULL)
        exec_path = s_strdup("/bin:/usr/bin");

    /* Set up the state machines for the search. */
    for ( i = 0; i < 256; i++ )
    {
        start_fsm[i] = 5;
        end_fsm[i] = 3;
    }
    start_fsm[(int)('<')] = 4;
    start_fsm[(int)('!')] = 3;
    start_fsm[(int)('-')] = 1;
    end_fsm[(int)('-')] = 1;

    /* Set up lookup tables for escaping shell specials */
    for ( i = 0; i < 256; i++ )
        ch_type[i] = 0;

    ch_type['&'] = ch_type[';'] = ch_type['\''] =
                                      ch_type['`'] = ch_type['"'] = ch_type['|'] =
                                              ch_type['*'] = ch_type['?'] = ch_type['~'] =
                                                      ch_type['<'] = ch_type['>'] = ch_type['^'] =
                                                              ch_type['('] = ch_type[')'] = ch_type['['] =
                                                                      ch_type[']'] = ch_type['{'] = ch_type['}'] =
                                                                              ch_type['$'] = ch_type['\\'] = SHELL_ESCAPE;

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** ncsa-html/pm_exit()\n");

    if (exec_path)
        s_free(exec_path);
}

static int do_parse(struct segment *, struct includeinfo *);

char *unescape_query(char *query)
{
    char ch1, ch2, *buf, *char_pt;
    int len = 0;

    if (query == NULL)
        return NULL;
    if (query[0] == '\0')
        return NULL;

    /* Compute a conservative estimate of the resulting length */
    for ( char_pt = query; (ch1 = *char_pt); char_pt++ )
        if ( isshesc(ch1) )
            len++;

    len += char_pt - query;
    buf = s_malloc(len+1);

    if (buf == NULL) return(NULL);

    for ( char_pt = buf; (ch1 = *query); query++ )
    {
        if ( ch1 == '%' && (ch2=query[1]) && s_isxdigit(ch2))
        {
            query++;
            ch1 = ( s_isdigit(ch2) ?  ch2 - '0' : s_toupper(ch2)-'A' +10);
            if ( (ch2=query[1]) && s_isxdigit(ch2))
            {
                query++;
                ch1 = (ch1 << 4) |
                      (s_isdigit(ch2) ? ch2-'0' : s_toupper(ch2)-'A'+10);
            }
        }
        if ( isshesc(ch1) )
            *char_pt++ = '\\';
        *char_pt++ = ch1;
    }

    return buf;
}

/* This function goes through the attribute pairs and searches for */
/* attributes that change the configuration of the includes. */
/* Currently, these are errmsg, timefmt and sizefmt. The function is */
/* used by all includes. When called from config, it changes the */
/* global configuration, otherwise only the local configuration. */

static void config_include(struct attr_pair *ap, struct includeinfo *ip)
{
    /* Go through the attribute list */
    for ( ; ap != NULL; ap = ap->next )
    {
        /* Skip attributes that have already been dealt with. */
        if ( ap->name == NULL )
            continue;

        /* Check if the attribute is one of the config attributes */
        if ( strcmp(ap->name, "errmsg") == 0 )
        {
            /* errmsg */
            if (ip->errmsg != NULL)
                s_free(ip->errmsg);
            ip->errmsg = ap->value;
            s_free(ap->name);
            ap->name = ap->value = NULL; /* Mark it as being read */
        }
        else if ( strcmp(ap->name, "sizefmt") == 0 )
        {
            /* sizefmt */
            if (ip->sizefmt != NULL)
                s_free(ip->sizefmt);
            ip->sizefmt = ap->value;
            s_free(ap->name);
            ap->name = ap->value = NULL; /* Mark it as being read */
        }
        else if ( strcmp(ap->name, "timefmt") == 0 )
        {
            /* timefmt */
            if (ip->timefmt != NULL)
                s_free(ip->timefmt);
            ip->timefmt = ap->value;
            s_free(ap->name);
            ap->name = ap->value = NULL; /* Mark it as being read */
        }
    }
}

/* Frees global or local config info. */
static void config_include_free(struct includeinfo *ip)
{
    if (ip->timefmt != NULL)
        s_free(ip->timefmt);
    if (ip->sizefmt != NULL)
        s_free(ip->sizefmt);
    if (ip->errmsg  != NULL)
        s_free(ip->errmsg);
    ip->timefmt = ip->sizefmt = ip->errmsg = NULL;
}


/*
** Search the attribute list for a file reference and access it.
**
** Returns -1 in case of error (and then the buffer might point to
** an error), else 0.
*/
static int reference_file(struct attr_pair *ap,
                          struct includeinfo *ip, int flags,
                          char **buffer)
{
    char url_buf[2048], *url_pt, *char_pt;
    fscentry_t *fep = NULL;


    url_buf[0] = '\0';
#if 0
    ip->fep = NULL;
#endif


    /* Go through the attribute list */
    for ( ; ap != NULL; ap = ap->next )
    {
        /* Skip attributes that have already been dealt with. */
        if ( ap->name == NULL )
            continue;

        if ( strcmp(ap->name, "virtual") == 0 )
        {
            s_free(ap->name);
            ap->name = NULL;	/* Mark it as being read */

            if (ap->value == NULL)
            {
                *buffer = "[No URL specified]";
                return -1;
            }

            break;
        }

        if ( strcmp(ap->name, "file") == 0)
        {
            s_free(ap->name);
            ap->name = NULL;	/* Mark it as being read */

            if (ap->value == NULL)
            {
                *buffer = "[No file specified]";
                return -1;
            }

            if ( ap->value[0] == '/' )
            {
                *buffer = "[Path must be relative]";
                return -1;
            }

            char_pt = ap->value;

            if (char_pt[0] == '.' && char_pt[1] == '.' &&
                    (char_pt[2] == '/' || char_pt[2] == '\0'))
            {
                *buffer = "[No '..' allowed in file path]";
                return -1;
            }

            for ( ; char_pt[3]; char_pt++ )
                if (char_pt[0] == '/' && char_pt[1] == '.' &&
                        char_pt[2] == '.' &&
                        (char_pt[3] == '/' || char_pt[3] == '\0'))
                {
                    *buffer = "[No '/..' allowed in file path]";
                    return -1;
                }

            break;
        }
    }


    if ( ap != NULL )
    {
        url_pt = url_buf;

        if ( ap->value[0] != '/' )
        {
            /* Url is relative */

            s_strcpy(url_buf, sizeof(url_buf), ip->cip->hip->url);

            char_pt = strrchr(url_buf, '/');
            if (char_pt)
                char_pt[1] = '\0';
        }

        s_strcat(url_buf, sizeof(url_buf), ap->value);

        fep = fscache_lookup(url_buf, flags|ip->fsc_flags);
        if ( fep == NULL || fep->fip == NULL)
        {
            if (debug > 2)
                fprintf(stderr, "fscache_lookup(\"%s\"): not found\n",
                        url_buf);

            *buffer = "[File not found]";
            return -1;
        }

        ip->fep = fep;
    }

    return 0;
}


/* The following functions are called for server-side includes. They */
/* are passed a pointer to a segment structure which they are supposed */
/* to fill, a pointer to a list of attributes, and a pointer to a */
/* structure with general information. */

static void cmd_config(struct segment *cp, struct attr_pair *ap,
                       struct includeinfo *ip)
{
    config_include(ap, ip);
    cp->block = NULL;
    cp->size = 0;
    cp->free_start = NULL;
    cp->free_size = 0;
}

static void cmd_user(struct segment *cp, struct attr_pair *ap,
                     struct includeinfo *ip)
{
    char buf[1048];
    char buf2[2048];
    struct passwd *pwp, pwb;


    pwp = NULL;
    buf2[0] = '\0';


    cp->block = NULL;
    cp->size = 0;
    cp->free_start = NULL;
    cp->free_size = 0;


    /* Go through the attribute list */
    for ( ; ap != NULL; ap = ap->next )
    {
        /* Skip attributes that have already been dealt with. */
        if ( ap->name == NULL ) continue;

        if ( strcmp(ap->name, "name") == 0 && ap->value != NULL)
        {

            s_free(ap->name);
            ap->name = NULL;	/* Mark it as being read */

            pwp = s_getpwnam_r(ap->value, &pwb, buf, sizeof(buf));
            if (pwp == NULL)
            {
                if (debug > 2)
                    fprintf(stderr, "cgi.c:cmd_user(), user %s not found\n",
                            ap->value);

                s_strcpy(buf2, sizeof(buf2), "[User not found]");
                break;
            }

            s_sprintf(buf2, sizeof(buf2),
                      "<a href=\"/~%s\">%s</a>", ap->value, pwp->pw_gecos);
            cp->free_start = cp->block = s_strdup(buf2);
            cp->size = strlen(buf2);
        }

        else if ( strcmp(ap->name, "mailto") == 0 && ap->value != NULL )
        {
            s_free(ap->name);
            ap->name = NULL;	/* Mark it as being read */

            if (buf2[0])
                s_strcat(buf2, sizeof(buf2), " ");

            s_strcat(buf2, sizeof(buf2), "<a href=\"mailto:");
            if (strchr(ap->value, '@') == NULL)
            {
                if (pwp == NULL)
                {
                    s_strcpy(buf2, sizeof(buf2),
                             "[name must occur before mailto]");
                    break;
                }

                s_strcat(buf2, sizeof(buf2), pwp->pw_name);
                s_strcat(buf2, sizeof(buf2), "@");
            }
            s_strcat(buf2, sizeof(buf2), ap->value);
            s_strcat(buf2, sizeof(buf2), "\">&lt;");
            if (strchr(ap->value, '@') == NULL)
            {
                s_strcat(buf2, sizeof(buf2), pwp->pw_name);
                s_strcat(buf2, sizeof(buf2), "@");
            }
            s_strcat(buf2, sizeof(buf2), ap->value);
            s_strcat(buf2, sizeof(buf2), "&gt;</a>");
        }
        else
        {
            s_strcpy(buf2, sizeof(buf2), "[Unknown tag]");
            break;
        }
    }

    cp->free_start = cp->block = s_strdup(buf2);
    cp->size = strlen(cp->block);
    time(&ip->lastmod);
}

static void cmd_echo(struct segment *cp, struct attr_pair *ap,
                     struct includeinfo *ip)
{
    struct connectioninfo *cip;
    struct httpinfo *hip;
    struct mimeinfo *mip = NULL;
    char *var_name;
    char buf[1025];
    struct tm time_pt;
    struct includeinfo local_ip = {NULL, 0, 0, NULL, 0, 0, NULL, NULL, NULL};
    char *timefmt;

    cip = ip->cip;
    hip = cip->hip;
    if (hip) mip = hip->mip;

    config_include(ap, &local_ip);
    if ( (timefmt = local_ip.timefmt) == NULL )
        timefmt = ip->timefmt;

    cp->block = NULL;
    cp->size = 0;
    cp->free_start = NULL;
    cp->free_size = 0;

    /* Go through the attribute list */
    for ( ; ap != NULL; ap = ap->next )
    {
        /* Skip attributes that have already been dealt with. */
        if ( ap->name == NULL ) continue;

        if ( strcmp(ap->name, "var") == 0 && ap->value != NULL)
        {
            s_free(ap->name);
            ap->name = NULL;	/* Mark it as being read */

            var_name = ap->value;
            if (strcmp(var_name, "DOCUMENT_NAME") == 0)
            {
                if (!(cp->block = strrchr(ip->fep->fip->path, '/')))
                    cp->block = ip->fep->fip->path;
            }
            else if (strcmp(var_name, "DOCUMENT_URI") == 0)
            {
                cp->block = hip->orig_url;
            }
            else if (strcmp(var_name, "DOCUMENT_ROOT") == 0)
            {
                cp->block = server_home;
            }
            else if (strcmp(var_name, "DATE_LOCAL") == 0)
            {
                strftime(buf, sizeof(buf), timefmt,
                         localtime_r(&(cip->cn_time), &time_pt) );
                cp->free_start = cp->block = s_strdup(buf);
            }
            else if (strcmp(var_name, "DATE_GMT") == 0)
            {
                strftime(buf, sizeof(buf), timefmt,
                         gmtime_r(&(cip->cn_time), &time_pt) );
                cp->free_start = cp->block = s_strdup(buf);
            }
            else if (strcmp(var_name, "LAST_MODIFIED") == 0)
            {
                strftime(buf, sizeof(buf), timefmt,
                         localtime_r(&(ip->fep->fip->sb.st_mtime), &time_pt) );
                cp->free_start = cp->block = s_strdup(buf);
            }
            else if (strcmp(var_name, "SERVER_SOFTWARE") == 0)
            {
                s_sprintf(buf, sizeof(buf), "phttpd/%s", server_version);
                cp->free_start = cp->block = s_strdup(buf);
            }
            else if (strcmp(var_name, "SERVER_NAME") == 0)
            {
                cp->block = server_host;
            }
            else if (strcmp(var_name, "GATEWAY_INTERFACE") == 0)
            {
                cp->block = "CGI/1.1";
            }
            else if (strcmp(var_name, "SERVER_PROTOCOL") == 0)
            {
                cp->block = hip->version;
            }
            else if (strcmp(var_name, "SERVER_PORT") == 0)
            {
                s_sprintf(buf, sizeof(buf), "%d", server_port);
                cp->free_start = cp->block = s_strdup(buf);
            }
            else if (strcmp(var_name, "REQUEST_METHOD") == 0)
            {
                cp->block = hip->method;
            }
            else if (strcmp(var_name, "QUERY_STRING") == 0)
            {
                cp->block = hip->request;
            }
            else if (strcmp(var_name, "QUERY_STRING_UNESCAPED") == 0)
            {
                cp->free_start = cp->block = unescape_query(hip->request);
            }
            else if (strcmp(var_name, "LOCAL_ADDR") == 0)
            {
                cp->block = cip->server.addr_s;
            }
            else if (strcmp(var_name, "LOCAL_HOST") == 0)
            {
                cp->block = get_hostname(&cip->server);
            }
            else if (strcmp(var_name, "REMOTE_ADDR") == 0)
            {
                cp->block = cip->client.addr_s;
            }
            else if (strcmp(var_name, "REMOTE_HOST") == 0)
            {
                cp->block = get_hostname(&cip->client);
            }
            else if (mip == NULL)
                continue;
            else if (strcmp(var_name, "REQUEST_METHOD") == 0)
            {
                cp->block = hip->method;
            }
            else if (strcmp(var_name, "CONTENT_TYPE") == 0)
            {
                cp->block = mime_getheader(mip, var_name, 1);
            }
            else if (strcmp(var_name, "REFERER") == 0)
            {
                cp->block = mime_getheader(mip, var_name, 1);
            }
            else if (strcmp(var_name, "CONTENT_LENGTH") == 0)
            {
                cp->block = mime_getheader(mip, var_name, 1);
            }
            else if (strncmp(var_name, "HTTP_", 5) == 0 &&
                     strcmp(var_name, "HTTP_AUTHORIZATION") != 0)
            {
                char *name, *p;
                int nsize;

#if 1
                S_STRALLOCA(var_name+5, &name, 0, &nsize);
#else
                nsize = strlen(var_name+5)+1;
                name = (char *) alloca(nsize);
                s_strcpy(name, nsize, var_name+5);
#endif

                for (p = name; *p; p++)
                    if (*p == '_')
                        *p = '-';

                cp->block = mime_getheader(mip, name, 1);
            }

            if ( cp->block != NULL )
                cp->size = strlen(cp->block);
            break;
        }
    }

    time(&ip->lastmod);
}

static void cmd_include(struct segment *cp, struct attr_pair *ap,
                        struct includeinfo *ip)
{
    fscentry_t *fep, *old_fep;
    fsinfo_t *fip;


    cp->free_start = NULL;
    if ( ip->level >= MAXINCLUDE )
    {
        /* Don't nest too deep. */
        cp->block = NULL;
        cp->size = 0;
        return;
    }

    old_fep = ip->fep;

    if (reference_file(ap, ip, FSCF_GETDATA, &cp->block) < 0)
    {
        /* Failed */
        cp->size = strlen(cp->block);
        return;
    }

    if (ip->fep == NULL)
        fep = old_fep;
    else
        fep = ip->fep;

    fip = fep->fip;

    if (S_ISDIR(fip->sb.st_mode))
    {
        cp->block = "[Can not include a directory]";
        cp->size = strlen(cp->block);

        if (fep != NULL && fep != old_fep)
        {
            ip->fep = old_fep;
            fscache_release(fep);
        }
        return;
    }

    if ( ip->lastmod )
        if ( ip->lastmod < fip->sb.st_mtime )
            ip->lastmod = fip->sb.st_mtime;

    cp->fep = fep;
    cp->block = fip->data.file.content;
    cp->free_size = cp->size = fip->sb.st_size;
    cp->free_start = cp->block;
    madvise(cp->block, fip->sb.st_size, MADV_WILLNEED);
    ip->level++;
    do_parse(cp, ip);
    ip->level--;
    madvise(cp->block, fip->sb.st_size, MADV_SEQUENTIAL);

    ip->fep = old_fep;
}


static void cmd_fsize(struct segment *cp, struct attr_pair *ap,
                      struct includeinfo *ip)
{
    char buf[32];
    off_t fsize = 0;
    struct includeinfo local_ip = {NULL, 0, 0, NULL, 0, 0, NULL, NULL, NULL};
    fscentry_t *fep, *old_fep;
    fsinfo_t *fip;
    char *sizefmt;


    config_include(ap, &local_ip);
    if ( (sizefmt = local_ip.sizefmt) == NULL )
        sizefmt = ip->sizefmt;

    old_fep = ip->fep;

    if (reference_file(ap, ip, 0, &cp->block) < 0)
    {
        /* Failed */
        ip->fep = old_fep;
        cp->size = strlen(cp->block);
        return;
    }

    if (ip->fep == NULL)
        fep = old_fep;
    else
        fep = ip->fep;

    fip = fep->fip;

    fsize = fip->sb.st_size;
    if ( ip->lastmod )
        if ( ip->lastmod < fip->sb.st_mtime )
            ip->lastmod = fip->sb.st_mtime;

    if (fep != NULL && fep != old_fep)
    {
        ip->fep = old_fep;
        fscache_release(fep);
    }

    if (sizefmt != NULL && strcmp(sizefmt,"bytes")==0)
    {
        int len;
        char *char_pt1, *char_pt2;

        s_sprintf(buf, sizeof(buf), "%ld", fsize);
        len = strlen(buf);
        char_pt1 = cp->free_start = cp->block = s_malloc(len+1+(len-1)/3);

        for ( char_pt2 = buf; len >= 0; len-- )
        {
            *char_pt1++ = *char_pt2++;
            if ( len > 1 && !((len-1)%3) ) *char_pt1++ = ',';
        }
    }
    else
    {
        if ( fsize < 10240 )
        {
            s_sprintf(buf, sizeof(buf), "%d", (int)fsize);
        }
        else if ( fsize < 1024*10240 )
        {
            fsize /= 1024;
            s_sprintf(buf, sizeof(buf), "%dK", (int)fsize);
        }
        else
        {
            fsize /= (1024*1024);
            s_sprintf(buf, sizeof(buf), "%dM", (int)fsize);
        }
        cp->free_start = cp->block = s_strdup(buf);
    }
    cp->size = strlen(cp->block);
    cp->free_size = 0;

    config_include_free(&local_ip);
}


static void cmd_flastmod(struct segment *cp, struct attr_pair *ap,
                         struct includeinfo *ip)
{
    char buf[256];
    struct tm time_pt;
    struct includeinfo local_ip = {NULL, 0, 0, NULL, 0, 0, NULL, NULL, NULL};
    fscentry_t *fep, *old_fep;
    fsinfo_t *fip;
    char *timefmt;
    time_t mtime;


    old_fep = ip->fep;

    config_include(ap, &local_ip);
    if ( (timefmt = local_ip.timefmt) == NULL )
        timefmt = ip->timefmt;

    if (reference_file(ap, ip, 0, &cp->block) < 0)
    {
        /* Failed */
        if (debug > 2)
            fprintf(stderr, "cmd_flastmod(): failed: %s\n", cp->block);

        ip->fep = old_fep;
        cp->size = strlen(cp->block);
        return;
    }

    if (ip->fep == NULL)
        fep = old_fep;
    else
        fep = ip->fep;

    fip = fep->fip;

    mtime = fip->sb.st_mtime;
    if ( ip->lastmod )
        if ( ip->lastmod < fip->sb.st_mtime )
            ip->lastmod = fip->sb.st_mtime;

    if (fep != NULL && fep != old_fep)
    {
        ip->fep = old_fep;
        fscache_release(fep);
    }

    strftime(buf, sizeof(buf), timefmt, localtime_r(&(mtime), &time_pt));

    cp->free_start = cp->block = s_strdup(buf);
    cp->size = strlen(buf);
    cp->free_size = 0;

    config_include_free(&local_ip);
}

static char *cgi_setenv(char **envp, const char *var, const char *val)
{
    char *buf;
    int i, len;
    int size;


    if (debug > 5)
        fprintf(stderr, "cgi_setenv: var=%s, val=%s\n",
                var, val ? val : "<null>");

    if (val == NULL)
        return NULL;


    size = strlen(var)+strlen(val)+2;
    buf = s_malloc(size);

    s_strcpy(buf, size, var);
    len = s_strcat(buf, size, "=");
    s_strcat(buf, size, val);

    for (i = 0; i < max_environ && envp[i]; i++)
        if (strncmp(envp[i], buf, len) == 0)
            break;

    if (i >= max_environ)
        return NULL;

    if (envp[i])
        s_free(envp[i]);

    envp[i] = buf;
    return buf;
}


static char *cgi_setenv_i(char **envp, const char *var, int num)
{
    char buf[256];

    s_sprintf(buf, sizeof(buf), "%d", num);
    return cgi_setenv(envp, var, buf);
}


static int cse_header(hashentry_t *hep, void *envp)
{
    char *name, *cp;
    int nsize;


    if (strcmp(hep->key, "AUTHORIZATION") == 0)
        return 0;

    nsize = strlen(hep->key)+6;
    name = (char *) alloca(nsize);
    s_strcpy(name, nsize, "HTTP_");
    s_strcat(name, nsize, hep->key);
    cp = name+5;
    while (*cp)
    {
        if (*cp == '-')
            *cp = '_';
        ++cp;
    }

    cp = cgi_setenv(envp, name, hep->data);

    return 0;
}


static char **cgi_setup_env(const char *method,
                            char *url,
                            struct connectioninfo *cip,
                            struct httpinfo *hip,
                            struct mimeinfo *mip,
                            const char *path,
                            char *path_info,
                            char *path_translated)
{
    char **envp, *buf, *request;
    int blen;


    envp = s_malloc((max_environ+1) * sizeof(char *));
    if (envp == NULL)
        return NULL;
    cgi_setenv(envp, "DOCUMENT_ROOT",
               (exec_newroot == NULL) ? server_home : "/" );
    cgi_setenv(envp, "TZ", tzname[0]);
    cgi_setenv(envp, "PATH", path);

    blen = strlen(server_version) + 8;
    buf = (char *) alloca(blen);
    s_sprintf(buf, blen, "phttpd/%s", server_version);

    cgi_setenv(envp, "SERVER_SOFTWARE", buf);
    cgi_setenv(envp, "SERVER_NAME", server_host);
    cgi_setenv(envp, "GATEWAY_INTERFACE", "CGI/1.1");
    cgi_setenv(envp, "REQUEST_METHOD", method);
    cgi_setenv(envp, "PATH_INFO", path_info);

    cgi_setenv_i(envp, "SERVER_PORT", server_port);

    if (path_translated && path_translated[0])
    {
        if (path_translated[0] != '/')
        {
            int bufsize;

            bufsize = strlen(server_home) + strlen(path_translated) + 2;
            buf = (char *) alloca(bufsize);
            s_strcpy(buf, bufsize, server_home);
            if (strcmp(path_translated, ".") != 0)
            {
                s_strcat(buf, bufsize, "/");
                s_strcat(buf, bufsize, path_translated);
            }

            cgi_setenv(envp, "PATH_TRANSLATED", buf);
        }
        else
            cgi_setenv(envp, "PATH_TRANSLATED", path_translated);
    }

    if (cip)
    {
        cgi_setenv(envp, "LOCAL_ADDR",  cip->server.addr_s);
        cgi_setenv(envp, "LOCAL_HOST", get_hostname(&cip->server));

        cgi_setenv(envp, "REMOTE_ADDR", cip->client.addr_s);
        cgi_setenv(envp, "REMOTE_HOST", get_hostname(&cip->client));
        cgi_setenv(envp, "REMOTE_IDENT", get_ident(&cip->client));

        cgi_setenv(envp, "AUTH_TYPE", cip->auth.type);
        cgi_setenv(envp, "REMOTE_USER", cip->auth.user);
        if (cip->auth.xsetenv)
            (*cip->auth.xsetenv)(cip->auth.xinfo, cgi_setenv, envp);
    }

    if (hip)
    {
        cgi_setenv(envp, "SERVER_PROTOCOL", hip->version);
        cgi_setenv(envp, "SCRIPT_NAME", url+cip->vspl);
        cgi_setenv(envp, "QUERY_STRING", hip->request);
    }

    if (mip)
    {
        cgi_setenv(envp, "CONTENT_TYPE",
                   mime_getheader(mip, "CONTENT-TYPE", 1));

        cgi_setenv(envp, "CONTENT_LENGTH",
                   mime_getheader(mip, "CONTENT-LENGTH", 1));

        ht_foreach(&mip->table, cse_header, (void *) envp);
    }



    /* Non CGI/1.1 conformant extension, but very nice to have... */
    if (hip->request && hip->request[0])
    {
        char **argv;
        int reqsize = strlen(hip->request)+1;
        request = (char *) alloca(reqsize);
        s_strcpy(request, reqsize, hip->request);

        if ((argv = strsplit(request, '&', 0)) != NULL)
        {
            int i;

            for (i = 0; argv[i]; i++)
            {
                char **varval, *var_name;


                if ((varval = strsplit(argv[i], '=', 2)) != NULL)
                {
                    if (debug > 4)
                        fprintf(stderr, "varval[0] = %s, varval[1] = %s\n",
                                varval[0],
                                varval[1]);

                    if (varval[0] && varval[1])
                    {
                        int varsize = strlen(varval[0])+14;
                        var_name = (char *) alloca(varsize);
                        s_strcpy(var_name, varsize, "PHTTPD_QUERY_");
                        s_strcat(var_name, varsize, varval[0]);

                        cgi_setenv(envp, var_name, varval[1]);
                    }

                    s_free(varval);
                }
            }

            s_free(argv);
        }
    }

    return envp;
}


static void cgi_free_env(char **envp)
{
    int i;


    if (envp == NULL)
        return;

    for (i = 0; i < max_environ && envp[i]; i++)
        s_free(envp[i]);

    s_free(envp);
}


static void cmd_exec(struct segment *cp,
                     struct attr_pair *ap,
                     struct includeinfo *ip)
{
    int tmp_fd;


    time(&ip->lastmod);

    cp->free_start = NULL;
    cp->block = NULL;

    if (!exec_enabled)
    {
        cp->block = "[The exec directive is not allowed]";
        cp->size = strlen(cp->block);
        return;
    }

    for ( ; ap != NULL; ap = ap->next)
    {
        if (ap->name == NULL)
            continue;

        if (strcmp(ap->name, "cmd") == 0)
        {
            s_free(ap->name);
            ap->name = NULL;

            if (ap->value == NULL)
            {
                cp->block = "[No program path specified]";
                cp->size = strlen(cp->block);
                break;
            }
            else
            {
                /* Run process, output to temp file, and then
                   mmap it into the block buffer */

                uid_t uid;
                gid_t gid;
                int fd_0, fd_1, fd_2, why, code;
                pid_t pid;
                char *argv[4], **envp, *wdir, *tmps;
                struct stat sb;
                char tbuf[256];
                time_t bt;
                struct tm tp;
                char *path_info = NULL;
                char *path_translated = NULL;


                uid = ip->fep->fip->uid;
                gid = ip->fep->fip->gid;

                if (exec_user_enabled == 0 && uid != -1)
                {
                    cp->block = "[User Exec not allowed]";
                    cp->size = strlen(cp->block);
                    break;
                }

                if (uid == -1)
                    uid = default_exec_uid;

                if (gid == -1)
                    gid = default_exec_gid;

                if ((tmp_fd = s_tmpfile(NULL, 0)) < 0)
                {
                    cp->block = "[Failure creating temporary file]";
                    cp->size = strlen(cp->block);
                    break;
                }

                fd_0 = -1;
                fd_1 = tmp_fd;
                fd_2 = 2;

                argv[0] = "/bin/sh";
                argv[1] = "-c";
                argv[2] = ap->value;
                argv[3] = NULL;

                envp = cgi_setup_env(ip->cip->hip->method,
                                     ip->cip->hip->url,
                                     ip->cip,
                                     ip->cip->hip,
                                     ip->cip->hip->mip,
                                     exec_path,
                                     path_info,
                                     path_translated);

                cgi_setenv(envp, "DOCUMENT_URI",
                           ip->cip->hip->url+ip->cip->vspl);

                cgi_setenv(envp, "DOCUMENT_ROOT", server_home);

                cgi_setenv(envp, "DOCUMENT_NAME", ip->fep->fip->path);
                cgi_setenv(envp, "QUERY_STRING_UNESCAPED",
                           (tmps = unescape_query(ip->cip->hip->request)));
                s_free(tmps);

                localtime_r(&ip->fep->fip->sb.st_mtime, &tp);
                strftime(tbuf, sizeof(tbuf), ip->timefmt, &tp);
                cgi_setenv(envp, "LAST_MODIFIED", tbuf);

                time(&bt);

                localtime_r(&bt, &tp);
                strftime(tbuf, sizeof(tbuf), ip->timefmt, &tp);
                cgi_setenv(envp, "DATE_LOCAL", tbuf);

                gmtime_r(&bt, &tp);
                strftime(tbuf, sizeof(tbuf), ip->timefmt, &tp);
                cgi_setenv(envp, "DATE_GMT", tbuf);

                wdir = NULL;

                pid = proc_run("/bin/sh",
                               uid, gid,
                               exec_newroot,
                               exec_niceval, exec_vmem,
                               exec_fdmax, exec_maxcputime,
                               fd_0, fd_1, fd_2,
                               argv, envp,
                               wdir);
                if (pid < 0)
                {
                    cp->block = "[Error starting program]";
                    cp->size = strlen(cp->block);
                    cgi_free_env(envp);
                    break;
                }

                s_close(fd_1);

                why = proc_wait(pid, &code);
                if (why != PROC_EXIT)
                {
                    cp->block = "[Process exited unexpectedly]";
                    cp->size = strlen(cp->block);
                    cgi_free_env(envp);
                    break;
                }

                cgi_free_env(envp);

                lseek(tmp_fd, 0, SEEK_SET);
                fd_0 = tmp_fd;
                if (fd_0 < 0)
                {
                    cp->block = "[Unable to open temporary file for reading]";
                    cp->size = strlen(cp->block);
                    break;
                }

                if (fstat(fd_0, &sb) < 0)
                {
                    cp->block = "[Unable to stat temporary file]";
                    cp->size = strlen(cp->block);
                    s_close(fd_0);
                    break;
                }

                cp->free_start = cp->block = s_malloc(sb.st_size);
                if ((cp->size = s_read(fd_0, cp->block, sb.st_size)) < 0)
                {
                    s_free(cp->block);
                    cp->free_start = NULL;

                    cp->block = "[Unable to read temporary file]";
                    cp->size = strlen(cp->block);
                    s_close(fd_0);
                    break;
                }

                cp->free_size = 0;

                s_close(fd_0);
            }
        }
        else if (strcmp(ap->name, "cgi") == 0)
        {
            if (ap->value == NULL)
            {
                cp->block = "[No CGI virtual path specified]";
                cp->size = strlen(cp->block);
                break;
            }
            else
            {
                /* Expand virtual path, then run CGI process
                   with output to temp file. Then check for "Location:"
                   (which gets included as an Anchor somehow) and if not
                   mmap it into the block buffer */

                cp->block = "[Error: 'exec cgi' is not implemented]";
                cp->size = strlen(cp->block);
                break;
            }
        }
    }
}


static void cmd_count(struct segment *cp, struct attr_pair *ap,
                      struct includeinfo *ip)
{
    struct count_t count, total;
    char *url, *fmt, ch, *char_pt1, *char_pt2, buf[2049];

    url = ip->cip->hip->orig_url;
    fmt = "hits";

    cp->free_start = cp->block = NULL;

    /* Go through the attribute list */
    for ( ; ap != NULL; ap = ap->next )
    {
        /* Skip attributes that have already been dealt with. */
        if ( ap->name == NULL )
            continue;

        if ( strcmp(ap->name, "virtual") == 0 )
        {
            s_free(ap->name);
            ap->name = NULL;	/* Mark it as being read */

            if (ap->value == NULL)
            {
                cp->block = "[No URL specified]";
                cp->size = strlen(cp->block);
                return;
            };

            char_pt1 = buf;
            if ( ap->value[0] != '/' )
            {
                s_strcpy(buf, sizeof(buf), ip->cip->hip->url);
                char_pt2 = buf;
                while ( (ch = *char_pt2++) != '\0' )
                    if ( ch == '/' )
                        char_pt1 = char_pt2;
                s_strcpy(char_pt1, sizeof(buf)-(char_pt1-buf), ap->value);
                url = buf;
            }
            else
            {
                url = ap->value;
            };
        }
        else if ( strcmp(ap->name, "fmt") == 0 )
        {
            s_free(ap->name);
            ap->name = NULL;	/* Mark it as being read */

            if (ap->value == NULL)
            {
                cp->block = "[No format specified]";
                cp->size = strlen(cp->block);
                return;
            };

            fmt = ap->value;
        };
    }

    get_count(url, &count);
    get_totals(&total);

    if ( strcmp(fmt, "%hits") == 0 )
        s_sprintf(buf, sizeof(buf), "%04.2f",
                  100.0*(float)(count.totalhits+1)/(float)(total.totalhits+1));
    else if ( strcmp(fmt, "%bytes") == 0 )
        s_sprintf(buf, sizeof(buf), "%04.2f",
                  100.0*(float)(count.totalbytes)/(float)(total.totalbytes));
    else if ( strcmp(fmt, "hits") == 0 )
        s_sprintf(buf, sizeof(buf), "%lu",count.totalhits+1);
    else
        s_sprintf(buf, sizeof(buf), "%lu",count.totalbytes);

    cp->free_start = cp->block = s_strdup(buf);
    cp->size = strlen(cp->block);
    cp->free_size = 0;
    time(&ip->lastmod);
}


/* Does the actual parsing. It builds up a linked list of elements */
/* containing the pieces that make up the document. */
static int do_parse(struct segment *cp, struct includeinfo *ip)
{
    struct segment *cp1, *cp2;
    char *char_pt, *end, *start_inc, *start_attr, *end_attr, *attr_pt;
    unsigned char ch, quote;
    int value_found;
    int i;
    size_t max;
    size_t length = 0;
    struct attr_pair *attributes,*ap;


    char_pt = cp->block;
    end = char_pt + cp->size;

    /* Main loop: go through it until there are no more includes */
    do
    {

        /* Search until you either hit "<!--#" or reach the end of the */
        /* file. This is a simple, yet fast algorithm that in a typical */
        /* HTML document only needs to look at between 20% - 25% of the */
        /* characters. */
        char_pt += 4;
        while ( char_pt < end )
        {
            if ( (ch = *char_pt) == '#' )
                if (strncmp(&char_pt[-4],"<!--",4) == 0)
                    break;
            char_pt += start_fsm[(int)ch];
        }

        if ( ++char_pt >= end )
            break; /* Nothing found */

        /* This is where the command name and attribute list starts. */
        start_inc = (start_attr = attr_pt = char_pt) - 5;

        /* Ok, now find the end of the comment. */
        char_pt += 2;
        while ( char_pt < end )
        {
            if ( (ch = *char_pt) == '>' )
                if (char_pt[-2]=='-' && char_pt[-1]=='-')
                    break;
            char_pt += end_fsm[(int)ch];
        };
        if ( ++char_pt >= end )
            break; /* No end found */

        /* The end of the attribute list is 3 characters earlier */
        end_attr = char_pt - 3;

        /* Skip white space */
        while ( attr_pt < end_attr && s_isspace(*attr_pt) )
            attr_pt++;
        if ( attr_pt >= end_attr )
            continue; /* No command name */

        /* Find the command name */
        max = end_attr - attr_pt;
        for ( i = 0; commands[i].name != NULL; i++ )
        {
            length = strlen(commands[i].name);
            if ( length <= max &&
                    strncasecmp(commands[i].name, attr_pt, length)==0 &&
                    (s_isspace(attr_pt[length]) || length==max) )
                break;
        };

        /* If no valid command, search for next include */
        if ( commands[i].name == NULL )
            continue;

        /* There was a valid command. Now we have to include stuff. */
        if ( debug > 2 )
            fprintf(stderr, "Server-side include: %s\n", commands[i].name);

        /* First, check to see if we have to look for attributes. */
        attributes = ap = NULL;
        if ( commands[i].has_attributes)
        {

            /* Parse attributes */
            attr_pt += length;	/* skip command name */
            while ( attr_pt < end_attr )
            {
                /* Skip white space */
                while ( s_isspace(*attr_pt) )
                    attr_pt++;
                if ( attr_pt >= end_attr )
                    break;

                /* Read in attribute name */
                ch = *attr_pt;
                start_attr = attr_pt;
                while ( attr_pt < end_attr && !s_isspace(ch) && ch != '=' )
                    ch = *++attr_pt;
                length = attr_pt - start_attr;
                if ( length <= 0 ) break;

                /* Start another name=value pair */
                if ( attributes == NULL )
                {
                    ap = s_malloc(sizeof(struct attr_pair));
                    attributes = ap;
                }
                else
                {
                    ap->next=s_malloc(sizeof(struct attr_pair));
                    ap = ap->next;
                }

                ap->next = NULL;
                ap->name = s_malloc(length+1);
                s_strncpy(ap->name, length+1, start_attr, length);
                ap->name[length] = '\0';
                ap->value = NULL;

                /* Is there an attribute value? */
                value_found = 0;
                while ( s_isspace(ch) || ch == '=' )
                {
                    if ( ch == '=' ) value_found = 1;
                    ch = *++attr_pt;
                }

                if ( attr_pt >= end_attr )
                    break;

                /* Is there a value to this attribute? */
                if ( value_found )
                    /* Is it quoted? */
                    if ( ch == '"' || ch == '\'' )
                    {
                        /* Yep, is quoted */
                        quote = ch;
                        ch = *++attr_pt;

                        /* Read in attribute value */
                        start_attr = attr_pt;
                        while ( attr_pt < end_attr && ch != quote )
                            ch = *++attr_pt;
                        length = attr_pt - start_attr;
                        ap->value = s_malloc(length+1);
                        s_strncpy(ap->value, length+1, start_attr, length);
                        ap->value[length] = '\0';

                        /* Skip over closing quote. */
                        attr_pt++;
                    }
                    else
                    {
                        /* Not quoted */

                        /* Read in attribute value */
                        start_attr = attr_pt;
                        while ( attr_pt < end_attr && !s_isspace(ch) )
                            ch = *++attr_pt;
                        length = attr_pt - start_attr;
                        ap->value = s_malloc(length+1);
                        s_strncpy(ap->value, length+1, start_attr, length);
                        ap->value[length] = '\0';
                    }		/* if (ch=='"') */
            }			/* while (attr_pt<end_attr) */

            if ( attributes && debug > 3 )
            {
                fprintf(stderr, "  attributes:\n");
                for ( ap = attributes; ap != NULL; ap = ap->next )
                    fprintf(stderr, "    %s = %s\n",
                            ap->name,
                            ap->value ? ap->value : "<null>");
            }
        }			/* if (has_attributes) */

        /* Cut up old segment in two. */
        cp2 = s_malloc(sizeof(struct segment));

        cp2->next = NULL;
        cp2->block = char_pt;
        cp2->size = end - char_pt;
        cp2->free_start = cp->free_start;
        cp2->free_size  = cp->free_size;
        cp2->fep        = cp->fep;
        cp->next = cp2;
        cp->size = start_inc - cp->block;
        cp->free_start = NULL;
        cp->free_size = 0;
        cp->fep = NULL;
        if ( ip->length )
            ip->length -= (char_pt - start_inc);

        /* Create a new segment and fill it with text. */
        cp2 = s_malloc(sizeof(struct segment));
        cp2->next = NULL;
        cp2->fep  = NULL;

        commands[i].function(cp2, attributes, ip);

        /* If not empty, insert the segment(s). */
        if ( cp2->block != NULL )
        {
            cp1 = cp->next;
            cp->next = cp2;
            while ( cp->next != NULL)
                cp = cp->next;
            cp->next = cp1;
        }

        cp = cp->next;
        if ( ip->length )
            ip->length += cp2->size;

        /* Free up space occupied by attributes */
        while ( attributes != NULL )
        {
            ap = attributes;
            attributes = attributes->next;
            if ( ap->name  != NULL )
                s_free(ap->name);
            if ( ap->value != NULL )
                s_free(ap->value);
            s_free(ap);
        }

    }
    while (char_pt < end);

    return 0;
}

static int parse_file(struct segment *cp, struct includeinfo *ip)
{
    int size;

    ip->level = 0;
    ip->timefmt = s_strdup(DEFAULT_TIMEFMT);
    ip->sizefmt = NULL;
    ip->errmsg = s_strdup("[an error occurred while processing this directive]");

    size = do_parse(cp, ip);

    config_include_free(ip);

    return size;
}

static int send_segment(int fd, struct segment *cp, int send_out)
{
    int bytes_sent = 0;

    /* Do we have to send anything at all? */
    if (send_out)
    {
        if (cp->block != NULL)
            if (fd_write(fd, cp->block, cp->size) < 0)
                bytes_sent = -1;
            else
                bytes_sent = cp->size;
    }

    /* Free the occupied space and close any open files */
    if ( cp->free_start != NULL && cp->fep == NULL )
        s_free(cp->free_start);

    if (cp->fep != NULL)
    {
        fscache_release(cp->fep);
    }

    return bytes_sent;
}


static int http_get_head(struct connectioninfo *cip)
{
    char *s_since;
#if 0
    char buf[256];
    struct tm tm_since;
    int t_since;
    int i;
#endif
    struct stat sb;
    int result;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;
    fscentry_t *fep;
    fsinfo_t *fip;
    int send_out = 1;
    int bytes_sent = 0;
    struct segment contents = {NULL, NULL, 0, NULL, 0};
    struct segment *cp1, *cp2;
    struct includeinfo parse_config;
    unsigned int fsc_flags = 0;


    if (debug > 1)
        fprintf(stderr, "*** ncsa-html/pm_get() called ***\n");


    if (hip && hip->mip && (hip->mip->pragma_flags & MPF_NOCACHE))
        fsc_flags = FSCF_RELOAD;

    /* Look up information about this document */
    fep = fscache_lookup(hip->url, fsc_flags|FSCF_GETDATA);

    if (fep == NULL)
        return -1;

    if (fep->fip == NULL)
    {
        fscache_release(fep);
        return -1;
    }

    fip = fep->fip;

    parse_config.cip = cip;
    parse_config.fep = fep;
    parse_config.lastmod = fip->sb.st_mtime;
    parse_config.length = fip->sb.st_size;
    parse_config.fsc_flags = fsc_flags;

    result = 200;

    s_since = mime_getheader(hip->mip, "IF-MODIFIED-SINCE", 1);

    hip->length = 0;

    if (strcasecmp(hip->method, "HEAD") == 0)
    {
        send_out = 0;
    }

    if (debug > 2)
        fprintf(stderr, "Setting up contents structure for file\n");

    /* Setup contents structure for file */
    contents.block = fip->data.file.content;

    contents.next = NULL;
    contents.free_size = contents.size = fip->sb.st_size;
    contents.free_start = contents.block;
    contents.fep = fep;
    cp1 = &contents;

    if (contents.block == (char *) MAP_FAILED)
    {
        if (debug)
        {
            fprintf(stderr, "ERROR on #%d: ", cip->request_no);
            perror("mmap");
        }
        result = 500;
        if ( hip->mip != NULL )
            http_error(cip, result, "Failed to map file into memory");
    }
    else
    {
        /* Parse file if necessary. This may split up the file into */
        /* several segments. */
        madvise(contents.block, contents.size, MADV_WILLNEED);

        parse_file(cp1, &parse_config);

        madvise(contents.block, sb.st_size, MADV_SEQUENTIAL);

        if (hip->mip != NULL)
        {
#if 0
            if (s_since && parse_config.lastmod)
            {
                t_since = atotm(s_since, &tm_since);
                if (t_since != -1 )
                {
                    if (fip->sb.st_mtime <= (time_t) t_since)
                    {
                        result = 304;
                        send_out = 0;
                    }
                }
                else
                {
                    /* Could not parse the date format - do a string compare */
                    http_time_r(&parse_config.lastmod, buf, sizeof(buf));
                    i = strlen(buf);
                    buf[i-1] = '\0';

                    if (strcmp(s_since, buf) == 0)
                    {
                        result = 304;
                        send_out = 0;
                    }
                }
            }
#endif

            http_sendheaders(fd, cip, result, NULL);
            if (send_out)
            {
                if (parse_config.lastmod)
                    http_sendlastmodified(fd, parse_config.lastmod);
                if (parse_config.length)
                    fd_printf(fd, "Content-Length: %d\n", parse_config.length);
                fd_puts("Content-Type: text/html\n", fd);
                http_sendlang(fd, hip->url);
            }
            fd_putc('\n', fd);
        }

        /* Now go through segment by segment and send it out, freeing */
        /* up the memory space or unmapping files if necessary. */
        do
        {
            if ((bytes_sent = send_segment(fd, cp1, send_out)) < 0)
            {
                if (debug)
                {
                    fprintf(stderr, "ERROR on #%d: ", cip->request_no);
                    (bytes_sent == -1 ? perror("write") : perror("close"));
                }
            }
            else
                hip->length += bytes_sent;

            cp2 = cp1->next;
            if ( cp1 != &contents )
                s_free(cp1);
            cp1 = cp2;
        }
        while (cp1 != NULL);

    }

    if (debug > 2)
        fprintf(stderr, "ncsa-html/http_get: Returning\n");

    return result;
}


int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;

    if (strcasecmp(hip->method, "GET") == 0 ||
            strcasecmp(hip->method, "HEAD") == 0)
        return http_get_head(cip);
    else
        return -2;
}
