/*
** http.c
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
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdarg.h>
#include <limits.h>
#include <time.h>
#include <syslog.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "phttpd.h"



int phttpd_call(const char *module,
                struct connectioninfo *cip)
{
    struct modinfo *mp;
    void *mkey;
    int status;
    char buf[2048];


    while (*module && *module == '/')
        ++module;

    s_strcpy(buf, sizeof(buf), modules_home);
    s_strcat(buf, sizeof(buf), "/");
    s_strcat(buf, sizeof(buf), module);

    mp = md_load(buf, &mkey);
    if (mp == NULL)
    {
        return http_error(cip, 500,
                          "A system error occured while loading module <B>%s</B>:<BR><PRE>%s</PRE>",
                          module, md_error());
    }

    status = mp->pm_request(cip);

    md_release(mkey);
    return status;
}

int phttpd_request(struct connectioninfo *cip)
{
    const char *handler;


    if (cip->hip == NULL)
        return -1;

    handler = url_gethandler(cip->hip->url);
    if (handler == NULL)
        return -1;
    return phttpd_call(handler, cip);
}


void http_sendlastmodified(int fd, time_t ct)
{
    char buf[64];

    fd_puts("Last-Modified: ", fd);
    fd_puts(http_time_r(&ct, buf, sizeof(buf)), fd);
}

char *http_sendheaders(int fd,
                       struct connectioninfo *cip,
                       int code,
                       char *type)
{
    char buf[256];
    time_t ct;
    int keepalive;


    ct = cip->cn_time;

    if (cip->hip && cip->hip->mip)
        keepalive = cip->hip->mip->connection_flags & MCF_KEEPALIVE;
    else
        keepalive = 0;

    if (ct == 0)
        time(&ct);

    if (type == NULL)
        switch (code)
        {
        case 200:
            type = "OK";
            break;

        case 201:
            type = "Created";
            break;

        case 202:
            type = "Accepted";
            break;

        case 203:
            type = "Provisional Information";
            break;

        case 204:
            type = "No Content";
            break;

        case 206:
            type = "Partial Content";
            break;

        case 300:
            type = "Multiple Choices";
            break;

        case 301:
            type = "Moved Permanently";
            break;

        case 302:
            type = "Moved Temporarily";
            break;

        case 303:
            type = "Method";
            break;

        case 304:
            type = "Not Modified";
            break;

        case 400:
            type = "Bad Request";
            break;

        case 401:
            type = "Unauthorized";
            break;

        case 402:
            type = "Payment Forbidden";
            break;

        case 403:
            type = "Forbidden";
            break;

        case 404:
            type = "Not Found";
            break;

        case 405:
            type = "Method Not Allowed";
            break;

        case 406:
            type = "None Acceptable";
            break;

        case 407:
            type = "Proxy Authentication Required";
            break;

        case 408:
            type = "Request Timeout";
            break;

        case 409:
            type = "Conflict";
            break;

        case 410:
            type = "Gone";
            break;

        case 412:
            type = "Precondition Failed";
            break;

        case 500:
            type = "Internal Server Error";
            break;

        case 501:
            type = "Not Implemented";
            break;

        case 502:
            type = "Bad Gateway";
            break;

        case 503:
            type = "Service Unavailable";
            break;

        case 504:
            type = "Gateway Timeout";
            break;

        default:
            type = "?";
        }

    fd_puts("HTTP/1.0 ", fd);
    fd_puti(code, fd);
    fd_putc(' ', fd);
    fd_puts(type, fd);
    fd_putc('\n', fd);

    fd_puts("MIME-Version: 1.0\n", fd);

    fd_puts("Date: ", fd);
    fd_puts(http_time_r(&ct, buf, sizeof(buf)), fd);

    fd_puts("Server: phttpd/", fd);
    fd_puts(server_version, fd);
    fd_putc('\n', fd);

    if (keepalive && (keepalive_timeout > 0 || keepalive_maxreq > 0))
    {
        fd_puts("Connection: Keep-Alive", fd);
        if (keepalive_timeout > 0)
            fd_printf(fd, ", timeout=%d", keepalive_timeout);
        if (keepalive_maxreq > 0)
            fd_printf(fd, ", maxreq=%d", keepalive_maxreq);
        fd_putc('\n', fd);
    }

    return type;
}

int http_unauthorized(struct connectioninfo *cip, const char *realm)
{
    int result;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
        fprintf(stderr, "      Sent UNAUTHORIZED message\n");

    result = 401;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);
        fd_printf(fd, "WWW-Authenticate: Basic realm=\"%s\"\n", realm);
        fd_putc('\n', fd);
    }

    if(strcasecmp(hip->method, "HEAD") == 0)
        return result;

    html_sysheader(fd, "H2", "Unauthorized");

    fd_printf(fd, "Please send authorization codes for realm \"%s\".\n",
              realm);

    html_sysfooter(fd);

    return result;
}



int http_forbidden(struct connectioninfo *cip)
{
    int result;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
        fprintf(stderr, "      Sent FORBIDDEN message\n");

    result = 403;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);
        fd_putc('\n', fd);
    }

    if(strcasecmp(hip->method, "HEAD") == 0)
        return result;

    html_sysheader(fd, "H2", "Access Denied");

    fd_printf(fd, "Your request to ");
    if (hip->orig_url == NULL)
        fd_printf(fd, "[null]");
    else
        html_href(fd, hip->orig_url + cip->vspl, hip->orig_url + cip->vspl );

    fd_printf(fd, " was denied.\n");

    html_sysfooter(fd);

    return result;
}


int http_authenticate(struct connectioninfo *cip,
                      const char *path,
                      const char *realm,
                      char *request)
{
    if (!access_auth(cip, path))
        return http_unauthorized(cip, realm);

    return 0;
}


int http_access(struct connectioninfo *cip,
                const char *auth_path,
                char *request)
{
    if (!access_check(auth_path, cip))
        return error_access(cip);

    return 0;
}


/*
 *  Extract the request (the part after the first '?') in URL and add
 *  it to the beginning of *REQUESTP.
 */
