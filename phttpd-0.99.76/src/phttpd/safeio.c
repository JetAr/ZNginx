/*
** safeio.c
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

#include <stdio.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <synch.h>
#include <sys/mman.h>
#include <netdb.h>

#include "phttpd.h"


/*
** Almost safe string printf
*/
int s_sprintf(char *buf, int size, const char *format, ...)
{
    va_list ap;
    int len;


    va_start(ap, format);
    len = s_vsprintf(buf, size, format, ap);
    va_end(ap);

    return len;
}


#undef vsprintf
int s_vsprintf(char *buf, int size, const char *format, va_list ap)
{
    int len;

#ifdef HAVE_VSNPRINTF
    len = __vsnprintf(buf, size, format, ap);
#else
    len = vsprintf(buf, format, ap);
    if (len > size)
        s_abort();
#endif

    return len;
}


/*
** Safe string copy.
**
** If the source string is longer than will fit in the output buffer,
** then only copy as many as will fit, and then NUL terminate the destination
** string.
**
** Returns the number of characters in the output buffer (not including
** the terminating NUL), or -1 if the buffer is of zero size.
*/
int s_strcpy(char *dst, int dstsize, const char *src)
{
    int srclen;


    if (dst == NULL)
        s_abort();

    if (dstsize-- < 1)
    {
        s_abort(); /* Temp, just to find all bad uses of it */
        return -1;
    }

    if (src == NULL)
    {
        *dst = '\0';
        return 0;
    }

    srclen = strlen(src);

    if (srclen > dstsize)
        srclen = dstsize;

    if (srclen > 0)
        memcpy(dst, src, srclen);
    dst[srclen] = '\0';

    return srclen;
}


/*
** Safe string limited copy.
**
** If the source string is longer than will fit in the output buffer,
** then only copy as many as will fit, and then NUL terminate the destination
** string.
**
** Returns the number of characters in the output buffer (not including
** the terminating NUL), or -1 if the buffer is of zero size.
*/
int s_strncpy(char *dst, int dstsize, const char *src, int maxcopy)
{
    int srclen;


    if (dst == NULL)
        s_abort();

    if (dstsize-- < 1 || maxcopy < 0)
    {
        s_abort(); /* Temp, just to find all bad uses of it */
        return -1;
    }

    if (src == NULL)
    {
        *dst = '\0';
        return 0;
    }

    srclen = strlen(src);

    if (srclen > maxcopy)
        srclen = maxcopy;

    if (srclen > dstsize)
        srclen = dstsize;

    if (srclen > 0)
        memcpy(dst, src, srclen);

    dst[srclen] = '\0';

    return srclen;
}


/*
** Safe string concatenation.
**
** If the source string is longer than will fit in the output buffer,
** then only copy as many as will fit, and then NUL terminate the destination
** string.
**
** Returns the number of characters in the output buffer (not including
** the terminating NUL), or -1 if the buffer is of zero size.
*/
int s_strcat(char *dst, int dstsize, const char *src)
{
    int srclen;
    int dstlen;


    if (dst == NULL)
        s_abort();

    if (dstsize-- < 1)
    {
        s_abort(); /* Temp, just to find all bad uses of it */
        return -1;
    }

    dstlen = strlen(dst);

    if (src == NULL)
        return dstlen;

    srclen = strlen(src);

    if (srclen+dstlen > dstsize)
        srclen = dstsize-dstlen;

    if (srclen < 0)
    {
        syslog(LOG_ERR, "s_strcat(..., %d, ...): buffer overflow", dstsize);
        return dstlen;
    }

    if (srclen > 0)
        memcpy(dst+dstlen, src, srclen);
    dst[dstlen+srclen] = '\0';

    return srclen+dstlen;
}



/*
** Safe string limited concatenation.
**
** If the source string is longer than will fit in the output buffer,
** then only copy as many as will fit, and then NUL terminate the destination
** string.
**
** Returns the number of characters in the output buffer (not including
** the terminating NUL), or -1 if the buffer is of zero size.
*/
int s_strncat(char *dst, int dstsize, const char *src, int maxcopy)
{
    int srclen;
    int dstlen;


    if (dst == NULL)
        s_abort();

    if (dstsize-- < 1)
    {
        s_abort(); /* Temp, just to find all bad uses of it */
        return -1;
    }

    dstlen = strlen(dst);

    if (src == NULL)
        return dstlen;

    srclen = strlen(src);

    if (srclen > maxcopy)
        srclen = maxcopy;

    if (srclen+dstlen > dstsize)
        srclen = dstsize-dstlen;

    if (srclen < 0)
    {
        syslog(LOG_ERR, "s_strncat(..., %d, ..., %d): buffer overflow",
               dstsize, maxcopy);

        return dstlen;
    }

    if (srclen > 0)
        memcpy(dst+dstlen, src, srclen);
    dst[dstlen+srclen] = '\0';

    return srclen+dstlen;
}


