/*
** safeio.h
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

#ifndef PHTTPD_SAFEIO_H
#define PHTTPD_SAFEIO_H

#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <synch.h>
#include <dirent.h>
#include <pwd.h>
#ifndef __osf__
#include <shadow.h>
#endif
#include <grp.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/stat.h>

extern void s_sleep(int seconds);

extern int s_open(); /* Can't use prototypes here */

extern int s_close(int fd);
extern int s_shutdown(int fd, int how);

extern int s_connect(int fd, struct sockaddr *sin, int len);
extern int s_accept(int fd, struct sockaddr *sin, int *lenp);

extern int s_ioctl(int fd, int what, void *ptr);

extern int s_select(int nfd,
                    fd_set *rs, fd_set *ws, fd_set *es,
                    struct timeval *tv);

extern int s_read(int fd, char *buf, int len);
extern int s_write(int fd, const char *buf, int len);

extern sema_t *s_sema_create(int count, int type);
extern void s_sema_destroy(sema_t *sp, int type);
extern pid_t s_sema_wait(sema_t *sp);

extern mutex_t *s_mutex_create(int type);
extern void s_mutex_destroy(mutex_t *mp, int type);

extern int s_chdir(const char *path);
extern int s_chroot(const char *path);

extern int s_dup2(int fd1, int fd2);

extern pid_t s_waitpid(pid_t pid, int *status, int options);

extern int s_execve(const char *path, char *const argv[], char *const envp[]);

#ifndef __osf__
extern struct spwd *s_getspnam_r(const char *name,
                                 struct spwd *res,
                                 char *buf,
                                 int buflen);
#endif

extern struct passwd *s_getpwnam_r(const char *name,
                                   struct passwd *res,
                                   char *buf,
                                   int buflen);

extern struct passwd *s_getpwuid_r(uid_t uid,
                                   struct passwd *res,
                                   char *buf,
                                   int buflen);

extern struct group *s_getgrnam_r(const char *name,
                                  struct group *res,
                                  char *buf,
                                  int buflen);

extern struct hostent *s_gethostbyaddr_r(const char *addr,
        int length,
        int type,
        struct hostent *result,
        char *buffer,
        int buflen,
        int *h_errnop);

extern struct hostent *s_gethostbyname_r(const char *name,
        struct hostent *result,
        char *buffer,
        int buflen,
        int *h_errnop);


struct allocstats
{
    mutex_t lock;
    unsigned long malloc_calls;
    unsigned long free_calls;
    unsigned long realloc_calls;
    unsigned long bytes;
    unsigned long blocks;
};

#ifdef INCLUDE_ALLOC_STATS
extern struct allocstats alloc_stats;
#endif

extern void *s_malloc(size_t size);
extern void s_free(void *ptr);
extern void *s_realloc(void *ptr, size_t size);
extern char *s_strdup(const char *str);
extern char *s_strxdup(const char *str, int extra, int *size);

extern int s_sprintf(char *buf, int size, const char *format, ...);
extern int s_vsprintf(char *buf, int size, const char *format, va_list ap);

#define NEW(ptr)    (ptr = s_malloc(sizeof(*ptr)))


extern int s_readlink(const char *path, char *buf, int buflen);
extern int s_unlink(const char *path);

extern void s_abort(void);

extern int s_stat(const char *path, struct stat *buf);
extern int s_lstat(const char *path, struct stat *buf);

extern int s_nice(int nice);

/*
** Safe string handling functions. Dstsize is the buffer size of dst
** (ie, including the terminating NUL)
*/
extern int s_strcpy(char *dst, int dstsize, const char *src);
extern int s_strncpy(char *dst, int dstsize, const char *src, int maxcopy);
extern int s_strcat(char *dst, int dstsize, const char *src);
extern int s_strncat(char *dst, int dstsize, const char *src, int maxcopy);


extern int s_tmpname(char *buf, int bufsize);
extern int s_tmpfile(char *buf, int bufsize);


#define s_isdigit(c)	((c)>='0' && (c)<='9')
#define s_isspace(c)	((c)==' ' || (c)=='\t' || (c)=='\r' || (c)=='\n')
#define s_isalnum(c)	(s_isdigit(c) || s_isalpha(c))
#define s_isxdigit(c)	(s_isdigit(c) || (((c)>='a' && (c)<='f') || ((c)>='A' && (c)<='F')))

#define s_islower(c)	((c)>='a' && (c)<='z')
#define s_isupper(c)	((c)>='A' && (c)<='Z')
#define s_isalpha(c)	(s_islower(c) || s_isupper(c))

#define s_toupper(c)	(s_islower(c) ? ((c) + ('A'-'a')) : (c))
#define s_tolower(c)	(s_isupper(c) ? ((c) + ('a'-'A')) : (c))

#endif
