/*
** ph.c
**
** Copyright (c) 1994,1995 Peter Eriksson <pen@signum.se>
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

#define IFM_SPECIAL

#include <stdio.h>
#include <alloca.h>
#include <time.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#include "phttpd.h"


static char *default_page = NULL;
static char *default_server = NULL;
static struct table *ph_labels = NULL;
static int n_labels = 0;
static char *include_header_file = NULL;
static char *include_footer_file = NULL;

#ifdef IFM_SPECIAL
static char *email_href_path = NULL;
static hashtable_t email_href_table;
#endif

static int phl_create(char *cmd,
                      char *line,
                      void *var)
{
    char **pair;


    pair = s_malloc(2* sizeof(char *));

    pair[0] = s_strdup(cmd);
    pair[1] = s_strdup(line);

    * (char ***) var = pair;

    return 1;
}


static void phl_free(void *var)
{
    char **pair;


    pair = (char **) var;
    if (pair[0])
        s_free(pair[0]);
    if (pair[1])
        s_free(pair[1]);

    s_free(pair);
}


static struct options ph_cfg_table[] =
{
    { "default-page",     T_STRING, &default_page,       NULL },
    { "default-server",   T_STRING, &default_server,     NULL },
    { "footer-file",	  T_STRING, &include_footer_file, NULL },
    { "header-file",	  T_STRING, &include_header_file, NULL },

#ifdef IFM_SPECIAL
    { "email-href-db",    T_STRING, &email_href_path,    NULL },
#endif

    { "ph-labels",        T_TABLE,  &ph_labels,          phl_create },
    { NULL,               -1,       NULL,                NULL }
};

void load_href_db(char *path,
                  hashtable_t *tp)
{
    int fd;
    char buf[1024];


    ht_init(tp, 0, NULL);

    fd = fd_open(path, O_RDONLY);
    if (fd < 0)
        return;

    while (fd_gets(buf, sizeof(buf), fd))
    {
        char *cp, *key, *value;


        cp = buf;
        key = value = NULL;

        while (*cp && s_isspace(*cp))
            ++cp;

        if (*cp == '#' || *cp == 0)
            continue;

        key = cp;
        while (*cp && !s_isspace(*cp))
            ++cp;

        if (*cp == 0)
            continue;

        *cp++ = '\0';

        while (*cp && s_isspace(*cp))
            ++cp;

        value = cp;

        cp = value + strlen(value)-1;
        while (cp > value && s_isspace(*cp))
            --cp;
        *++cp = '\0';

        if (debug > 2)
            fprintf(stderr, "Inserting key=%s, value=%s\n", key, value);

        ht_insert(tp, key, 0, s_strdup(value), 0, s_free);
    }

    fd_close(fd);
}


int pm_init(const char **argv)
{
    char *cfg_path, *cp;
    const char *name;
    int cfg_size;


    name = argv[0];

    if (debug > 1)
        fprintf(stderr, "*** libph/pm_init(\"%s\") called ***\n", name);

    cfg_path = s_strxdup(name, 6, &cfg_size);

    cp = strrchr(cfg_path, '.');
    if (cp && strcmp(cp, ".so") == 0)
        *cp = '\0';

    s_strcat(cfg_path, cfg_size, ".conf");
    if (config_parse_file(cfg_path, ph_cfg_table, 0) < 0)
        return -1;

    if (config_parse_argv(argv+1, ph_cfg_table) < 0)
        return -1;

    if (ph_labels)
        n_labels = ph_labels->length;

    if (email_href_path)
        load_href_db(email_href_path, &email_href_table);

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** libph/pm_exit() called ***\n");

    if (default_page)
        s_free(default_page);

    if (default_server)
        s_free(default_server);

    if (ph_labels)
        tbl_free(ph_labels, phl_free);

    if (email_href_path)
    {
        ht_destroy(&email_href_table);
        s_free(email_href_path);
    }
}


static void send_head(int fd,
                      char *header_type,
                      char *title)
{
    fscentry_t *fep = NULL;
    fsinfo_t *fip = NULL;


    if (include_header_file)
    {
        fep = fscache_lookup(include_header_file, FSCF_GETDATA);
        if (fep != NULL && (fip = fep->fip) != NULL)
        {
            fd_write(fd, fip->data.file.content, fip->sb.st_size);
        }

        fscache_release(fep);
    }

    if (fip == NULL)
    {
        fd_puts("<HTML>\n", fd);
        fd_puts("<HEAD>\n", fd);
        fd_printf(fd, "<TITLE>Query Results</TITLE>\n");
        fd_puts("</HEAD>\n", fd);
        fd_puts("<BODY>\n", fd);
    }

    fd_printf(fd, "<%s>%s</%s>\n", header_type, title, header_type);
}


static void send_tail(int fd)
{
    fscentry_t *fep = NULL;
    fsinfo_t *fip = NULL;


    if (include_footer_file)
    {
        fep = fscache_lookup(include_footer_file, FSCF_GETDATA);
        if (fep != NULL && (fip = fep->fip) != NULL)
        {
            fd_write(fd, fip->data.file.content, fip->sb.st_size);
        }

        fscache_release(fep);
    }

    if (fip == NULL)
        html_sysfooter(fd);

    fd_puts("</BODY>\n", fd);
    fd_puts("</HTML>\n", fd);
}


static int error(int fd,
                 char *header,
                 char *msg,
                 ...)
{
    va_list ap;


    va_start(ap, msg);

    send_head(fd, "H2", header);
    fd_vprintf(fd, msg, ap);

    send_tail(fd);
    va_end(ap);

    return 200;
}



static int get_idx(char *label, char **prompt)
{
    int i;

    for (i = 0; i < n_labels; i++)
    {
        char **pair = ph_labels->value[i];

        if (strcmp(label, pair[0]) == 0)
        {
            if (prompt)
                *prompt = pair[1];
            return i;
        }
    }

    return -1;
}

struct resvinfo
{
    int len;
    char **resv;
};

static struct table *get_results(int fd)
{
    int code, num, pos, nidx;
    struct table *tp;
    char buf[2048], label[2048], value[2048];
    struct resvinfo *rp;


    tp = tbl_create(0);

    pos = 0;
    rp = NULL;

    while (fd_gets(buf, sizeof(buf), fd))
    {
        if (sscanf(buf, "%d : %d : %[^ \t:] : %[^\n\r]",
                   &code, &num, label, value) == 4)
        {
            if (code != -200)
                break;

            if (pos != num)
            {
                if (rp != NULL)
                    tbl_append(tp, rp);

                rp = s_malloc(sizeof(struct resvinfo));

                rp->len = n_labels;
                rp->resv = s_malloc(rp->len * sizeof(char *));

                pos = num;
            }

            nidx = get_idx(label, NULL);
            if (nidx < 0)
            {
                char *cp, **pair;

                pair = s_malloc(2 * sizeof(char *));

                pair[0] = s_strdup(label);
                pair[1] = s_strdup(label);

                cp = pair[1];
                if (s_islower(*cp))
                    *cp = s_toupper(*cp);

                while (*cp)
                {
                    if (*cp == '_')
                        *cp = ' ';
                    ++cp;
                }

                nidx = n_labels;

                if (ph_labels == NULL)
                    ph_labels = tbl_create(0);

                n_labels = rp->len = tbl_append(ph_labels, pair);
                rp->resv = s_realloc(rp->resv,
                                     rp->len * sizeof(char *));

            }

            rp->resv[nidx] = s_strdup(value);
        }
    }

    if (rp != NULL)
        tbl_append(tp, rp);

    return tp;
}

static void rp_free(void *val)
{
    struct resvinfo *rip;
    int i;


    rip = (struct resvinfo *) val;
    for (i = 0; i < rip->len; i++)
        if (rip->resv[i])
            s_free(rip->resv[i]);
    s_free(rip);
}


static int find_label(char *label)
{
    int i;


    for (i = 0; i < n_labels; i++)
    {
        char **pair = ph_labels->value[i];

        if (strcmp(pair[0], label) == 0)
            return i;
    }

    return -1;
}


static char *get_label(int idx)
{
    if (idx >= 0 && idx < n_labels)
    {
        char **pair = ph_labels->value[idx];

        return pair[0];
    }

    return NULL;
}


static int get_url_pointer(char *email,
                           char *buf,
                           int size)
{
    if (email_href_path)
    {
        hashentry_t *hep;


        hep = ht_lookup(&email_href_table, email, 0);
        if (hep == NULL)
            return 0;

        s_strcpy(buf, size, hep->data);
        ht_release(hep);
        return 1;
    }
    else
    {
        if (strcmp(email, "peter@ifm.liu.se") == 0)
        {
            s_strcpy(buf, size, "http://www.ifm.liu.se/~peter/");
            return 1;
        }
        else if (strcmp(email, "johan@lysator.liu.se") == 0)
        {
            s_strcpy(buf, size, "http://www.lysator.liu.se/~johan/");
            return 1;
        }
    }

    return 0;
}

static int get_href(struct resvinfo *rip,
                    int idx,
                    char *value,
                    char *qvalue,
                    char *buf,
                    int size)
{
    char *label;


    label = get_label(idx);
    if (label == NULL)
        return 0;

    if (strcmp(label, "email") == 0)
    {
        s_strcpy(buf, size, "mailto:");
        s_strcat(buf, size, qvalue);

        return 1;
    }
#ifdef IFM_SPECIAL
    else if (strcmp(label, "name") == 0)
    {
        int email_idx;
        char *email;


        email_idx = find_label("email");
        if (email_idx >= 0 && email_idx < rip->len &&
                (email = rip->resv[email_idx]) != NULL)
        {
            if (get_url_pointer(email, buf, size))
                return 1;
        }
    }

    else if (strcmp(label, "department") == 0)
    {
        if (strncmp(value, "IFM", 3) == 0)
        {
            s_strcpy(buf, size, "http://www.ifm.liu.se/");
            return 1;
        }
        else if (strncmp(value, "ISY", 3) == 0)
        {
            s_strcpy(buf, size, "http://www.isy.liu.se/");
            return 1;
        }
        else if (strncmp(value, "IDA", 3) == 0)
        {
            s_strcpy(buf, size, "http://www.ida.liu.se/");
            return 1;
        }
        else if (strncmp(value, "IKP", 3) == 0)
        {
            s_strcpy(buf, size, "http://www.ikp.liu.se/");
            return 1;
        }
        else if (strncmp(value, "MAI", 3) == 0)
        {
            s_strcpy(buf, size, "http://www.math.liu.se/");
            return 1;
        }
    }
#endif

    return 0;
}


static void send_results(int fd,
                         struct table *tp)
{
    int i, j, len;
    int *field_width;
    char buf[1024], buf2[1024];


    field_width = (int *) alloca(n_labels * sizeof(int));
    if (field_width == NULL)
        s_abort();

    for (i = 0; i < n_labels; i++)
        field_width[i] = 0;

    for (i = 0; i < tp->length; i++)
    {
        struct resvinfo *rip = tp->value[i];

        for (j = 0; j < rip->len; j++)
            if (rip->resv[j])
            {
                if ((len = strlen(rip->resv[j])) > field_width[j])
                    field_width[j] = len;
            }
    }

    for (i = 0; i < n_labels; i++)
    {
        char **pair = ph_labels->value[i];
        int len = strlen(pair[1]);


        if (field_width[i] > 0 && len > field_width[i])
            field_width[i] = len;
    }

    fd_puts("<PRE><TABLE CELLSPACING=3 WIDTH=100%>\n", fd);
    fd_puts("<TR>", fd);

    for (i = 0; i < n_labels; i++)
    {
        char **pair = ph_labels->value[i];

        if (field_width[i] == 0)
            continue;

        html_strquote(pair[1], buf, sizeof(buf));

        fd_printf(fd, " <TH ALIGN=LEFT> %-*s", field_width[i], buf);
    }

    fd_printf(fd, "\n<TR> <TD COLSPAN=%d> <HR>\n", n_labels);

    for (i = 0; i < tp->length; i++)
    {
        struct resvinfo *rip = tp->value[i];

        fd_puts("<TR>", fd);
        for (j = 0; j < rip->len; j++)
        {
            if (field_width[j] == 0)
                continue;

            fd_puts(" <TD> ", fd);

            if (rip->resv[j] != NULL)
            {
                int have_anchor = 0;


                len = strlen(rip->resv[j]);

                html_strquote(rip->resv[j], buf, sizeof(buf));

                have_anchor = get_href(rip, j, rip->resv[j], buf,
                                       buf2, sizeof(buf2));
                if (have_anchor < 0)
                    return;

                if (have_anchor)
                    fd_printf(fd, "<A HREF=\"%s\">", buf2);

                fd_puts(buf, fd);

                if (have_anchor)
                    fd_puts("</A>", fd);

                if (field_width[j]-len > 0)
                    fd_printf(fd, "%-*s",
                              field_width[j]-len,
                              "");
            }
            else
                fd_printf(fd, "%-*s",
                          field_width[j], "");
        }

        fd_putc('\n', fd);
    }

    fd_printf(fd, "\n<TR> <TD COLSPAN=%d> <HR>\n", n_labels);
    fd_puts("</TABLE></PRE>\n", fd);
}

static void get_list(int fd, int ph_fd)
{
    struct table *tp;


    tp = get_results(ph_fd);
    send_results(fd, tp);
    tbl_free(tp, rp_free);
}

static int http_get(struct connectioninfo *cip)
{
    char buf[1024], *cp, *cp2, *code_s, buf2[256], **argv, *request;
    int code, fd_length, sock_fd, result, i, len;


    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;



    if (debug > 1)
        fprintf(stderr, "*** libph/http_get() called ***\n");

    result = 200;
    sock_fd = -1;
    argv = NULL;

    if (hip->request)
    {
        int reqsize = strlen(hip->request)+1;

        request = (char *) alloca(reqsize);
        s_strcpy(request, reqsize, hip->request);
    }
    else
        request = NULL;


    if (request == NULL ||
            request[0] == '\0' ||
            (argv = strsplit(request, '&', 2)) == NULL)
    {
        if (default_page)
            return http_redirect(cip, default_page, NULL, NULL, 302);
        else
        {
            if (hip->mip != NULL)
            {
                http_sendheaders(fd, cip, result, NULL);

                fd_puts("Content-Type: text/html\n", fd);
                fd_putc('\n', fd);
            }

            fd_length = fd_written(fd);

            result = error(fd, "Error", "No question asked");
            goto End;
        }
    }

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);

        fd_puts("Content-Type: text/html\n", fd);
        fd_putc('\n', fd);
    }

    fd_length = fd_written(fd);

    cp = strchr(hip->url+1, '/');
    if (cp == NULL)
        cp = default_server;
    else
        ++cp;

    if (cp == NULL || *cp == '\0')
    {
        result = error(fd, "Error", "No PH server specified");
        goto End;
    }

    cp2 = strchr(cp, ':');
    if (cp2 == NULL)
        cp2 = "105";
    else
        *cp2++ = '\0';

    if (debug > 4)
        fprintf(stderr, "Before fd_sconnect(\"%s\", \"%s\")\n",
                cp, cp2);

    sock_fd = fd_sconnect(cp, cp2);
    if (sock_fd < 0)
    {
        result = error(fd, "Error", "Can not connect to NS service at %s", cp);
        goto End;
    }

    if (debug > 4)
        fprintf(stderr, "\tGot fd#%d\n", sock_fd);

    fd_printf(sock_fd, "query");
    for (i = 0; argv[i]; i++)
    {
        char *cp, *last, *obj;

        cp = strchr(argv[i], '=');
        if (cp == NULL || cp[1] == '\0')
            continue;
#if 0
        fd_printf(sock_fd, " %s", argv[i]);
#else
        *cp++ = '\0';

        obj = strtok_r(cp, " \t", &last);
        while (obj)
        {
            fd_printf(sock_fd, " %s=%s", argv[i], obj);
            obj = strtok_r(NULL, " \t", &last);
        }
#endif
    }

    fd_putc('\n', sock_fd);
    fd_flush(sock_fd);

    fd_puts("quit\n", sock_fd);
    fd_shutdown(sock_fd, 1);

    if (debug > 4)
        fprintf(stderr, "Sent reply, waiting for answer\n");

    if (fd_gets(buf, sizeof(buf), sock_fd) == NULL)
    {
        result = error(fd, "Error", "Premature end of transmission");
        goto End;
    }

    if (debug > 4)
        fprintf(stderr, "\t->Got one line\n");

    cp = buf;
    code_s = strtok_r(buf, ":", &cp);
    if (code_s == NULL)
    {
        result = error(fd, "Error", "protocol error: <PRE>%s</PRE>", buf);
        goto End;
    }

    code = atoi(code_s);
    switch (code)
    {
    case 102:
        send_head(fd, "H2", "Query Results");
        get_list(fd, sock_fd);
        send_tail(fd);
        break;

    default:
        len = s_strcpy(buf2, sizeof(buf2), "Error #");
        s_sprintf(buf2+len, sizeof(buf2)-len, "%d", code);
        result = error(fd, buf2, cp);
    }

End:
    if (debug > 4)
        fprintf(stderr, "All done, beginnig cleanup\n");

    if (argv)
        s_free(argv);

    if (sock_fd >= 0)
        fd_close(sock_fd);

    if (debug > 4)
        fprintf(stderr, "Exiting.\n");

    return result;
}



int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;

    if (strcasecmp(hip->method, "GET") == 0)
        return http_get(cip);
    else
        return -1;
}
