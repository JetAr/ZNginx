#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#include "phttpd.h"

int pm_init(const char **argv)
{
    fprintf(stderr, "*** pm_init(\"%s\") called ***\n", argv[0]);
    return 0;
}


void pm_exit(void)
{
    fprintf(stderr, "*** pm_exit() called ***\n");
}


static int http_get(struct connectioninfo *cip)
{
    char buf[256];
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    fprintf(stderr, "*** http_get() called ***\n");

    http_time_r(&cip->cn_time, buf, sizeof(buf));

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, 200, NULL);

        fd_printf(fd, "Last-Modified: %s", buf);
        fd_printf(fd, "Content-Type: text/html\n\n");
    }

    html_sysheader(fd, "H2", "Test av DynLink - GET");

    fd_printf(fd, "<B>Server host is:</B> %s<BR>\n", server_host);
    fd_printf(fd, "<B>Server port is:</B> %d<BR>\n", server_port);

    fd_printf(fd, "<B>Client host is:</B> %s<BR>\n", cip->client.addr_s);
    fd_printf(fd, "<B>Client port is:</B> %d\n", cip->client.port);

    fd_printf(fd, "<P><B>Current time is:</B> %s", buf);

    html_sysfooter(fd);

    return 200;
}



static int http_head(struct connectioninfo *cip)
{
    char buf[256];
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug)
        fprintf(stderr, "*** http_head() called ***\n");

    http_time_r(&cip->cn_time, buf, sizeof(buf));

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, 200, NULL);

        fd_printf(fd, "Last-Modified: %s", buf);
        fd_printf(fd, "Content-Type: text/html\n");
    }

    return 200;
}



static int http_put(struct connectioninfo *cip)
{
    int i;
    char buf[256];
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug)
        fprintf(stderr, "*** http_put() called ***\n");

    i = 0;

    while (fd_gets(buf, sizeof(buf), fd))
        i++;

    http_time_r(&cip->cn_time, buf, sizeof(buf));

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, 200, NULL);

        fd_printf(fd, "Last-Modified: %s", buf);
        fd_printf(fd, "Content-Type: text/html\n\n");
    }

    html_sysheader(fd, "H2", "Test av DynLink - PUT");

    fd_printf(fd, "<B>Antal inputrader:</B> %d<BR>\n", i);

    fd_printf(fd, "<B>Server host is:</B> %s<BR>\n", server_host);
    fd_printf(fd, "<B>Server port is:</B> %d<BR>\n", server_port);

    fd_printf(fd, "<B>Client host is:</B> %s<BR>\n", cip->client.addr_s);
    fd_printf(fd, "<B>Client port is:</B> %d\n", cip->client.port);

    fd_printf(fd, "<P><B>Current time is:</B> %s", buf);

    html_sysfooter(fd);

    return 200;
}


static int http_post(struct connectioninfo *cip)
{
    int i;
    char buf[256];
    int len;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug)
        fprintf(stderr, "*** http_post() called ***\n");

    http_time_r(&cip->cn_time, buf, sizeof(buf));

    len = -1;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, 200, NULL);

        fd_printf(fd, "Last-Modified: %s", buf);
        fd_printf(fd, "Content-Type: text/html\n\n");

    }

    fd_flush(fd);

    if (len < 0)
    {
        /* XXXXX Not working!!!! XXXXXX */
        i = 0;
        while (fd_gets(buf, sizeof(buf), fd))
        {
            fprintf(stderr, "      fd_gets: \"%s\"\n", buf);
            i++;
        }

        fprintf(stderr, "      done (fd_gets loop)\n");
    }
    else
    {
        fd_printf(fd, "<HR>\n");
        i = fd_read(fd, buf, len);
        buf[i] = '\0';
        fd_printf(fd, "<B>Data:</B> %s\n", buf);
        fd_printf(fd, "<HR>\n");
    }


    html_sysheader(fd, "H1", "Test av DynLink - POST");

    if (debug > 2)
        fprintf(stderr, "   http_post, after fd_gets()\n");

    fd_printf(fd, "<B>Antal input-tecken:</B> %d<BR>\n", i);

    html_sysfooter(fd);
    return 200;
}


int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;


    if (strcasecmp(hip->method, "GET") == 0)
        return http_get(cip);
    else if (strcasecmp(hip->method, "HEAD") == 0)
        return http_head(cip);
    else if (strcasecmp(hip->method, "PUT") == 0)
        return http_put(cip);
    else if (strcasecmp(hip->method, "POST") == 0)
        return http_post(cip);
    else
        return -1;
}
