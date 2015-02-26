/*
** autofail.h
**
** Copyright (c) 1995-1997 Peter Eriksson <pen@signum.se>
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

#ifndef PHTTPD_AUTOFAIL_H
#define PHTTPD_AUTOFAIL_H


#define strcpy(a,b)	!&! DO_NOT_USE_STRCPY
#define strcat(a,b)	!&! DO_NOT_USE_STRCAT
#define strncpy(a,b,c)	!&! DO_NOT_USE_STRNCPY

#define malloc(a)	!&! DO_NOT_USE_MALLOC
#define calloc(a,b)	!&! DO_NOT_USE_CALLOC
#define free(a)		!&! DO_NOT_USE_FREE
#define realloc(a,b)	!&! DO_NOT_USE_REALLOC
#define strdup(a)	!&! DO_NOT_USE_STRDUP

#define readlink(a,b)	!&! DO_NOT_USE_READLINK
#define unlink(a)	!&! DO_NOT_USE_UNLINK
#define stat(a,b)	!&! DO_NOT_USE_STAT
#define lstat(a,b)	!&! DO_NOT_USE_LSTAT

#define abort()		!&! DO_NOT_USE_ABORT

#define open(a,b)	!&! DO_NOT_USE_OPEN
#define close(a)	!&! DO_NOT_USE_CLOSE
#define read(a,b,c)	!&! DO_NOT_USE_READ
#define write(a,b,c)	!&! DO_NOT_USE_WRITE

#define shutdown(a,b)	!&! DO_NOT_USE_SHUTDOWN
#define connect(a,b,c) 	!&! DO_NOT_USE_CONNECT
#define select(a,b,c,d)	!&! DO_NOT_USE_SELECT
#define ioctl(a,b,c)	!&! DO_NOT_USE_IOCTL

#define sleep(a)	!&! DO_NOT_USE_SLEEP
#define chdir(a)	!&! DO_NOT_USE_CHDIR
#define chroot(a)	!&! DO_NOT_USE_CHROOT
#define dup2(a,b)	!&! DO_NOT_USE_DUP2
#define waitpid(a,b)	!&! DO_NOT_USE_WAITPID
#define execve(a,b,c)	!&! DO_NOT_USE_EXECVE

#define system(a)	!&! DO_NOT_USE_SYSTEM
#define popen(a,b)	!&! DO_NOT_USE_POPEN
#define fopen(a,b)	!&! DO_NOT_USE_FOPEN

#define sprintf(a,b)	!&! DO_NOT_USE_SPRINTF
#define vsprintf(a,b)	!&! DO_NOT_USE_VSPRINTF

#define gethostbyaddr(a) !&! DO_NOT_USE_GETHOSTBYADDR
#define gethostbyname(a) !&! DO_NOT_USE_GETHOSTBYNAME

#endif
