/*
** mime.h
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

#ifndef PHTTPD_MIME_H
#define PHTTPD_MIME_H

#define MCF_KEEPALIVE  0x0001

#define MPF_NOCACHE    0x0001

struct mimeinfo
{
    int version;
    hashtable_t table;

    unsigned int connection_flags;
    unsigned int pragma_flags;
};


struct mimeinfo_old
{
    int version;

    char *from;
    int connection_flags;
    char *host;
    struct table *accept;
    struct table *accept_encoding;
    struct table *accept_language;
    char *user_agent;
    char *referer;
    char *authorization;
    char *charge_to;
    char *if_modified_since;
    char *pragma;

    char *content_type;
    int   content_length;
};


extern struct mimeinfo *mime_getheaders(int fd);

extern void mime_freeheaders(struct mimeinfo *mip);

extern void mime_writeheaders(int fd, struct mimeinfo *mip);

extern int mime_getline(char *buf, int size, int fd);

extern void *mime_getheader(struct mimeinfo *mip,
                            const char *header,
                            int str_flag);
#endif
