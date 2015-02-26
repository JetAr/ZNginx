/*
** error.c
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
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>

#include "phttpd.h"


hashtable_t *error_page_table = NULL;

/* Returns -1 in case of error, else 0 */
static int make_full_url(struct connectioninfo *cip, char *buf, int len)
{
    int i = 0;
    u_long addr;
    struct hostent *hp, hb;
    char buf1[2048];
    char buf2[2048];
    int h_errno;


    /* Need to rewrite 'SERVER_URL' , will use buf1 instead of server_url */
    if (rkmultimode)
    {
        addr=cip->serverif;
        hp = s_gethostbyaddr_r((char *)&addr, sizeof (addr), AF_INET,
                               &hb, buf2, sizeof(buf2), &h_errno);

        if ( hp != NULL )
        {
            if (server_port == 80)
            {
                s_strcpy(buf1, sizeof(buf1), "http://");
                s_strcat(buf1, sizeof(buf1), hp->h_name);
            }
            else
            {
                int len;


                s_strcpy(buf1, sizeof(buf1), "http://");
                s_strcat(buf1, sizeof(buf1), hp->h_name);
                len = s_strcat(buf1, sizeof(buf1), ":");
                s_sprintf(buf1+len, sizeof(buf1)-len, "%d", server_port);
            }
        }
    }

    if (cip->hip->orig_url[0] == '/')
        i = s_strcpy(buf, len, (!rkmultimode) ? server_url : buf1);

    if (url_quote(cip->hip->orig_url+cip->vspl, buf+i, len-i, "?", 0) == NULL)
        return -1;

    i = strlen(buf);

    if (cip->hip->orig_request)
    {
        buf[i++] = '?';
        if (url_quote(cip->hip->orig_request, buf+i, len-i, "\"", 1) == NULL)
            return -1;
        i = strlen(buf);
    }

    buf[i] = '\0';
    return 0;
}



int error_bad_request(struct connectioninfo *cip,
                      const char *buf)
{
    hashentry_t *hep;
    char buf1[2048];
    int len, code;


    hep = ht_lookup(error_page_table, "400", 0);
    if (hep == NULL)
        hep = ht_lookup(error_page_table, "*", 1);

    if (hep)
    {
        len = s_strcpy(buf1, sizeof(buf1), "code=400&url=");

        if (make_full_url(cip, buf1+len, sizeof(buf1)-len) < 0)
            return -1;

        len = s_strcat(buf1, sizeof(buf1), "request=");

        url_quote(buf, buf1+len, sizeof(buf1)-len, "\"", 1);

        code = http_redirect(cip, hep->data, buf1, NULL, 302);
        ht_release(hep);
        return code;
    }
    else
        return http_error(cip, 400,
                          "Bad HTTP request:<BR><PRE>%s</PRE>\n", buf);
}



int error_not_found(struct connectioninfo *cip)
{
    char buf1[2048];
    hashentry_t *hep;
    int len, code;


    hep = ht_lookup(error_page_table, "404", 0);
    if (hep == NULL)
        hep = ht_lookup(error_page_table, "*", 1);

    if (hep)
    {
        len = s_strcpy(buf1, sizeof(buf1), "code=404&url=");

        if (make_full_url(cip, buf1+len, sizeof(buf1)-len) < 0)
        {
            syslog(LOG_ERR, "error_not_found: make_full_url() failed");
            return -1;
        }

        code = http_redirect(cip, hep->data, buf1, NULL, 302);

        ht_release(hep);
        return code;
    }
    else
    {
        if (make_full_url(cip, buf1, sizeof(buf1)) < 0)
        {
            syslog(LOG_ERR, "error_not_found: make_full_url() failed");
            return -1;
        }

        return http_error(cip, 404,
                          "The requested URL, <A HREF=\"%s\">%s</A>, was not found.",
                          buf1, buf1);
    }
}


int error_method_denied(struct connectioninfo *cip)
{
    char buf1[2048];
    hashentry_t *hep;
    int len, code;


    hep = ht_lookup(error_page_table, "405", 0);
    if (hep == NULL)
        hep = ht_lookup(error_page_table, "*", 1);

    if (hep)
    {
        len = s_strcpy(buf1, sizeof(buf1), "code=405&url=");

        if (make_full_url(cip, buf1+len, sizeof(buf1)-len) < 0)
            return -1;

        code = http_redirect(cip, hep->data, buf1, NULL, 302);
        ht_release(hep);
        return code;
    }
    else
    {
        if (make_full_url(cip, buf1, sizeof(buf1)) < 0)
            return -1;

        return http_error(cip, 405,
                          "Method Denied for URL <A HREF=\"%s\">%s</A>",
                          buf1,buf1);
    }
}

int error_access(struct connectioninfo *cip)
{
    char buf1[2048];
    hashentry_t *hep;
    int len, code;

    hep = ht_lookup(error_page_table, "403", 0);

    if (hep == NULL)
        hep = ht_lookup(error_page_table, "*", 1);

    if (hep)
    {
        len = s_strcpy(buf1, sizeof(buf1), "code=403&url=");

        if (make_full_url(cip, buf1+len, sizeof(buf1)-len) < 0)
            return -1;

        code = http_redirect(cip, hep->data, buf1, NULL, 302);

        ht_release(hep);
        return code;
    }
    else
    {
        if (make_full_url(cip, buf1, sizeof(buf1)) < 0)
            return -1;

        return http_error(cip, 403,
                          "Acess Denied for URL <A HREF=\"%s\">%s</A>",
                          buf1, buf1);
    }
}

int error_system(struct connectioninfo *cip,
                 const char *prompt)
{
    return http_error(cip, 500,
                      "A system error occured:<BR><PRE>%s: %s</PRE>",
                      prompt, strerror(errno));
}