/*
** Safe file descriptor open. Handles EINTR.
** Returns -1, or the opened file descriptor.
*/
#undef open
int s_open(path, oflag, mode)
const char *path;
int oflag;
mode_t mode;
{
    int s;


    while ((s = open(path, oflag, mode)) < 0 && errno == EINTR)
        ;

    if (s < 0 && (errno == EMFILE ||
                  errno == ENFILE ||
                  errno == ENOMEM ||
                  errno == ENOSR))
    {
        /* Too many open files */

        syslog(LOG_WARNING, "s_open(\"%s\", 0%o): %m", path, oflag);

        /* Should perhaps, sleep and retry - and if it fails
           again restart the server? */
    }

    return s;
}

#undef close
int s_close(int fd)
{
    int s;


    while ((s = close(fd)) < 0 && errno == EINTR)
        ;

    return s;
}


#undef shutdown
int s_shutdown(int fd, int how)
{
    int s;


    while ((s = shutdown(fd, how)) < 0 && errno == EINTR)
        ;

    return s;
}


#undef connect
int s_connect(int fd, struct sockaddr *sin, int len)
{
    int s;


    if (debug > 7)
        fprintf(stderr, "s_connect(): Start\n");

    while ((s = connect(fd, sin, len)) < 0 && errno == EINTR)
        if (debug > 7)
            perror("connect()");

    if (s < 0 && (errno == EMFILE ||
                  errno == ENFILE ||
                  errno == ENOMEM ||
                  errno == ENOSR))
    {
        /* Too many open files */

        syslog(LOG_WARNING, "s_connect(%d, ...): %m", fd);

        /* Should perhaps, sleep and retry - and if it fails
           again restart the server? */
    }

    if (debug > 7)
        fprintf(stderr, "s_connect(): Exiting\n");

    return s;
}

#undef accept
int s_accept(int fd, struct sockaddr *sin, int *lenp)
{
    int s;


    while ((s = accept(fd, sin, lenp)) < 0 &&
            (errno == EINTR || errno == EPROTO))
        ;

    if (s < 0 && (errno == EMFILE ||
                  errno == ENFILE ||
                  errno == ENOMEM ||
                  errno == ENOSR))
    {
        /* Too many open files */

        syslog(LOG_WARNING, "s_accept(%d, ...): %m", fd);

        /* Should perhaps, sleep and retry - and if it fails
           again restart the server? */
    }

    return s;
}


#undef ioctl
int s_ioctl(int fd, int what, void *ptr)
{
    int s;


    while ((s = ioctl(fd, what, ptr)) < 0 && errno == EINTR)
        ;

    return s;

}



#undef select
int s_select(int nfd, fd_set *rs, fd_set *ws, fd_set *es, struct timeval *tv)
{
    int s;


    while (( s = select(nfd, rs, ws, es, tv)) < 0 && errno == EINTR)
        ;

    if (s < 0 && debug)
        perror("select");

    return s;
}

#undef read
int s_read(int fd, char *buf, int len)
{
    int s;


    while ((s = read(fd, buf, len)) < 0 && errno == EINTR)
        ;

    if (s < 0 && debug)
        perror("read");

    if (debug > 5)
        fprintf(stderr, "read(%d), len=%d\n", fd, s);

    return s;
}


#undef write
int s_write(int fd, const char *buf, int len)
{
    int s;


    while ((s = write(fd, buf, len)) < 0 && errno == EINTR)
        ;

    return s;
}

mutex_t *s_mutex_create(int type)
{
    mutex_t *mp;


    if (type == USYNC_PROCESS)
    {
        int fd;


        fd = s_open("/dev/zero", 2);
        if (fd < 0)
            return NULL;

        mp = (mutex_t *) mmap((caddr_t) NULL,
                              sizeof(mutex_t),
                              PROT_READ+PROT_WRITE,
                              MAP_SHARED, fd, 0);

        s_close(fd);
    }
    else
        mp = (mutex_t *) s_malloc(sizeof(mutex_t));

    if (mp)
        mutex_init(mp, type, NULL);

    return mp;
}


void s_mutex_destroy(mutex_t *mp, int type)
{
    mutex_destroy(mp);

    if (type == USYNC_PROCESS)
        munmap((caddr_t) mp, sizeof(mutex_t));
    else
        s_free((void *)mp);
}


