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

#ifndef PHTTPD_LOGGER_H
#define PHTTPD_LOGGER_H

struct connectioninfo;
struct httpinfo;

struct count_t
{
    unsigned long totalhits;
    unsigned long totalbytes;
    time_t starttime;
    time_t lastaccess;
    unsigned long reserved;
};

extern void log_init(char *pathname, char *cnt_pathname);

extern void log_reopen(void);

extern char *log_time_r(const time_t *btp,
                        char *buf,
                        int bufsize);

extern void log_connection(struct connectioninfo *cip,
                           int result);

extern void get_count(char *url, struct count_t *count);

extern void get_totals(struct count_t *count);

extern void print_totals(int fd);

extern void print_counts(int fd);

#endif