void
http_extract_request(char *url, char **requestp)
{
    char *cp;

    if (url == NULL)
        return;

    cp = strchr(url, '?');
    if (cp != NULL)
    {
        *cp++ = '\0';
        if (requestp == NULL)
            return;

        if (!*requestp)
        {
            *requestp = s_strdup(cp);
        }
        else
        {
            char *oldrequest = *requestp;
            size_t oldlen = strlen(oldrequest);
            size_t addlen = strlen(cp);

            *requestp = s_malloc(oldlen + 1 + addlen + 1);
            memcpy(*requestp, oldrequest, oldlen);
            *(*requestp + oldlen) = '?';
            memcpy(*requestp + oldlen + 1, cp, addlen);
            *(*requestp + oldlen + 1 + addlen) = '\0';
            s_free(oldrequest);
        }
    }
}



int http_get_request(struct connectioninfo *cip,
                     int *keepalive)
{
    char buf[2048], *lastcp, *url, *req;
    struct httpinfo *hip = NULL;
    int result = -1;
    ucentry_t *ucp;
    urlinfo_t *uip;
    char *realm;
    char *mvsxy; /* RK MVS */


    ucp = NULL;
    uip = NULL;

    if (debug > 1)
        fprintf(stderr, "http_get_request(), fd=%d\n", cip->fd);

    if ((req = fd_gets(NULL, maxurlsize, cip->fd)) == NULL)
    {
        if (debug > 2)
            fprintf(stderr, "\t->Returning failed (fd=%d)\n", cip->fd);
        return -1;
    }

    hip = s_malloc(sizeof(struct httpinfo));

    hip->method = strtok_r(req, " \t\n\r", &lastcp);
    if (hip->method == NULL)
    {
        s_free(hip);
        return -1;
    }

    url = strtok_r(NULL, " \t\n\r", &lastcp);
    if (url)
        hip->version = s_strdup(strtok_r(NULL, " \t\n\r", &lastcp));

    /* Added for Multi-Virtual-Server RK */
    /* YES IT IS UGLY, BUT.... */
    if ( rkmultimode == 0 ) /* NO MULTIMODE */
        cip->vspl=0;
    else
    {
        if (url)
        {
            int len;

            if ( debug >6 )
                fprintf(stderr,"MVS-RK: Adding prefix\n");
            len = strlen(url)+10;
            mvsxy=s_malloc(len);
            if ( url[0]=='/' )
                s_sprintf(mvsxy, len, "/%lx%s", cip->serverif, url);
            else
                s_sprintf(mvsxy, len, "/%lx/%s", cip->serverif, url);

            /* lenght of prefix is 8 + 1 (still fix !) */
            cip->vspl=9; /* RK */
            if ( debug >6 )
                fprintf(stderr,"MVS-RK: calling free\n");

            s_strcpy(url,strlen(url)+10, mvsxy);

            url[strlen(mvsxy)]=0x00; /* wg.logfile */
            s_free(mvsxy);
            if ( debug >6 )
                fprintf(stderr,"MVS-RK: prefix done\n");
        }
    }
    /* END RK */

    if (debug > 4)
        fprintf(stderr, "Method = %s, URL = %s\n",
                hip->method, url ? url : "<null>");

    if (url == NULL)
    {
        s_free(hip->method);
        s_free(hip);
        return -1;
    }

    cip->hip = hip;

    http_extract_request(url, &hip->request);
    hip->orig_url = s_strdup(url);
    hip->orig_request = s_strdup(hip->request);
    url_unquote(url, 0);

    if (debug > 2)
        fprintf(stderr, "url=%s\norig_url=%s\nreq=%s\norig_req=%s\n",
                url ? url : "<null>",
                hip->orig_url ? hip->orig_url : "<null>",
                hip->request ? hip->request : "<null>",
                hip->orig_request ? hip->orig_request : "<null>");

    if (hip->version != NULL)
    {
        hip->mip = mime_getheaders(cip->fd);
        if (hip->mip == NULL)
        {
            error_bad_request(cip, "Invalid MIME headers");
            goto End;
        }

        if (keepalive && hip->mip &&
                (hip->mip->connection_flags & MCF_KEEPALIVE))
            *keepalive = 1;
    }

    if (strcasecmp(hip->method, "PING") == 0)
    {
        if (hip->mip)
        {
            http_sendheaders(cip->fd, cip, 200, NULL);
            result = 200;
            fd_putc('\n', cip->fd);
        }
        fd_puts("PONG\n", cip->fd);
        goto End;
    }

    if (hip->mip && (hip->mip->pragma_flags & MPF_NOCACHE))
        ucp = urlcache_lookup(url, UCF_ALL+UCF_RELOAD);
    else
        ucp = urlcache_lookup(url, UCF_ALL);

    if (ucp == NULL)
        goto End;

    uip = ucp->uip;
    if (uip == NULL)
    {
        urlcache_release(ucp);
        goto End;
    }

    hip->url = s_strdup(uip->rewrite.url);

    if (uip->rewrite.request)
    {
        if (hip->request)
            s_free(hip->request);
        hip->request = s_strdup(uip->rewrite.request);
    }

    if (uip->redirect.url)
    {
        result = http_redirect(cip,
                               uip->redirect.url,
                               uip->redirect.request,
                               hip->request,
                               302);
        goto End;
    }

    if (uip->predirect.url)
    {
        result = http_redirect(cip,
                               uip->predirect.url,
                               uip->predirect.request,
                               hip->request,
                               301);
        goto End;
    }

    if (uip->access.path)
    {
        result = http_access(cip, uip->access.path, hip->request);
        if (result != 0)
            goto End;
    }


    /* ADDED by RK */
    if (debug > 4)
        fprintf(stderr,"Check write ACCESS\n");

    if (write_needs_auth &&
            (strcasecmp(hip->method, "PUT")==0 ||
             strcasecmp(hip->method, "DELETE")==0))
    {
        if (debug > 4)
            fprintf(stderr,"We have a write CMD...\n");

        if (write_getauthenticate(url, buf, sizeof(buf)))
        {
            uip->auth.source=s_strdup(buf);
            realm = uip->auth.source;
            if (realm)
            {
                while (*realm && !s_isspace(*realm))
                    ++realm;
                if (s_isspace(*realm))
                    *realm++ = '\0';
                while (*realm && s_isspace(*realm))
                    ++realm;
            }
            uip->auth.realm = realm;
            uip->flags |= UCF_AUTH;
            if ( debug > 4 )
                fprintf(stderr,"Using auth for...%s\n",uip->auth.source);
        }
        else
        {
            uip->auth.source=NULL;
            result = -403;
            goto End;
        }
    } /* end of write_needs_auth */

    if (uip->auth.source)
    {
        result = http_authenticate(cip,
                                   uip->auth.source,
                                   uip->auth.realm,
                                   hip->request);
        if (result != 0)
            goto End;
    }

    if (uip->handler)
        result = phttpd_call(uip->handler, cip);

    if (debug > 3)
        fprintf(stderr, "MAIN-RETURN: Result=%d\n", result);

End:
    if (result < 0)
        switch (result)
        {
        case -1:
            result = error_not_found(cip);
            break;

        case -403:
            result = error_access(cip);
            break;

        case -405:
            result = error_method_denied(cip);
            break;

        default:
            result = error_bad_request(cip, hip->method);
            syslog(LOG_NOTICE, "bad HTTP request (#%d) method: %s",
                   cip->request_no,
                   hip->method);
        }

    if (ucp)
        urlcache_release(ucp);

    if (hip->length == 0)
        *keepalive = 0;

    if (debug > 1)
        fprintf(stderr, "http_get_request(), fd=%d, returning %d\n",
                cip->fd, result);

    return result;
}