sema_t *s_sema_create(int count, int type)
{
    sema_t *sp;


    if (type == USYNC_PROCESS)
    {
        int fd;


        fd = s_open("/dev/zero", 2);
        if (fd < 0)
            return NULL;

        sp = (sema_t *) mmap((caddr_t) NULL,
                             sizeof(sema_t),
                             PROT_READ+PROT_WRITE,
                             MAP_SHARED, fd, 0);

        s_close(fd);
    }
    else
        sp = (sema_t *) s_malloc(sizeof(sema_t));

    if (sp)
        sema_init(sp, count, type, NULL);

    return sp;
}


void s_sema_destroy(sema_t *sp, int type)
{
    sema_destroy(sp);

    if (type == USYNC_PROCESS)
        munmap((caddr_t) sp, sizeof(sema_t));
    else
        s_free(sp);
}



pid_t s_sema_wait(sema_t *sp)
{
    pid_t s;

    while ((s = sema_wait(sp)) && errno == EINTR)
        ;

    return s;
}


#undef chdir
int s_chdir(const char *path)
{
    int s;


    while ((s = chdir(path)) < 0 && errno == EINTR)
        ;

    return s;
}

#undef chroot
int s_chroot(const char *path)
{
    int s;


    while ((s = chroot(path)) < 0 && errno == EINTR)
        ;

    return s;
}


#undef dup2
/* dup2 is not MT-Safe according to the SunOS 5 man page */
int s_dup2(int fd1, int fd2)
{
    int s;
    static mutex_t dup2_lock;


    mutex_lock(&dup2_lock);

    while ((s = dup2(fd1, fd2)) < 0 && errno == EINTR)
        ;

    mutex_unlock(&dup2_lock);

    if (s < 0 && (errno == EMFILE))
    {
        /* Too many open files */

        syslog(LOG_WARNING, "s_dup2(%d, &d): %m", fd1, fd2);

        /* Should perhaps, sleep and retry - and if it fails
           again restart the server? */
    }

    return s;
}

#undef waitpid
pid_t s_waitpid(pid_t pid, int *status, int options)
{
    int s;


    while ((s = waitpid(pid, status, options)) < 0 && errno == EINTR)
        ;

    return s;
}


#undef execve
int s_execve(const char *path, char *const argv[], char *const envp[])
{
    int s;


    while ((s = execve(path, argv, envp)) > 0 && errno == EINTR)
        ;

    return s;
}

#if 0
DIR *s_opendir(const char *path)
{
    DIR *dp;


    while ((dp = opendir(path)) == NULL && errno == EINTR)
        ;

    return dp;
}


int s_closedir(DIR *dirp)
{
    int s;


    while ((s = closedir(dirp)) < 0 && errno == EINTR)
        ;

    return s;
}


struct dirent *s_readdir_r(DIR *dirp, struct dirent *res)
{
    struct dirent *s;


    while ((s = readdir_r(dirp, res)) == NULL &&
            (errno == EINTR || errno == EAGAIN))
        ;

    return s;
}
#endif




#define SA_MAGIC 356834534
#define SA_FREE  562368734

struct allocinfo
{
    int magic;
    int size;
};

#ifdef INCLUDE_ALLOC_STATS
struct allocstats alloc_stats;
#endif

#undef malloc
void *s_malloc(size_t size)
{
    struct allocinfo *p;
    int i, as;


    i = 0;
    as = size + sizeof(struct allocinfo);

Loop:
    p = (struct allocinfo *) malloc(as);
    if (p == NULL)
    {
        if (i++ < 10)
        {
            syslog(LOG_WARNING,
                   "s_malloc(%d): %m - retrying in %d seconds",
                   size, i);
            s_sleep(i);
            goto Loop;
        }
        else
        {
            syslog(LOG_ERR, "s_malloc(%d): %m - aborting", size);
            s_abort();
        }
    }

    p->magic = SA_MAGIC;
    p->size = size;

    ++p;

    memset((void *) p, 0, size);

#ifdef INCLUDE_ALLOC_STATS
    mutex_lock(&alloc_stats.lock);
    alloc_stats.malloc_calls++;
    alloc_stats.bytes += size;
    alloc_stats.blocks++;
    mutex_unlock(&alloc_stats.lock);
#endif

    return (void *) p;
}


