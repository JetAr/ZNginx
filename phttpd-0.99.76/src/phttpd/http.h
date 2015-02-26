/*
** http.h
**
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

#ifndef PHTTPD_HTTP_H
#define PHTTPD_HTTP_H

struct connectioninfo;
struct mimeinfo;


struct httpinfo
{
    char *method;
    char *url;
    char *orig_url;
    char *request;
    char *orig_request;
    char *version;

    struct mimeinfo *mip;

    /* Result Parameters */
    int length;
};

extern int http_get_request(struct connectioninfo *cip,
                            int *keepalive);

extern void http_freeinfo(struct httpinfo *hip);


extern int http_error(struct connectioninfo *cip,
                      int code,
                      const char *message,
                      ...);

extern int http_redirect(struct connectioninfo *cip,
                         const char *url,
                         const char *request,
                         const char *orig_req,
                         int code);

extern int http_not_modified(struct connectioninfo *cip);

extern int http_precondition_failed(struct connectioninfo *cip);

extern char *http_time_r(const time_t *btp, char *buf, int bufsize);


extern void http_extract_request(char *url, char **requestp);

extern char *http_sendheaders(int fd, struct connectioninfo *cip,
                              int code, char *type);

extern void http_sendlastmodified(int fd, time_t ct);

extern void http_sendlang(int, const char *);

#endif
