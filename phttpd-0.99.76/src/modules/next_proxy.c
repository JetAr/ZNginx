/*
** next_proxy.c
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
#include <alloca.h>

#include "phttpd.h"

static char *proxy_address = NULL;
static char *proxy_port = NULL;

static struct options next_cfg_table[] =
{
    { "proxy-address", T_STRING, &proxy_address, NULL },
    { "proxy-port",    T_STRING, &proxy_port, NULL },

    { NULL, -1, NULL, NULL }
};


int pm_init(const char **argv)
{
    const char *name = argv[0];
    char *cfg_path, *cp;
    int cfg_size;


    if (debug > 1)
        fprintf(stderr, "next_proxy: pm_init(\"%s\")\n", name);

    cfg_path = s_strxdup(name, 6, &cfg_size);

    cp = strrchr(cfg_path, '.');
    if (cp && strcmp(cp, ".so") == 0)
        *cp = '\0';

    s_strcat(cfg_path, cfg_size, ".conf");
    if (config_parse_file(cfg_path, next_cfg_table, 0) < 0)
        return -1;

    if (config_parse_argv(argv+1, next_cfg_table) < 0)
        return -1;

    return 0;
}

void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "next_proxy: pm_exit()\n");

    if (proxy_address)
        s_free(proxy_address);

    if (proxy_port)
        s_free(proxy_port);
}



int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip;
    int sock_fd;


    hip = cip->hip;

    if (debug > 1)
    {
        fprintf(stderr, "next_proxy: pm_request()\n");
        fprintf(stderr, "\thip->method = %s\n", hip->method);
        fprintf(stderr, "\thip->url = %s\n", hip->url);
    }

    sock_fd = fd_sconnect(proxy_address, proxy_port ? proxy_port : "80");
    if (sock_fd < 0)
    {
        if (debug > 2)
            fprintf(stderr, "fd_sconnect(\"%s\") failed\n", proxy_port);
        goto Fail;
    }

    if (hip->version)
    {
        fd_printf(sock_fd, "%s %s%s%s %s\n",
                  hip->method, hip->orig_url,
                  hip->request ? "?" : "",
                  hip->request ? hip->request : "",
                  hip->version);
        if (hip->mip)
            mime_writeheaders(sock_fd, hip->mip);

        fd_putc('\n', sock_fd);
    }
    else
        fd_printf(sock_fd, "%s %s%%s\n",
                  hip->method, hip->orig_url,
                  hip->request ? "?" : "",
                  hip->request ? hip->request : "");

    if (debug > 4)
        fprintf(stderr, "next_proxy: relaying\n");

    fd_flush(sock_fd);

    fd_relay(sock_fd, cip->fd, 1);

    if (debug > 4)
        fprintf(stderr, "next_proxy: closing down\n");

    fd_close(sock_fd);

    return 200;

Fail:
    return -1;
}


