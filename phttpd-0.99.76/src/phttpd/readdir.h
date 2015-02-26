/*
** readdir.h
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

#ifndef PHTTPD_READDIR_H
#define PHTTPD_READDIR_H

#ifdef USE_GETDENTS
typedef struct
{
    int fd;
    int len;
    char buf[8192];
    struct dirent *dep;
} S_DIR;
#else
typedef struct
{
    struct dirent deb;
    DIR *dp;
} S_DIR;
#endif

extern S_DIR *s_opendir(const char *path);
extern struct dirent *s_readdir(S_DIR *dp);
extern void s_closedir(S_DIR *dp);

#endif
