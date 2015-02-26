/*
** isproxy.c
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
#include <stdlib.h>
#include <string.h>

#include "phttpd.h"
#include <netinet/in.h>


static struct table *proxy_table = NULL;


struct proxy_config
{
    char *url;

    char *host;
    char *port;
    char *pattern;

    struct sockaddr_in sin;
};


static int str2proxy_config(char *cmd,
                            char *line,
                            void *var)
{
    struct proxy_config *pcp;
    char *cp, *url, *host, *port, *pattern;


    if (debug)
        fprintf(stderr, "str2proxy_config(\"%s\")\n", line);

    pcp = s_malloc(sizeof(struct proxy_config));

    url = cmd;
    if (!url)
        goto Fail;

    host = strtok_r(line, ":", &cp);
    if (!host)
        goto Fail;

    port = strtok_r(NULL, " \t", &cp);
    if (!port)
        goto Fail;

    pattern = strtok_r(NULL, " \t", &cp);

    pcp->url = s_strdup(url);
    pcp->host = s_strdup(host);
    pcp->port = s_strdup(port);
    pcp->pattern = s_strdup(pattern);

    if (fd_mksockaddr_in(pcp->host, pcp->port, &pcp->sin) < 0)
        goto Fail;

    * (struct proxy_config **) var = pcp;

    if (debug)
        fprintf(stderr, "\t-> %s %s %s %s\n",
                pcp->url, pcp->host, pcp->port,
                pcp->pattern ? pcp->pattern : "<null>");

    return 1;

Fail:
    if (debug)
        fprintf(stderr, "\t-> Error\n");

    if (pcp->url)
        s_free(pcp->url);

    if (pcp->host)
        s_free(pcp->host);

    if (pcp->port)
        s_free(pcp->port);

    if (pcp->pattern)
        s_free(pcp->pattern);

    s_free(pcp);
    return 0;

}


char *connect_error_page = NULL;

static struct options isproxy_cfg_table[] =
{
    { "connect-error-page",  T_STRING, &connect_error_page, NULL  },

    { "proxy-table",         T_TABLE, &proxy_table, str2proxy_config },

    { NULL,                  -1,       NULL,    NULL }
};


int pm_init(const char **argv)
{
    const char *name = argv[0];
    char *cfg_path, *cp;
    int cfgsize;


    if (debug > 1)
        fprintf(stderr, "*** isproxy/pm_init(\"%s\") called ***\n", name);

    cfgsize = strlen(name)+6;
    cfg_path = s_malloc(cfgsize);
    s_strcpy(cfg_path, cfgsize, name);

    cp = strrchr(cfg_path, '.');
    if (cp && strcmp(cp, ".so") == 0)
        *cp = '\0';

    s_strcat(cfg_path, cfgsize, ".conf");

    if (config_parse_file(cfg_path, isproxy_cfg_table, 0) < 0)
        return -1;

    if (config_parse_argv(argv+1, isproxy_cfg_table) < 0)
        return -1;

    return 0;
}


static void proxy_free(void *p)
{
    struct proxy_config *pcp = (struct proxy_config *) p;


    if (pcp->url)
        s_free(pcp->url);
    if (pcp->host)
        s_free(pcp->host);
    if (pcp->pattern)
        s_free(pcp->pattern);
    s_free(pcp);
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** isproxy/pm_exit() called ***\n");

    if (proxy_table)
        tbl_free(proxy_table, proxy_free);

    if (connect_error_page)
        s_free(connect_error_page);
}



static int send_page(struct connectioninfo *cip, char *page)
{
    return http_redirect(cip, page, NULL, NULL, 302);
}


static void write_data(int fd,
                       struct httpinfo *hip,
                       char *url,
                       char *client_addr)
{
    char buf[2048];


    fd_printf(fd, "%s %s%s%s%s%s\n",
              hip->method,
              url_quote(url, buf, sizeof(buf), "?", 0),
              hip->request == NULL ? "" : "?",
              hip->request == NULL ? "" : hip->request,
              hip->version == NULL ? "" : " ",
              hip->version == NULL ? "" : hip->version);

    fd_flush(fd);

    if (hip->mip)
    {
        fd_printf(fd, "Forwarded: by %s/ for %s\n",
                  server_url, client_addr);

        mime_writeheaders(fd, hip->mip);

        fd_putc('\n', fd);
    }

    fd_flush(fd);
}


#define STATUS_RELAYED 200
struct ctinfo
{
    char *url;
    struct proxy_config *result;
};


static int check_table(void *value, void *misc)
{
    struct proxy_config *pcp;
    struct ctinfo *ctp;
    int len;


    pcp = (struct proxy_config *) value;
    ctp = (struct ctinfo *) misc;


    if (debug)
        fprintf(stderr, "Checking %s vs %s\n", pcp->url, ctp->url);

    len = strlen(pcp->url);
    if (len > (int) strlen(ctp->url))
        return 0;

    if (strncmp(pcp->url, ctp->url, len) == 0)
    {
        ctp->result = pcp;
        return 1;
    }

    return 0;
}


int pm_request(struct connectioninfo *cip)
{
    int sock_fd, i;
    char url_buf[2048];
    struct ctinfo ctb;
    struct proxy_config *pcp;

    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
    {
        fprintf(stderr, "*** isproxy/pm_request() called ***\n");
        fprintf(stderr, "\thip->method = %s\n", hip->method);
        fprintf(stderr, "\thip->url = %s\n", hip->url);
    }


    i = 0;

    if (proxy_table == NULL)
        return 0;

    ctb.url = hip->url;
    ctb.result = NULL;

    tbl_foreach(proxy_table, check_table, &ctb);

    if (ctb.result == NULL)
        return 0;

    pcp = ctb.result;

    if (pcp->pattern)
    {
        if (url_rewrite(hip->url,
                        pcp->pattern,
                        url_buf,
                        sizeof(url_buf)) == NULL)
            return error_not_found(cip);
    }
    else
        s_strcpy(url_buf, sizeof(url_buf), hip->url);

    if (debug > 2)
        fprintf(stderr, "Proxy Connection to %s:%s\n",
                pcp->host, pcp->port);

    sock_fd = fd_connect((struct sockaddr *) &pcp->sin, sizeof(pcp->sin));
    if (sock_fd < 0)
    {
        if (connect_error_page)
            return send_page(cip, connect_error_page);
        else
            return error_system(cip, "connect");
    }

    write_data(sock_fd, hip, url_buf, cip->client.addr_s);

    fd_relay(sock_fd, fd, 1);

    fd_close(sock_fd);

    return STATUS_RELAYED;
}
