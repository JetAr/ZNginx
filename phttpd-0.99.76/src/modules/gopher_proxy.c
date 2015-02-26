/*
** gopher_proxy.c
**
** Copyright (c) 1995 Peter Eriksson <pen@signum.se>
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


int pm_init(const char **argv)
{
    const char *name = argv[0];

    if (debug > 1)
        fprintf(stderr, "gopher_proxy: pm_init(\"%s\")\n", name);

    return 0;
}

void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "gopher_proxy: pm_exit()\n");
}


static int get_rq(char *in_url,
                  char **host,
                  char **port,
                  char **out_url)
{
    char *cp;


    if (strncmp(in_url, "gopher://", 9) != 0)
        return -1;

    cp = in_url+9;
    *host = cp;
    while (*cp && !(*cp == '/' || *cp == ':'))
        ++cp;

    if (*cp == 0)
        return 1;

    if (*cp == ':')
    {
        *cp++ = '\0';
        *port = cp;

        while (*cp && *cp != '/')
            ++cp;

        if (*cp == 0)
            return 2;
    }

    *cp++ = '\0';
    *out_url = cp;

    return 3;
}


void send_href(int fd,
               char *prot,
               char *host,
               char *port,
               char seltype,
               char *selector,
               char *label,
               char *append)
{
    char seltag[2];
    char buf[1024];


    seltag[0] = seltype,
                seltag[1] = '\0';

    if (port && strcmp(prot, "gopher") == 0 &&
            strcmp(port, "70") == 0)
        port = NULL;

    fd_printf(fd, "<LI> <A HREF=\"%s://%s%s%s/%s%s\">%s%s</A>\n",
              prot,
              host ? host : "",
              port ? ":" : "",
              port ? port : "",
              seltag, url_quote(selector, buf, sizeof(buf), "?", 0),
              label,
              append ? append : "");
}


void send_telnet(int fd,
                 char *prot,
                 char *host,
                 char *port,
                 char *selector,
                 char *label,
                 char *append)
{
    char buf[1024];

    if (port && strcmp(prot, "telnet") == 0 &&
            strcmp(port, "23") == 0)
        port = NULL;

    fd_printf(fd, "<LI> <A HREF=\"%s://%s%s%s%s%s/\">%s%s</A>\n",
              prot,
              selector ? url_quote(selector, buf, sizeof(buf), "?", 0) : "",
              selector ? "@" : "",
              host ? host : "",
              port ? ":" : "",
              port ? port : "",
              label,
              append ? append : "");
}

int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip;
    char *tmp, *host, *port, *url, buf[1024];
    int sock_fd, result, fd, tmpsize;


    fd = cip->fd;
    host = port = url = NULL;

    hip = cip->hip;

    if (debug > 1)
    {
        fprintf(stderr, "gopher_proxy: pm_request()\n");
        fprintf(stderr, "\thip->method = %s\n", hip->method);
        fprintf(stderr, "\thip->url = %s\n", hip->url);
    }


    tmpsize = strlen(hip->url)+1;
    tmp = (char *) alloca(tmpsize);
    s_strcpy(tmp, tmpsize, hip->url);

    if (get_rq(tmp, &host, &port, &url) < 0)
        goto Fail;

    if (debug > 2)
        fprintf(stderr, "get_rq(), host=%s, port=%s, url=%s\n",
                host ? host : "<null>",
                port ? port : "<null>",
                url ? url : "<null>");

    /* XXX should probably check if this is ourself we are connecting to... */

    sock_fd = fd_sconnect(host, port ? port : "70");
    if (sock_fd < 0)
    {
        if (debug > 2)
            fprintf(stderr, "fd_sconnect(\"%s\") failed\n", host);
        goto Fail;
    }

    if (debug > 4)
        fprintf(stderr, "Gopher query: %s\n", url+1);

    result = 200;
    if (hip->mip)
        http_sendheaders(fd, cip, result, NULL);

    switch (url[0])
    {
    case '7':
    {
        char **argv, *request, *full_url;
        int i, fusize;

        fusize = i = strlen(hip->url);
        full_url = alloca(fusize);
        s_strncpy(full_url, fusize, hip->url, i-1);
        full_url[i-1] = '\0';

        if (debug > 4)
            fprintf(stderr, "Gopher query.\n");

        if (hip->request)
        {
            int reqsize = strlen(hip->request)+1;
            request = (char *) alloca(reqsize);
            s_strcpy(request, reqsize, hip->request);
        }
        else
            request = NULL;

        if (request == NULL ||
                *request == '\0' ||
                (argv = strsplit(request, '&', 2)) == NULL)
        {
            fd_puts("Content-Type: text/html\n\n", fd);
            fd_puts("<HTML><HEAD>\n", fd);
            fd_printf(fd, "<TITLE>Gopher Search of %s</TITLE>\n",
                      full_url);
            fd_puts("</HEAD><BODY>\n", fd);
            fd_printf(fd, "<H2 ALIGN=CENTER>Gopher Search of %s</H2>\n",
                      full_url);
            fd_printf(fd, "<ISINDEX>\n");
            fd_puts("</BODY></HTML>\n", fd);
            break;
        }

        fd_puts(url+1, sock_fd);

        fd_putc('\t', sock_fd);

        for (i = 0; argv[i]; i++)
        {
            fd_puts(argv[i], sock_fd);
            if (argv[i+1])
                fd_putc(' ', sock_fd);
        }

        fd_putc('\n', sock_fd);
        fd_flush(sock_fd);
    }
    case '\0':
    case '1':
        if (url[0] != '7')
        {
            if (debug > 4)
                fprintf(stderr, "Gopher menu.\n");

            if (url[0])
                fd_printf(sock_fd, "%s\n", url+1);
            else
                fd_puts("\n", sock_fd);
            fd_flush(sock_fd);
        }

        fd_puts("Content-Type: text/html\n\n", fd);
        html_sysheader(fd, "H2", "Gopher Menu:");

        fd_puts("<OL>\n", fd);
        while (fd_gets(buf, sizeof(buf), sock_fd))
        {
            char *label, *selector, *host, *port, *extra;


            label = buf+1;
            host = port = NULL;

            selector = strchr(label, '\t');
            if (selector)
            {
                *selector++ = '\0';

                host = strchr(selector, '\t');
                if (host)
                {
                    *host++ = '\0';

                    port = strchr(host, '\t');
                    if (port)
                    {
                        *port++ = '\0';

                        extra = strchr(port, '\t');
                        if (extra)
                            *extra++ = '\0';
                    }
                }
            }

            switch (buf[0])
            {
            case '.':
                break;

            case 'i':
                fd_printf(fd, "<LI> %s\n", label);
                break;

            case '3':
                fd_printf(fd, "<B>%s</B>\n", label);
                break;

            case '0':
            case '2':
            case '4':
            case '5':
            case '6':
            case '7':
            case '9':
            case 'g':
            case 'I':
            case 's':
            case ';':
                send_href(fd, "gopher", host, port, buf[0], selector, label, NULL);
                break;

            case '1':
                send_href(fd, "gopher", host, port, buf[0], selector, label, "/");
                break;

            case '8':
                send_telnet(fd, "telnet", host, port, selector, label, NULL);
                break;

            case 'T':
                send_telnet(fd, "tn3270", host, port, selector, label, NULL);
                break;

            default:
                fd_printf(fd, "<LI> ? type='%c' (%d), label=%s",
                          buf[0], (int) buf[0], label);
                if (selector)
                    fd_printf(fd, ", selector=%s", selector);
                if (host)
                    fd_printf(fd, ", host=%s", host);
                if (port)
                    fd_printf(fd, ", port=%s", port);
                fd_putc('\n', fd);
            }
        }

        fd_puts("</OL>\n", fd);
        html_sysfooter(fd);
        break;

    case '0':
        if (url[0])
            fd_printf(sock_fd, "%s\n", url+1);
        else
            fd_puts("\n", sock_fd);
        fd_flush(sock_fd);

        if (debug > 4)
            fprintf(stderr, "Gopher text.\n");
        fd_puts("Content-Type: text/plain\n\n", fd);

        fd_relay(sock_fd, fd, 0);
        break;

    case '2':
    {
        char **argv, *request, *url;
        int i;

        url = alloca(i = strlen(hip->url));
        s_strncpy(url, i, hip->url, i-1);
        url[i-1] = '\0';

        if (hip->request)
        {
            int reqsize = strlen(hip->request)+1;
            request = (char *) alloca(reqsize);
            s_strcpy(request, reqsize, hip->request);
        }
        else
            request = NULL;

        if (request == NULL ||
                *request == '\0' ||
                (argv = strsplit(request, '&', 2)) == NULL)
        {
            fd_puts("Content-Type: text/html\n\n", fd);
            fd_puts("<HTML><HEAD>\n", fd);
            fd_printf(fd, "<TITLE>CSO Search of %s</TITLE>\n",
                      url);
            fd_puts("</HEAD><BODY>\n", fd);
            fd_printf(fd, "<H2 ALIGN=CENTER>CSO Search of %s</H2>\n",
                      url);
            fd_printf(fd, "<ISINDEX>\n");
            fd_puts("</BODY></HTML>\n", fd);
            break;
        }

        fd_puts("query", sock_fd);
        for (i = 0; argv[i]; i++)
            fd_printf(sock_fd, " %s", argv[i]);

        fd_putc('\n', sock_fd);
        fd_flush(sock_fd);
        fd_puts("quit\n", sock_fd);
        fd_shutdown(sock_fd, 1);

        fd_puts("Content-Type: text/plain\n\n", fd);
        fd_relay(sock_fd, fd, 0);
    }
    break;

    case '4':
        if (url[0])
            fd_printf(sock_fd, "%s\n", url+1);
        else
            fd_puts("\n", sock_fd);
        fd_flush(sock_fd);

        if (debug > 4)
            fprintf(stderr, "Gopher macbinhex.\n");
        fd_puts("Content-Type: application/binhex\n\n", fd);

        fd_relay(sock_fd, fd, 0);
        break;

    case '5':
        if (debug > 4)
            fprintf(stderr, "Gopher pcbinary.\n");

        if (url[0])
            fd_printf(sock_fd, "%s\n", url+1);
        else
            fd_puts("\n", sock_fd);
        fd_flush(sock_fd);

        fd_puts("Content-Type: application/octet-stream\n\n", fd);

        fd_relay(sock_fd, fd, 0);
        break;


    case '6':
        if (debug > 4)
            fprintf(stderr, "Gopher uuencoded.\n");

        if (url[0])
            fd_printf(sock_fd, "%s\n", url+1);
        else
            fd_puts("\n", sock_fd);
        fd_flush(sock_fd);

        fd_puts("Content-Type: application/uuencoded\n\n", fd);

        fd_relay(sock_fd, fd, 0);
        break;

    case '9':
        if (debug > 4)
            fprintf(stderr, "Gopher binary.\n");

        if (url[0])
            fd_printf(sock_fd, "%s\n", url+1);
        else
            fd_puts("\n", sock_fd);
        fd_flush(sock_fd);

        fd_puts("Content-Type: application/octet-stream\n\n", fd);

        fd_relay(sock_fd, fd, 0);
        break;

    case 'g':
    case 'I':
        if (debug > 4)
            fprintf(stderr, "Gopher GIF.\n");

        if (url[0])
            fd_printf(sock_fd, "%s\n", url+1);
        else
            fd_puts("\n", sock_fd);
        fd_flush(sock_fd);

        fd_puts("Content-Type: image/gif\n\n", fd);

        fd_relay(sock_fd, fd, 0);
        break;


    case 's':
        if (debug > 4)
            fprintf(stderr, "Gopher Sound.\n");

        if (url[0])
            fd_printf(sock_fd, "%s\n", url+1);
        else
            fd_puts("\n", sock_fd);
        fd_flush(sock_fd);

        fd_puts("Content-Type: audio/unknown\n\n", fd);

        fd_relay(sock_fd, fd, 0);
        break;

    case ';':
        if (debug > 4)
            fprintf(stderr, "Gopher MPEG.\n");

        if (url[0])
            fd_printf(sock_fd, "%s\n", url+1);
        else
            fd_puts("\n", sock_fd);
        fd_flush(sock_fd);

        fd_puts("Content-Type: vide/mpeg\n\n", fd);

        fd_relay(sock_fd, fd, 0);
        break;

    default:
        fd_puts("Content-Type: text/html\n\n", fd);
        html_sysheader(fd, "H2", "Gopher Menu:");

        fd_printf(fd, "Unknown gopher query type: '%c'\n", url[1]);
        html_sysfooter(fd);
    }


    if (debug > 4)
        fprintf(stderr, "gopher_proxy: closing down\n");

    fd_close(sock_fd);

    return 200;

Fail:
    return -1;
}

