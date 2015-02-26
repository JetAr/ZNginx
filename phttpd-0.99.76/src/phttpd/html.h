/*
** html.h
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

#ifndef PHTTPD_HTML_H
#define PHTTPD_HTML_H

extern void html_href(int fd,
                      const char *href,
                      const char *text);

extern void html_email(int fd,
                       const char *address);

extern void html_sysheader(int fd,
                           const char *htype,
                           const char *format,
                           ...);

extern void html_sysfooter(int fd);

extern void html_error(int fd,
                       const char *type,
                       const char *format,
                       va_list ap);


extern char *html_strquote(const char *str,
                           char *buf,
                           int size);

#endif