#undef free
void s_free(void *ptr)
{
    struct allocinfo *p;


    if (ptr == NULL)
        return;

    p = (struct allocinfo *) ptr;
    --p;

    if (p->magic != SA_MAGIC)
    {
        if (p->magic == SA_FREE)
            syslog(LOG_ERR, "s_free(): freeing freed block of size %d",
                   p->size);
        else
            syslog(LOG_ERR, "s_free(): bad magic number");
        s_abort();
    }

#ifdef INCLUDE_ALLOC_STATS
    mutex_lock(&alloc_stats.lock);
    alloc_stats.free_calls++;
    alloc_stats.bytes -= p->size;
    alloc_stats.blocks--;
    mutex_unlock(&alloc_stats.lock);
#endif

    p->magic = SA_FREE;
    free(p);
}


#undef realloc
void *s_realloc(void *ptr, size_t size)
{
    struct allocinfo *p, *op;
    int os, sa, i;


    if (ptr == NULL)
        return s_malloc(size);

    sa = size + sizeof(struct allocinfo);
    op = (struct allocinfo *) ptr;
    --op;

    os = op->size;

    if (op->magic != SA_MAGIC)
    {
        if (op->magic == SA_FREE)
            syslog(LOG_ERR, "s_realloc(): reallocating freed block of size %d",
                   op->size);
        else
            syslog(LOG_ERR, "s_realloc(): bad magic number");
        s_abort();
    }

    i = 0;
Loop:
    p = realloc(op, sa);
    if (p == NULL)
    {
        if (i++ < 10)
        {
            syslog(LOG_WARNING,
                   "s_realloc(..., %d): %m - retrying in %d seconds",
                   size, i);
            s_sleep(i);
            goto Loop;
        }
        else
        {
            syslog(LOG_ERR, "s_realloc(..., %d): %m - aborting", size);
            s_abort();
        }
    }

    p->size = size;
    ++p;

    if (size > os)
    {
        char *cp = (char *) p;

        memset(cp + os, 0, size - os);
    }

#ifdef INCLUDE_ALLOC_STATS
    mutex_lock(&alloc_stats.lock);
    alloc_stats.realloc_calls++;
    if (size > os)
        alloc_stats.bytes += (size - os);
    else
        alloc_stats.bytes -= (os - size);
    mutex_unlock(&alloc_stats.lock);
#endif

    return (void *) p;
}


char *s_strdup(const char *str)
{
    char *ptr;
    int len;


    if (str == NULL)
        return NULL;

    len = strlen(str);
    ptr = s_malloc(len+1);
    memcpy(ptr, str, len+1);

    return ptr;
}

char *s_strxdup(const char *str, int extra, int *size)
{
    char *ptr;
    int len;


    if (str == NULL)
        return NULL;

    len = strlen(str);
    if (size)
        *size = len+1+extra;
    ptr = s_malloc(len+1+extra);
    memcpy(ptr, str, len+1);

    return ptr;
}

#undef readlink
int s_readlink(const char *path, char *buf, int buflen)
{
    int s;


    while ((s = readlink(path, buf, buflen)) < 0 && errno == EINTR)
        ;

    return s;
}

#undef unlink
int s_unlink(const char *path)
{
    int s;


    while ((s = unlink(path)) < 0 && errno == EINTR)
        ;

    return s;
}



/*
** The normal abort() uses a signal that forces threads to to switch
** to another thread, before turning over the control to the debugger.
**
** That is not so optimal when using a non-threads-aware debugger like Gdb.
*/
void s_abort(void)
{
    * (int *) 0 = 4711;
}


#undef stat
int s_stat(const char *path, struct stat *buf)
{
    int status;


    while ((status = stat(path, buf)) < 0 && errno == EINTR)
        ;

    return status;
}

#undef lstat
int s_lstat(const char *path, struct stat *buf)
{
    int status;


    while ((status = lstat(path, buf)) < 0 && errno == EINTR)
        ;

    return status;
}


/* Generate a unique filename */
int s_tmpname(char *buf, int bufsize)
{
    static mutex_t tmplock;
    static int count;
    int i;

    mutex_lock(&tmplock);
    i = count++;
    mutex_unlock(&tmplock);

    return s_sprintf(buf, bufsize, "/tmp/.tmp.%d.%d", getpid(), i);
}


/* Create and open a unique temporary file */
int s_tmpfile(char *buf, int bufsize)
{
    char buf2[1024];
    int fd;


    if (buf == NULL)
    {
        buf = buf2;
        bufsize = sizeof(buf2);
    }

    while (s_tmpname(buf, bufsize) >= 0)
    {
        fd = s_open(buf, O_RDWR+O_CREAT+O_TRUNC, 0600);
        if (fd >= 0)
        {
            if (s_unlink(buf) < 0)
            {
                s_close(fd);
                return -1;
            }

            return fd;
        }
    }

    return -1;
}