void http_freeinfo(struct httpinfo *hip)
{
    if (hip->method)
        s_free(hip->method);

    if (hip->url)
        s_free(hip->url);

    if (hip->request)
        s_free(hip->request);

    if (hip->orig_url)
        s_free(hip->orig_url);

    if (hip->orig_request)
        s_free(hip->orig_request);

    if (hip->version)
        s_free(hip->version);

    if (hip->mip)
        mime_freeheaders(hip->mip);

    s_free(hip);
}

int http_error(struct connectioninfo *cip,
               int code,
               const char *format,
               ...)
{
    va_list ap;
    char *type, buf[256];
    struct httpinfo *hip = cip->hip;


    if (hip->mip != NULL)
    {
        type = http_sendheaders(cip->fd, cip, code, NULL);
        fd_puts("Content-Type: text/html\n", cip->fd);
        fd_putc('\n', cip->fd);
    }
    else
    {
        int blen;

        blen = s_strcpy(buf, sizeof(buf), "Error Code #");
        s_sprintf(buf+blen, sizeof(buf)-blen, "%d", code);
        type = buf;
    }

    if (strcasecmp(hip->method, "HEAD") == 0)
        return code;

    va_start(ap, format);

    html_error(cip->fd, type, format, ap);

    va_end(ap);

    return code;
}

