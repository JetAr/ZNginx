/*
** config.c
**
** This is the main file of the config module.
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
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <thread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "phttpd.h"


int pm_init(const char **argv)
{
    const char *name = argv[0];

    if (debug > 1)
        fprintf(stderr, "*** libconfig/pm_init(\"%s\") called ***\n", name);

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** libconfig/pm_exit() called ***\n");
}


struct miscinfo
{
    int fd;
    char *url;
};

static int print_connection(struct connectioninfo *cip, void *misc)
{
    char buf[256];
    struct miscinfo *mp;



    mp = (struct miscinfo *) misc;

    fd_printf(mp->fd, "<tr><td align=right>%d</td>\n", cip->request_no);

    fd_printf(mp->fd, "<td align=right><A HREF=\"%s?descriptors#%d\">%d</A></td>",
              mp->url, (cip->fd < 0 ? -(cip->fd) : cip->fd), cip->fd);

    fd_printf(mp->fd, "<td align=right>%d</td> <td>%s:%d</td><td>%s</td>",
              (int) cip->tid,
              cip->client.addr_s,
              cip->client.port,
              ctime_r(&cip->cn_time, buf, sizeof(buf)));

    if (cip->hip && cip->hip->orig_url)
        fd_printf(mp->fd, "<td>%s%s%s</td></tr>\n", cip->hip->orig_url,
                  cip->hip->request ? "?" : "",
                  cip->hip->request ? cip->hip->request : "");

    return 0;
}

static int print_module(struct modinfo *mp, void *misc)
{
    int fd;
    char buf[256];


    fd = * (int *) misc;

    fd_printf(fd, "<tr><td>%s</td><td align=right>%d</td><td align=right>%s</td></tr>\n",
              mp->name,
              mp->use,
              ctime_r(&mp->mtime, buf, sizeof(buf)));

    return 0;
}

static int print_fd(int fd, struct fd_buffer *bp, void *misc)
{
    int out_fd;


    out_fd = * (int *) misc;

    if (debug > 5)
        fprintf(stderr, "CONFIG: print_fd(%d, %p, %p)\n",
                fd, bp, misc);

    if (fd == out_fd)
        return 0;

    fd_printf(out_fd, "<a name=\"%d\">\n", fd);
    fd_puts("<tr>\n", out_fd);

    if (bp)
    {
        fd_printf(out_fd, "<td align=right>%d</td>\n", fd);
        fd_printf(out_fd, "<td align=right>%d</td>", bp->type);

        fd_printf(out_fd, "<td align=right>%d</td>\n",
                  bp->in.end - bp->in.start);
        fd_printf(out_fd, "<td align=right>%d</td>\n",
                  bp->in.size);
        fd_printf(out_fd, "<td align=right>%d</td>\n",
                  bp->in.state);

        fd_printf(out_fd, "<td align=right>%d</td>\n",
                  bp->out.end - bp->out.start);
        fd_printf(out_fd, "<td align=right>%d</td>\n",
                  bp->out.size);
        fd_printf(out_fd, "<td align=right>%d</td>\n",
                  bp->out.state);

    }
    else
    {
        fd_printf(out_fd, "<td align=right>%d</td>", fd);
        fd_puts("<td align=right>unknown</td> ", out_fd);
        fd_puts("<td align=right>unknown</td> ", out_fd);
        fd_puts("<td align=right>unknown</td> ", out_fd);
        fd_puts("<td align=right>unknown</td> ", out_fd);
        fd_puts("<td align=right>unknown</td> ", out_fd);
        fd_puts("<td align=right>unknown</td> ", out_fd);
        fd_puts("<td align=right>unknown</td> ", out_fd);
        fd_puts("<td align=right>unknown</td> ", out_fd);
    }

    fd_puts("</tr>\n", out_fd);
    fd_puts("</a>\n", out_fd);
    return 0;
}


static int print_table_pair(void *value, void *misc)
{
    char **pair = (char **) value;
    int fd = * (int *) misc;


    fd_printf(fd, "<tr><td>%s</td> <td>%s</td></tr>\n", pair[0], pair[1]);

    return 0;
}

static void print_stats(int fd, cachestat_t *csp, char *label)
{
    fd_printf(fd, "<tr><td>%-8s<td align=right>%10lu<td align=right>%10lu<td align=right>%10lu<td align=right>%10lu</tr>\n",
              label,
              csp->lookups,
              csp->reloads,
              csp->updates,
              csp->hits);
}

static int http_get_head(struct connectioninfo *cip)
{
    int result, len, i;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;
    extern struct options global_cfg_table[];

    result = 200;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);

        fd_puts("Pragma: No-Cache\n", fd);

        http_sendlastmodified(fd, cip->cn_time);

        fd_puts("Content-Type: text/html\n", fd);
    }

    if (hip->mip != NULL)
        fd_putc('\n', fd);

    if (strcasecmp(hip->method, "HEAD") == 0)
        return result;

    len = fd_written(fd);

    if (hip->request == NULL)
    {
        html_sysheader(fd, "H2", "Phttpd Information Menu");

        fd_puts("<OL>\n", fd);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?statistics\">Server Statistics</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?variables\">Configuration Variables</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?url_handlers\">URL Handlers</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?url_redirects\">URL Temporary Redirects</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?url_predirects\">URL Permanent Redirects</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?url_rewrites\">URL Rewrite Rules</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?read_authenticate\">URL Read Authenticate Rules</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?write_authenticate\">URL Write Authenticate Rules</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?host_access\">Url Host Access Table</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?connections\">Current Connections</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?modules\">Loaded Modules</A>\n",
                  hip->url);

        fd_printf(fd,
                  "<LI><A HREF=\"%s?descriptors\">Open Descriptors</A>\n",
                  hip->url);

        if (cntfile_path)
        {
            fd_printf(fd,
                      "<LI><A HREF=\"%s?requests\">Request Summary</A>\n",
                      hip->url);
        }

        fd_puts("</OL>\n", fd);
    }

    else if (strcmp(hip->request, "variables") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd Variables");

        fd_puts("<table width=\"80%\">\n", fd);
        fd_puts("<tr><th align=left width=\"30%\">Name</th><th align=left>Value</th></tr>\n", fd);
        fd_puts("<tr><td colspan=2><hr noshade size=0></td></tr>\n", fd);

        for (i = 0; global_cfg_table[i].name; i++)
        {
            switch (global_cfg_table[i].type)
            {
            case T_NUMBER:
                fd_puts("<tr>\n", fd);
                fd_printf(fd, "<td>%s</td> <td>%d</td>\n",
                          global_cfg_table[i].name,
                          * (int *) global_cfg_table[i].var);
                fd_puts("</tr>\n", fd);
                fd_puts("</tr>\n", fd);
                break;

            case T_STRING:
                fd_puts("<tr>\n", fd);
                fd_printf(fd, "<td>%s</td> <td>\"%s\"</td>\n",
                          global_cfg_table[i].name,
                          * (char **) global_cfg_table[i].var ?
                          * (char **) global_cfg_table[i].var : "<null>");
                fd_puts("</tr>\n", fd);
                break;

            case T_BOOL:
                fd_puts("<tr>\n", fd);
                fd_printf(fd, "<td>%s</td> <td>%s</td>\n",
                          global_cfg_table[i].name,
                          (* (int *) global_cfg_table[i].var) ?
                          "True" : "False");
                fd_puts("</tr>\n", fd);
                break;
            }
        }
        fd_puts("</table>\n", fd);
    }

    else if (strcmp(hip->request, "url_handlers") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd URL Handlers");

        fd_puts("<table width=\"80%\">\n", fd);
        fd_puts("<tr><th align=left width=\"30%\">Url</th><th align=left>Module</th></tr>\n",fd);
        fd_puts("<tr><td colspan=2><hr noshade size=0></td></tr>\n", fd);
        if (url_handlers_table)
            tbl_foreach(url_handlers_table, print_table_pair, &fd);
        fd_puts("</table>\n", fd);
    }

    else if (strcmp(hip->request, "url_redirects") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd URL Temporary Redirects");

        fd_puts("<table width=\"80%\">\n", fd);
        fd_puts("<tr><th align=left width=\"30%\">From</th><th align=left>To</th></tr>\n",fd);
        fd_puts("<tr><td colspan=2><hr noshade size=0></td></tr>\n", fd);
        if (url_redirects_table)
            tbl_foreach(url_redirects_table, print_table_pair, &fd);
        fd_puts("</table>\n", fd);
    }

    else if (strcmp(hip->request, "url_predirects") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd URL Permanent Redirects");

        fd_puts("<table width=\"80%\">\n", fd);
        fd_puts("<tr><th align=left width=\"30%\">From</th><th align=left>To</th></tr>\n",fd);
        fd_puts("<tr><td colspan=2><hr noshade size=0></td></tr>\n", fd);

        if (url_predirects_table)
            tbl_foreach(url_predirects_table, print_table_pair, &fd);
        fd_puts("</table>\n", fd);
    }

    else if (strcmp(hip->request, "url_rewrites") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd URL Rewrites");

        fd_puts("<table width=\"80%\">\n", fd);
        fd_puts("<tr><th align=left width=\"30%\">From</th><th align=left>To</th></tr>\n",fd);
        fd_puts("<tr><td colspan=2><hr noshade size=0></td></tr>\n", fd);
        if (url_rewrites_table)
            tbl_foreach(url_rewrites_table, print_table_pair, &fd);
        fd_puts("</table>\n", fd);
    }

    else if (strcmp(hip->request, "read_authenticate") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd URL Read Authentication Rules");

        fd_puts("<table width=\"80%\">\n", fd);
        fd_puts("<tr><th align=left width=\"30%\">Url</th><th align=left>Domain</th></tr>\n",fd);
        fd_puts("<tr><td colspan=2><hr noshade size=0></td></tr>\n", fd);
        if (read_authenticate_table)
            tbl_foreach(read_authenticate_table, print_table_pair, &fd);
        fd_puts("</table>\n", fd);
    }

    else if (strcmp(hip->request, "write_authenticate") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd URL Write Authentication Rules");

        fd_puts("<table width=\"80%\">\n", fd);
        fd_puts("<tr><th align=left width=\"30%\">Url</th><th align=left>Domain</th></tr>\n",fd);
        fd_puts("<tr><td colspan=2><hr noshade size=0></td></tr>\n", fd);
        if (write_authenticate_table)
            tbl_foreach(write_authenticate_table, print_table_pair, &fd);
        fd_puts("</table>\n", fd);
    }

    else if (strcmp(hip->request, "host_access") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd URL Host Access Rules");

        fd_puts("<table width=\"80%\">\n", fd);
        fd_puts("<tr><th align=left width=\"30%\">Url</th><th align=left>File</th></tr>\n",fd);
        fd_puts("<tr><td colspan=2><hr noshade size=></td></tr>\n", fd);
        if (host_access_table)
            tbl_foreach(host_access_table, print_table_pair, &fd);
        fd_puts("</table>\n", fd);
    }

    else if (strcmp(hip->request, "statistics") == 0)
    {
        struct count_t count;
        char buf[256];
        int cs, dt;
        time_t time_now;


        time(&time_now);

        html_sysheader(fd, "H2", "Phttpd Statistics");

        fd_puts("<H3>General</H3>", fd);

        fd_puts("<table>", fd);

        fd_printf(fd, "<tr><td>Server started:</td><td>%s</td></tr>",
                  ctime_r(&start_time, buf, sizeof(buf)));

        fd_printf(fd, "<tr><td>Current time:</td><td>%s",
                  ctime_r(&time_now, buf, sizeof(buf)));

        fd_printf(fd, "<tr><td>Connections totally:</td><td>%d ",
                  cip->request_no);

        cs = cip->request_no / (time_now - start_time);
        if (cs == 0)
        {
            dt = (time_now - start_time) / 60 + 1;
            cs = cip->request_no / dt;

            if (cs == 0)
            {
                dt = (time_now - start_time) / 3600 + 1;
                cs = cip->request_no / dt;
                fd_printf(fd, "(%d conns/hour)", cs);
            }
            else
                fd_printf(fd, "(%d conns/min)", cs);
        }
        else
            fd_printf(fd, "(%d conns/sec)", cs);

        fd_printf(fd, "</td></tr><tr><td><A HREF=\"%s?connections\">", hip->url);

        fd_printf(fd, "Connections currently</A>:</td><td>%d</td></tr>\n",
                  clist_active());

        fd_puts("</table>", fd);

        fd_puts("<H3>Cache statistics</H3>\n", fd);

        {
            cachestat_t csb;

            fd_puts("<table><tr>", fd);
            fd_printf(fd, "<th align=left>%-8s<th align=right>%10s<th align=right>%10s<th align=right>%10s<th align=right>%10s</tr>\n",
                      "Cache",
                      "Lookups",
                      "Reloads",
                      "Updates",
                      "Hits");

            fd_puts("<tr><td colspan=5><hr noshade size=0></td></tr>\n", fd);

            urlcache_getstats(&csb);
            print_stats(fd, &csb, "URL");

            fscache_getstats(&csb);
            print_stats(fd, &csb, "Filesys");

            usercache_getstats(&csb);
            print_stats(fd, &csb, "Username");

            hostcache_getstats_byname(&csb);
            print_stats(fd, &csb, "Hostname");

            hostcache_getstats_byaddr(&csb);
            print_stats(fd, &csb, "Hostaddr");

            fd_puts("</table>\n", fd);
        }

#ifdef INCLUDE_ALLOC_STATS
        {
            struct allocstats lasb;

            fd_puts("<H3>Memory allocation statistics</H3>\n", fd);

            mutex_lock(&alloc_stats.lock);
            lasb = alloc_stats;
            mutex_unlock(&alloc_stats.lock);

            fd_puts("<table>", fd);
            fd_printf(fd, "<tr><td>Calls to malloc</td><td align=right>%lu</td></tr>\n",
                      lasb.malloc_calls);

            fd_printf(fd, "<tr><td>Calls to free</td><td align=right>%lu</td></tr>\n",
                      lasb.free_calls);

            fd_printf(fd, "<tr><td>Calls to realloc</td><td align=right>%lu</td></tr>\n",
                      lasb.realloc_calls);

            fd_printf(fd, "<tr><td>Bytes allocated</td><td align=right>%lu</td></tr>\n",
                      lasb.bytes);

            fd_printf(fd, "<tr><td>Blocks allocated</td><td align=right>%lu</td></tr>\n",
                      lasb.blocks);

            fd_puts("</table>", fd);
        }
#endif

        if (cntfile_path)
        {
            get_totals(&count);

            fd_printf(fd,
                      "<H4>Totals for <A HREF=\"%s?requests\">Summary</A> Period: %s",
                      hip->url,
                      ctime_r(&(count.starttime), buf, sizeof(buf)));
            fd_printf(fd, " to %s</H4>\n",
                      ctime_r(&time_now, buf, sizeof(buf)));
            print_totals(fd);

        }

    }

    else if (cntfile_path && strcmp(hip->request, "requests") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd Requests Summary");

        print_counts(fd);
    }

    else if (strcmp(hip->request, "connections") == 0)
    {
        struct miscinfo mib;

        html_sysheader(fd, "H2", "Phttpd Current Connections");

        mib.fd = fd;
        mib.url = hip->url;

        fd_puts("<table>\n", fd);
        fd_puts("<tr>\n", fd);
        fd_puts("<th>Conn</th>\n", fd);
        fd_puts("<th>File</th>\n", fd);
        fd_puts("<th>Thread</th>\n", fd);
        fd_puts("<th align=left>Client</th>\n", fd);
        fd_puts("<th align=left>Started</th>\n", fd);
        fd_puts("<th align=left>URL</th>\n", fd);
        fd_puts("</tr><tr><td colspan=6><hr noshade size=0></td></tr>\n", fd);

        clist_foreach(print_connection, (void *) &mib);
        fd_puts("</table>", fd);
    }

    else if (strcmp(hip->request, "modules") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd Loaded Modules");

        fd_puts("<table width=80%>\n<tr>\n", fd);
        fd_puts("<th align=left>Module</th>\n", fd);
        fd_puts("<th align=right>Use</th>\n", fd);
        fd_puts("<th align=right>Modified</th>\n", fd);
        fd_puts("<tr> <td colspan=3><hr noshade size=0>\n", fd);

        md_foreach(print_module, (void *) &fd);

        fd_puts("</table>", fd);
    }

    else if (strcmp(hip->request, "descriptors") == 0)
    {
        html_sysheader(fd, "H2", "Phttpd Descriptors");

        fd_printf(fd, "(Excluding this connection, fd#%d)\n", fd);

        fd_puts("<table width=\"90%\">\n", fd);
        fd_puts("<tr>\n", fd);
        fd_puts("<th>fd</th>\n", fd);
        fd_puts("<th>type</th>\n", fd);

        fd_puts("<th>input len</th>\n", fd);
        fd_puts("<th>input size</th>\n", fd);
        fd_puts("<th>input state</th>\n", fd);

        fd_puts("<th>output len</th>\n", fd);
        fd_puts("<th>output size</th>\n", fd);
        fd_puts("<th>output state</th>\n", fd);
        fd_puts("</tr>\n", fd);

        fd_puts("<tr>\n<td colspan=8><hr noshade size=0></td>\n</tr>\n", fd);

        fd_foreach(print_fd, (void *) &fd);

        fd_puts("</table><p>\n", fd);
        fd_puts("<b>Types:</b> 0=Socket, 1=File, 2=TTY, 3=Pipe, 4=String<br>\n", fd);
        fd_puts("<b>States:</b> -1=EOF, 0=Free, 1=Open\n", fd);
    }

    else
    {
        html_sysheader(fd, "H2", "Unknown config request");

        fd_puts("<BLOCKQUOTE>", fd);
        fd_puts(hip->request, fd);
        fd_puts("</BLOCKQUOTE>", fd);
    }

    html_sysfooter(fd);
#if 0
    hip->length = fd_written(fd) - len;
#endif
    return result;
}




int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;


    if (strcasecmp(hip->method, "GET") == 0 ||
            strcasecmp(hip->method, "HEAD") == 0)
        return http_get_head(cip);
    else
        return -1;
}
