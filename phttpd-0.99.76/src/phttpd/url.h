/*
** url.h
**
** Copyright (c) 1994 Peter Eriksson <pen@signum.se>
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

#ifndef __URL_H__
#define __URL_H__

#include <sys/stat.h>

extern struct table *url_handlers_table;
extern struct table *url_redirects_table;
extern struct table *url_predirects_table;
extern struct table *url_rewrites_table;
extern struct table *read_authenticate_table;
extern struct table *write_authenticate_table;
extern struct table *host_access_table;
extern struct table *url_expand_table;
extern struct table *content_language_table;



extern char *url_expand(const char *path,
                        char *realpath,
                        int maxsize,
                        struct stat *sp,
                        uid_t *uid, gid_t *gid);

extern char *url_getparent(const char *path, char *buf, int bufsize);

extern const char *url_gethandler(char *url);

extern char *url_match(struct table *tp,
                       const char *url,
                       char *result,
                       int size);

extern char *url_getredirect(const char *url, char *out_url, int size);
extern char *url_getpredirect(const char *url, char *out_url, int size);
extern char *read_getauthenticate(const char *url, char *out_url, int size);
extern char *write_getauthenticate(const char *url, char *out_url, int size);
extern char *host_getaccess(const char *url, char *out_url, int size);
extern char *url_getrewrite(const char *url, char *out_url, int url_size);

extern char *url_quote(const char *from,
                       char *to,
                       int size,
                       char *quotemap,
                       int only_qm);

extern void url_unquote(char *str, int plus_flag);

extern char *url_rewrite(const char *url,
                         const char *pattern,
                         char *buf,
                         int size);

#endif
