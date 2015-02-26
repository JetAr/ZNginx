/*
** logger.c
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
#include <synch.h>
#include <fcntl.h>
#include <ndbm.h>
#include <string.h>

#include "phttpd.h"


static char *log_path = NULL;
static mutex_t log_lock;
static mutex_t count_lock;
static char *count_path = NULL;
static DBM *count_fd = NULL;

static struct count_t totals;



void log_init(char *path, char *cnt_path)
{
    mutex_init(&log_lock, USYNC_THREAD, NULL);
    mutex_init(&count_lock, USYNC_THREAD, NULL);

    if (path)
    {
        log_path = path;
        log_fd = fd_open(path, O_WRONLY+O_CREAT+O_APPEND, 0644);

        totals.totalhits = 0;
        totals.totalbytes = 0;
        totals.starttime = totals.lastaccess = time(NULL);
        totals.reserved = 0;

    }


    if (cnt_path)
    {
        count_path = cnt_path;
        count_fd = dbm_open(cnt_path, O_RDWR|O_CREAT, 0644);
        if (count_fd != NULL)
        {
            datum cnt_datum;
            datum key_datum;

            key_datum.dptr = "Totals";
            key_datum.dsize = strlen(key_datum.dptr);
            cnt_datum = dbm_fetch(count_fd, key_datum);

            if (cnt_datum.dptr)
            {
                memcpy(&totals, cnt_datum.dptr, sizeof(struct count_t));
            }
        }
    }
}

void log_reopen(void)
{

    mutex_lock(&log_lock);
    if (log_fd >= 0)
        fd_close(log_fd);

    if (log_path != NULL)
        log_fd = fd_open(log_path, O_WRONLY+O_CREAT+O_APPEND, 0644);

    mutex_unlock(&log_lock);

    mutex_lock(&count_lock);
    if (count_fd != NULL)
        dbm_close(count_fd);

    if (count_path != NULL)
        count_fd = dbm_open(count_path, O_RDWR|O_CREAT, 0644);

    mutex_unlock(&count_lock);
}


static const char *const month[] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


char *log_time_r(const time_t *btp, char *buf, int bufsize)
{
    struct tm l_tpb, g_tpb;
    int d, tz_h, tz_m;
    char *s;


    if (bufsize < 28)
        return NULL;

    if (localtime_r(btp, &l_tpb) == NULL)
        return NULL;

    if (gmtime_r(btp, &g_tpb) == NULL)
        return NULL;

    d = (l_tpb.tm_hour * 60 + l_tpb.tm_min) -
        (g_tpb.tm_hour * 60 + g_tpb.tm_min);

    if (d <= -12*60)
        d = 24*60 + d;
    else if (d >= 24*60)
        d = d - 24*60;

    if (d < 0)
    {
        s = "-";
        d = -d;
    }
    else if (d > 0)
        s = "+";
    else
        s = "";

    tz_h = d / 60;
    tz_m = d - (tz_h * 60);

    s_sprintf(buf, bufsize, "%02d/%s/%d:%02d:%02d:%02d %s%02d%02d",
              l_tpb.tm_mday,
              month[l_tpb.tm_mon],
              l_tpb.tm_year + 1900,
              l_tpb.tm_hour,
              l_tpb.tm_min,
              l_tpb.tm_sec,
              s,
              tz_h,
              tz_m);

    return buf;
}



void log_connection(struct connectioninfo *cip,
                    int result)
{
    struct httpinfo *hip = cip->hip;
    struct mimeinfo *mip = hip ? hip->mip : NULL;


    mutex_lock(&log_lock);

    if (log_fd >= 0)
    {
        char buf[256], buf1[2048], buf2[2048], buf3[2048], buf4[2048];
        char buf5[2048], buf6[2048];
        char *url, *request, *hostname;
        char *referer = NULL, *client = NULL;
        char *auth_user = NULL;
        char *ident_user = NULL;

        url = hip->orig_url;
        if (url)
            url = url_quote(hip->url, buf1, sizeof(buf1), "?", 0);

        request = hip->request;
        if (request)
            request = url_quote(request, buf2, sizeof(buf2), "\"", 1);

        if (mip)
        {
            referer = mime_getheader(mip, "REFERER", 1);
            client  = mime_getheader(mip, "USER-AGENT", 1);
        }

        if (referer)
            referer = url_quote(referer, buf3, sizeof(buf3), "", 0);

        if (client)
            client = url_quote(client, buf4, sizeof(buf4), "\"", 1);

        auth_user = cip->auth.user;
        if (auth_user)
            auth_user = url_quote(auth_user, buf5, sizeof(buf5), "", 0);

        ident_user = get_ident(&cip->client);
        if (ident_user)
            ident_user = url_quote(ident_user, buf6, sizeof(buf6), "", 0);

        hostname = get_hostname(&cip->client);

        fd_printf(log_fd,
                  "%s %s %s [%s] \"%s%s%s%s%s%s%s\" %d ",
                  hostname ? hostname : (cip->client.addr_s ? cip->client.addr_s : "-"),
                  ident_user ? ident_user : "-",
                  auth_user ? auth_user : "-",
                  log_time_r(&cip->cn_time, buf, sizeof(buf)),
                  hip->method ? hip->method : "",
                  url ? " " : "",
                  url ? url : "",
                  request ? "?" : "",
                  request ? request : "",
                  hip->version ? " " : "",
                  hip->version ? hip->version : "",
                  result);

        if (hip->length)
            fd_printf(log_fd, "%d", hip->length);
        else
            fd_putc('-', log_fd);

        if (extended_logging)
        {
            fd_printf(log_fd, " %s %s%s%s",
                      referer ? referer : "-",
                      client ? "\"" : "",
                      client ? client : "-",
                      client ? "\"" : "");
        }
        fd_putc('\n', log_fd);

        fd_flush(log_fd);
    }

    mutex_unlock(&log_lock);

    mutex_lock(&count_lock);

    if (result < 400 && count_fd != NULL && hip->orig_url)
    {
        struct count_t count;
        datum cnt_datum;
        datum url_datum;

        totals.totalhits++;
        totals.totalbytes += (unsigned long)(hip->length);
        totals.lastaccess = cip->cn_time;
        url_datum.dptr = "Totals";
        url_datum.dsize = strlen(url_datum.dptr);
        cnt_datum.dptr = (char*)&totals;
        cnt_datum.dsize = sizeof(struct count_t);
        dbm_store(count_fd, url_datum, cnt_datum, DBM_REPLACE);

        if (debug > 2)
            fprintf(stderr, "logger: counter: url=%s\n", hip->orig_url);

        url_datum.dptr = hip->orig_url;
        url_datum.dsize = strlen(url_datum.dptr);
        cnt_datum = dbm_fetch(count_fd, url_datum);

        if (cnt_datum.dptr)
        {
            memcpy(&count, cnt_datum.dptr, sizeof(struct count_t));
            count.totalhits++;
            count.totalbytes += (unsigned long)(hip->length);
            count.lastaccess = cip->cn_time;
            cnt_datum.dptr = (char*)&count;
            cnt_datum.dsize = sizeof(struct count_t);
            dbm_store(count_fd, url_datum, cnt_datum, DBM_REPLACE);
        }
        else
        {
            count.totalhits  = 1;
            count.totalbytes = hip->length;
            count.starttime =
                count.lastaccess = cip->cn_time;
            count.reserved  = 0;
            cnt_datum.dptr = (char*)&count;
            cnt_datum.dsize = sizeof(struct count_t);
            dbm_store(count_fd, url_datum, cnt_datum, DBM_INSERT);
        };
    }

    mutex_unlock(&count_lock);
}

void get_count(char *url, struct count_t *count)
{
    count->totalhits = count->totalbytes = 0;

    if (debug > 2)
        fprintf(stderr, "get_count(), url=%s\n", url);

    mutex_lock(&count_lock);

    if (count_fd != NULL && url)
    {
        datum cnt_datum;
        datum url_datum;

        url_datum.dptr = url;
        url_datum.dsize = strlen(url);
        cnt_datum = dbm_fetch(count_fd, url_datum);

        if (cnt_datum.dptr)
            memcpy(count, cnt_datum.dptr, sizeof(struct count_t));
        else
            dbm_clearerr(count_fd);
    }

    mutex_unlock(&count_lock);
}

void get_totals(struct count_t *count)
{
    count->totalhits  = totals.totalhits;
    count->totalbytes = totals.totalbytes;
    count->starttime  = totals.starttime;
    count->lastaccess  = totals.lastaccess;
    count->reserved = totals.reserved;
}

void print_totals(int fd)
{
    double days = (double)(totals.lastaccess - totals.starttime) /
                  (24.0*60.0*60.0);
    fd_printf(fd, "<PRE>\nFiles Transmitted During Summary Period %14lu\n",
              totals.totalhits);
    fd_printf(fd, "Bytes Transmitted During Summary Period %14lu\n",
              totals.totalbytes);
    fd_printf(fd, "Average Files Transmitted Daily         %14.0f\n",
              totals.totalhits/days);
    fd_printf(fd, "Average Bytes Transmitted Daily         %14.0f\n</PRE>\n",
              totals.totalbytes/days);
}

void print_counts(int fd)
{
    if (count_fd != NULL)
    {
        struct count_t count;
        datum url_datum, cnt_datum;
        double thits = totals.totalhits;
        double tbytes = totals.totalbytes;

        fd_puts("<PRE>\n", fd);
        fd_puts("%Reqs  %Byte  Bytes Sent   Requests  Archive Section\n", fd);
        fd_puts("------ ------ ------------ -------- |------------------------------------\n", fd);

        mutex_lock(&count_lock);

        for (url_datum = dbm_firstkey(count_fd);
                url_datum.dptr != NULL;
                url_datum = dbm_nextkey(count_fd))
        {
            if (strncmp(url_datum.dptr, "Totals", 6) == 0) continue;

            cnt_datum = dbm_fetch(count_fd, url_datum);
            if (cnt_datum.dptr)
            {
                memcpy(&count, cnt_datum.dptr, sizeof(struct count_t));
                fd_printf(fd, "%6.2f %6.2f %12d %8d | %.*s\n",
                          100.0*(double)(count.totalhits)/thits,
                          100.0*(double)(count.totalbytes)/tbytes,
                          count.totalbytes, count.totalhits,
                          url_datum.dsize, url_datum.dptr);
            }
            else
                break;

        }

        mutex_unlock(&count_lock);

        fd_puts("</PRE>\n", fd);
    }
}