void http_sendlang(int fd, const char *url)
{
    int i;
    char **pair;

    if(content_language_table == NULL)
        return;

    for(i = 0; i < content_language_table->length; ++i)
    {
        pair = content_language_table->value[i];
        if(strmatch(url, pair[0]))
        {
            fd_puts("Content-Language: ", fd);
            fd_puts(pair[1], fd);
            fd_putc('\n', fd);
            break;
        }
    }
}


/* Send a redirect pointer */
int http_redirect(struct connectioninfo *cip,
                  const char *url,
                  const char *request,
                  const char *orig_req,
                  int code)
{
    char buf1[2048], buf2[2048], buf3[2048];
    int result, len;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    url = url_quote(url, buf1, sizeof(buf1), "?", 0);
    if (url == NULL)
        return -1;

    if (request)
    {
        request = url_quote(request, buf2, sizeof(buf2), "\"", 1);
        if (request == NULL)
            return -1;
    }

    if (orig_req)
    {
        orig_req = url_quote(orig_req, buf3, sizeof(buf3), "\"", 1);
        if (orig_req == NULL)
            return -1;
    }

    if (debug > 1)
        fprintf(stderr, "      Sent URL REDIRECT (%s -> %s)\n",
                hip->url, url);

    result = code;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);

        fd_printf(fd, "Location: %s%s%s%s\n",
                  url,
                  (orig_req || request) ? "?" : "",
                  orig_req ? orig_req : "",
                  request ? request : "");


        http_sendlastmodified(fd, cip->cn_time);
        fd_puts("Content-Type: text/html\n\n", fd);
    }

    if (strcasecmp(hip->method, "HEAD") == 0)
        return result;

    len = fd_written(fd);

    html_sysheader(fd, "H2", "Document moved");

    fd_puts("The document has moved to this URL:<P>\n", fd);
    fd_puts("<UL>\n", fd);

    fd_printf(fd, "<B><A HREF=\"%s%s%s%s\">%s%s%s%s</A></B>.\n",
              url,
              (orig_req || request) ? "?" : "",
              orig_req ? orig_req : "",
              request ? request : "",
              url,
              (orig_req || request) ? "?" : "",
              orig_req ? orig_req : "",
              request ? request : "");

    fd_puts("</UL>\n", fd);

    html_sysfooter(fd);

    hip->length = fd_written(fd) - len;

    return result;
}


/* Send a 'Not Modified' message */
int http_not_modified(struct connectioninfo *cip)
{
    int result;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
        fprintf(stderr, "      Sent NOT MODIFIED message\n");

    result = 304;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);
        fd_putc('\n', fd);
    }

    return result;
}


/* Send a 'Precondition Failed' message */
int http_precondition_failed(struct connectioninfo *cip)
{
    int result;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
        fprintf(stderr, "      Sent PRECONDITION FAILED message\n");

    result = 412;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd, cip, result, NULL);
        fd_putc('\n', fd);
    }

    if(strcasecmp(hip->method, "HEAD") == 0)
        return result;

    html_sysheader(fd, "H2", "Precondition Failed");

    fd_printf(fd, "The entity which lives at ");
    if (hip->orig_url == NULL)
        fd_printf(fd, "[null]");
    else
        html_href(fd, hip->orig_url + cip->vspl, hip->orig_url + cip->vspl );

    fd_printf(fd, " was changed.\n");

    html_sysfooter(fd);

    return result;
}


static const char *const weekday[] =
{
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static const char *const month[] =
{
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};


char *http_time_r(const time_t *btp, char *buf, int bufsize)
{
    struct tm tpb;


    if (bufsize < 31)
        return NULL;

    if (gmtime_r(btp, &tpb) == NULL)
        return NULL;

    s_sprintf(buf, bufsize, "%s, %02d %s %d %02d:%02d:%02d GMT\n",
              weekday[tpb.tm_wday],
              tpb.tm_mday,
              month[tpb.tm_mon],
              tpb.tm_year + 1900,
              tpb.tm_hour,
              tpb.tm_min,
              tpb.tm_sec);

    return buf;
}
