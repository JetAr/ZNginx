/*
** fdstdio.c
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

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <thread.h>
#include <string.h>
#include <syslog.h>
#ifdef __sgi
#include <bstring.h>
#endif

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#ifndef __osf__
#include <sys/filio.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/resource.h>

#include "phttpd.h"


int so_sndbuf = 0;
int so_rcvbuf = 0;
int select_timeout = 2*60;
int max_fds = 0;


static struct fd_buffer **buffer = NULL;

#define FD_INVALID(fd)	((fd) < 0 || (fd) >= max_fds)


/*
** Terminate (generate a core) with an error message
*/
static void fd_error(char *format, ...)
{
    va_list ap;


    va_start(ap, format);

    syslog(LOG_ERR, "fatal fdstdio error: %s", format);

    if (debug > 1)
    {
        fprintf(stderr, "*** FDStdio Error: ");
        vfprintf(stderr, format, ap);
        perror("");
        putc('\n', stderr);

        fflush(stderr);
    }

    va_end(ap);

    s_abort();
}



/*
** Initialize the FDSTDIO subsystem
*/
void fd_init(void)
{
    int i;
    struct rlimit rlb;


    if (getrlimit(RLIMIT_NOFILE, &rlb) < 0)
    {
        syslog(LOG_ERR, "getrlimit() failed: %m");

        if (debug > 1)
            perror("getrlimit()");

        rlb.rlim_max = 64;
    }

    rlb.rlim_cur = rlb.rlim_max;

    if (setrlimit(RLIMIT_NOFILE, &rlb) < 0)
    {
        syslog(LOG_ERR, "setrlimit() failed: %m");

        if (debug > 1)
            perror("setrlimit()");
    }

    max_fds = rlb.rlim_cur;

    buffer = s_malloc(sizeof(struct fd_buffer *) * max_fds);

    for (i = 0; i < max_fds; i++)
    {
        struct fd_buffer *bp;

        bp = buffer[i] = s_malloc(sizeof(struct fd_buffer));

        mutex_init(&bp->in.lock, USYNC_THREAD, NULL);
        mutex_init(&bp->out.lock, USYNC_THREAD, NULL);

        bp->in.buf = NULL;
        bp->in.size = 0;

        bp->out.buf = NULL;
        bp->out.size = 0;

        bp->in.state = FDS_FREE;
        bp->out.state = FDS_FREE;
    }
}



/*
** Associate an FDSTDIO buffer with a file descriptor
*/
void fd_reopen(int fd, int how, int type)
{
    int val;
    struct fd_buffer *bp = buffer[fd];


    if (FD_INVALID(fd))
        fd_error("fd_reopen(%d): Invalid file descriptor!\n", fd);


    if (bp->in.state != FDS_FREE || bp->out.state != FDS_FREE)
        fd_error("fd_reopen(%d): Already opened!\n", fd);

    bp->type = type;

    bp->in.start = 2;
    bp->in.end = 2;
    bp->in.save = 0;
    bp->in.count = 0;

    how &= 3;

    if (how == O_RDONLY || how == O_RDWR)
    {
        /* XXX - if O_RDONLY and FDT_FILE, use mmap :-) */

        bp->in.buf = s_malloc(BUFFER_SIZE+2);
        bp->in.size = BUFFER_SIZE+2;

        bp->in.state = FDS_OPEN;
    }
    else
    {
        bp->in.buf = NULL;
        bp->in.size = 0;
        bp->in.state = FDS_EOF;
    }

    bp->out.start = 0;
    bp->out.end = 0;
    bp->out.save = 0;
    bp->out.count = 0;

    if (how == O_WRONLY || how == O_RDWR)
    {
        bp->out.buf = s_malloc(BUFFER_SIZE);
        bp->out.size = BUFFER_SIZE;

        bp->out.state = FDS_OPEN;
    }
    else
    {
        bp->out.buf = NULL;
        bp->out.size = 0;
        bp->out.state = FDS_EOF;
    }

    if (type == FDT_SOCKET)
    {
        if (so_sndbuf > 0)
            setsockopt(fd, SOL_SOCKET, SO_SNDBUF,
                       (void *) &so_sndbuf, sizeof(so_sndbuf));

        if (so_rcvbuf > 0)
            setsockopt(fd, SOL_SOCKET, SO_RCVBUF,
                       (void *) &so_rcvbuf, sizeof(so_rcvbuf));

        val = 1;
        setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, (void *) &val, sizeof(val));

        val = 1;
        setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *) &val, sizeof(val));

        val = IPTOS_LOWDELAY;
        setsockopt(fd, IPPROTO_IP, IP_TOS, (void *) &val, sizeof(val));
    }
}



int fd_close(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int length;


    if (FD_INVALID(fd))
        fd_error("fd_close(%d): Invalid file descriptor!\n", fd);

    fd_flush(fd);

    mutex_lock(&bp->in.lock);
    bp->in.state = FDS_FREE;
    if (bp->in.buf)
    {
        s_free(bp->in.buf);
        bp->in.buf = NULL;
        bp->in.size = 0;
    }
    mutex_unlock(&bp->in.lock);

    mutex_lock(&bp->out.lock);
    bp->out.state = FDS_FREE;
    if (bp->out.buf)
    {
        s_free(bp->out.buf);
        bp->out.buf = NULL;
        bp->out.size = 0;
    }
    length = bp->out.count;
    mutex_unlock(&bp->out.lock);

    s_close(fd);

    return length;
}


int fd_shutdown(int fd, int how)
{
    struct fd_buffer *bp = buffer[fd];
    int status, dont_lock;


    if (FD_INVALID(fd))
        fd_error("fd_shutdown(%d): Invalid file descriptor!\n", fd);

    if (how < 0)
    {
        how = -how;
        dont_lock = 1;
    }
    else
        dont_lock = 0;

    if (how == 1 || how == 2)
    {
        if (!dont_lock)
        {
            fd_flush(fd);
            mutex_lock(&bp->out.lock);
        }

        bp->out.state = FDS_EOF;

        if (!dont_lock)
            mutex_unlock(&bp->out.lock);
    }

    if (how == 0 || how == 2)
    {
        if (!dont_lock)
            mutex_lock(&bp->in.lock);

        bp->in.state = FDS_EOF;

        if (!dont_lock)
            mutex_unlock(&bp->in.lock);
    }

    status = 0;
    if (bp->type != FDT_FILE)
        s_shutdown(fd, how);

    return status;
}


int fd_foreach(int (*fcnp)(int fd,
                           struct fd_buffer *bp,
                           void *misc),
               void *misc)
{
    int i, status;


    status = 0;

    for (i = 0; i < max_fds; i++)
    {
        if (mutex_trylock(&buffer[i]->in.lock))
        {
            /* fd buffer was busy, call fd_foreach, but with a
               NULL buffer pointer */
            if ((status = (*fcnp)(i, NULL, misc)) != 0)
                break;

            continue;
        }

        if (mutex_trylock(&buffer[i]->out.lock))
        {
            /* We got the input lock, but the output lock was busy.
               So we just call fd_foreach with a NULL buffer pointer */

            mutex_unlock(&buffer[i]->in.lock);

            if ((status = (*fcnp)(i, NULL, misc)) != 0)
                break;
            continue;
        }

        if (buffer[i]->in.state != FDS_FREE)
            if ((status = (*fcnp)(i, buffer[i], misc)) != 0)
            {
                mutex_unlock(&buffer[i]->out.lock);
                mutex_unlock(&buffer[i]->in.lock);
                break;
            }

        mutex_unlock(&buffer[i]->out.lock);
        mutex_unlock(&buffer[i]->in.lock);
    }

    return status;
}


int fd_open(const char *file, int how, ...)
{
    int fd, mode;
    va_list ap;



    va_start(ap, how);

    if (how & O_CREAT)
    {
        mode = va_arg(ap, int);
        fd = s_open(file, how, mode);
    }
    else
        fd = s_open(file, how);

    va_end(ap);

    if (fd < 0)
        return -1;

    fd_reopen(fd, how, FDT_FILE);

    return fd;
}



int fd_connect(struct sockaddr *sap, int len)
{
    int sock, status;


    if (debug > 6)
        fprintf(stderr, "fd_connect(): socket()\n");

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0)
        return -1;

    if (debug > 6)
        fprintf(stderr, "fd_connect(): s_connect()\n");

    status = s_connect(sock, sap, len);

    if (status < 0)
    {
        s_close(sock);
        return -1;
    }

    if (debug > 6)
        fprintf(stderr, "fd_connect(): fd_reopen()\n");

    fd_reopen(sock, 2, FDT_SOCKET);

    return sock;
}


int fd_mksockaddr_in(const char *host,
                     char *service,
                     struct sockaddr_in *sin)
{
    memset(sin, 0, sizeof(sin));

    sin->sin_family = AF_INET;
    if (s_isdigit(host[0]))
        sin->sin_addr.s_addr = inet_addr(host);
    else
    {
        struct hostent hp;
        char buf[2048];


        if (s_gethostbyname_r(host, &hp, buf, sizeof(buf), &h_errno) == NULL)
            return -1;

        memcpy(&sin->sin_addr, hp.h_addr_list[0], hp.h_length);
    }

    if (s_isdigit(service[0]))
        sin->sin_port = htons(atoi(service));
    else
    {
        struct servent sp;
        char buf[2048];


        if (getservbyname_r(service, "tcp", &sp, buf, sizeof(buf)) == NULL)
            return -1;

        sin->sin_port = sp.s_port;
    }

    return 0;

}

/*
** Initiate a socket TCP/IP connection to a foreign host/port
*/
int fd_sconnect(const char *host, char *service)
{
    struct sockaddr_in sin;


    if (debug > 5)
        fprintf(stderr, "fd_sconnect(): Calling fd_mksockaddr()\n");

    if (fd_mksockaddr_in(host, service, &sin) < 0)
        return -1;

    if (debug > 5)
        fprintf(stderr, "fd_sconnect(): Calling fd_connect()\n");

    return fd_connect((struct sockaddr *) &sin, sizeof(sin));
}


/* ********************************************************************** */

static int _fd_fionread(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int avail, status;


    if (bp->in.state == FDS_EOF)
        return -1;

    if (bp->type == FDT_SOCKET)
        return 65536;

    if (bp->type == FDT_FILE)
    {
        struct stat sb;
        int cpos;

        if (fstat(fd, &sb) < 0)
        {
            syslog(LOG_INFO,
                   "_fd_fionread(): fstat() failed: %m");

            bp->in.state = FDS_EOF;
            return -1;
        }

        cpos = lseek(fd, 0, SEEK_CUR);
        if (cpos < 0)
        {
            syslog(LOG_INFO, "_fd_fionread(): lseek() failed: %m");

            bp->in.state = FDS_EOF;
            return -1;
        }

        avail = sb.st_size - cpos;
    }
    else
    {
        avail = -2;

        status = s_ioctl(fd, FIONREAD, &avail);

        if (status < 0 || avail < 0)
        {
            syslog(LOG_INFO, "_fd_fionread(): ioctl(FIONREAD) failed: %m");

            bp->in.state = FDS_EOF;
            return status;
        }
    }

    return avail;
}


static int _fd_avail(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int avail, status;


    avail = bp->in.end - bp->in.start;

    status = _fd_fionread(fd);
    if (status > 0)
        avail += status;

    return avail;
}


int fd_avail(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int avail;


    if (FD_INVALID(fd))
        fd_error("fd_avail(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->in.lock);

    if (bp->in.state == FDS_FREE)
        fd_error("fd_avail(%d): Not opened\n", fd);

    avail = _fd_avail(fd);

    mutex_unlock(&bp->in.lock);

    return avail;
}


static int _fd_wait(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int status;
    fd_set fds;


    if (bp->in.state == FDS_EOF)
        return -1;

    if (bp->type == FDT_FILE)
        return 1;

    FD_ZERO(&fds);
    FD_SET(fd, &fds);

    if (select_timeout)
    {
        struct timeval tvb;


        tvb.tv_sec = select_timeout;
        tvb.tv_usec = 0;

        /*
        ** Why oh why can't all Unixes be like Linux and automatically
        ** update the Timeout structure if select() exits prematurely?
        */
        status = s_select(fd+1, &fds, NULL, NULL, &tvb);

    }
    else
        status = s_select(fd+1, &fds, NULL, NULL, NULL);

    if (status < 0)
        syslog(LOG_INFO, "_fd_wait(): select() failed: %m");

    if (status <= 0)
        bp->in.state = FDS_EOF;


    if (_fd_fionread(fd) <= 0)
    {
        if (debug > 10)
            fprintf(stderr,
                    "fdstdio.c: _fd_wait(%d): _fd_fionread() <= 0: EOF\n", fd);

        bp->in.state = FDS_EOF;
        status = -1;
    }

    return status;
}



int fd_wait(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int status, avail;


    if (FD_INVALID(fd))
        fd_error("fd_wait(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->in.lock);

    if (bp->in.state == FDS_FREE)
        fd_error("fd_avail(%d): Not opened\n", fd);

    avail = _fd_avail(fd);
    if (avail < 0)
        status = -1;
    else if (avail == 0)
        status = _fd_wait(fd);
    else
        status = 1;

    mutex_unlock(&bp->in.lock);

    return status;
}


#define FD_AVAIL(bp)   (bp->in.end - bp->in.start)


/*
** Returns the number of bytes available in the input buffer,
** or -1 when there is nothing in the buffer and EOF or an error
** has been seen.
*/
static int _fd_fill(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int empty, avail, status;


    if (bp->in.start == bp->in.end)
    {
        bp->in.start = 2;
        bp->in.end = 2;
    }

    /* Get the number of bytes available in the buffer */
    empty = bp->in.size - bp->in.end;
    if (empty == 0)
    {
        avail = FD_AVAIL(bp);
        if (debug > 9)
            fprintf(stderr, "_fd_fill(), returning %d\n", avail);

        return avail;
    }


    /* Get the number of bytes awaiting in the kernel */
    avail = _fd_fionread(fd);
    if (debug > 9)
        fprintf(stderr, "_fd_fill(): _fd_fionread(%d) = %d\n", fd, avail);

    if (avail < 0)
    {
        avail = FD_AVAIL(bp);

        if (avail == 0)
            return -1;
        else
            return avail;
    }

    if (avail > 0)
    {
        if (empty < avail)
            avail = empty;

        if (bp->in.state == FDS_EOF)
            status = -1;
        else
        {
            if (debug > 9)
                fprintf(stderr, "_fd_fill(): s_read(%d, .., %d)\n",
                        fd, avail);

            status = s_read(fd, bp->in.buf + bp->in.end, avail);
            if (status < 0)
                syslog((errno == EPIPE || errno == ECONNRESET) ?
                       LOG_INFO : LOG_ERR,
                       "_fd_fill(): s_read(%d, .., %d) failed: %m",
                       fd, avail);
        }

        if (status <= 0)
        {
            bp->in.state = FDS_EOF;

            avail = FD_AVAIL(bp);

            if (avail == 0)
                return -1;
            else
                return avail;
        }

        bp->in.end += status;
    }

    return FD_AVAIL(bp);
}


static int _fd_bfill(int fd)
{
    int avail;


    avail = _fd_fill(fd);
    if (avail == 0)
    {
        if (debug > 10)
            fprintf(stderr, "_fd_bfill(%d): before _fd_wait()\n", fd);

        if (_fd_wait(fd) < 0)
        {
            if (debug > 10)
                fprintf(stderr, "_fd_bfill(%d): _fd_wait() failed\n", fd);
            return -1;
        }

        avail = _fd_fill(fd);
    }

    return avail;
}


static int _fd_read(int fd, char *buf, int len)
{
    struct fd_buffer *bp = buffer[fd];
    int avail, got;


    got = 0;

    if (debug > 10)
        fprintf(stderr, "_fd_read(%d, ..., %d): Start\n",
                fd, len);

    avail = FD_AVAIL(bp);
    if (avail == 0)
    {
        if (bp->type != FDT_FILE)
            if (_fd_wait(fd) < 0)
            {
                if (debug > 10)
                    fprintf(stderr, "_fd_read(%d): _fd_wait(): failed\n", fd);
                return -1;
            }
    }
    else
    {
        if (avail > len)
            avail = len;

        memcpy(buf, bp->in.buf + bp->in.start, avail);
        buf += avail;
        got += avail;
        len -= avail;
        bp->in.start += avail;
    }

    while (len > 0 && (avail = _fd_fionread(fd)) > 0)
    {
        if (avail > len)
            avail = len;

        avail = s_read(fd, buf, avail);
        if (avail > 0)
        {
            got += avail;
            len -= avail;
            buf += avail;
        }
        else if (avail <= 0)
        {
            if (debug > 10)
                fprintf(stderr, "_fd_read(%d): _s_read() failed\n",
                        fd);

            bp->in.state = FDS_EOF;
            len = -1;
        }
    }

    return got;
}





int fd_read(int fd, char *buf, int len)
{
    struct fd_buffer *bp = buffer[fd];
    int avail;


    if (FD_INVALID(fd))
        fd_error("fd_read(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->in.lock);

    if (bp->in.state == FDS_FREE)
        fd_error("fd_read(%d): Not opened\n", fd);

    avail = _fd_read(fd, buf, len);

    mutex_unlock(&bp->in.lock);

    return avail;
}


static int _fd_rungetc(int c, int fd)
{
    struct fd_buffer *bp = buffer[fd];


    if (c == EOF)
        bp->in.save = FDIS_EOF;
    else if (bp->in.start > 0)
    {
        bp->in.buf[--bp->in.start] = (char) c;
        return c;
    }

    return EOF;
}


static int _fd_ungetc(int c, int fd)
{
    struct fd_buffer *bp = buffer[fd];


    if (c != '\n' || bp->type != FDT_SOCKET)
        return _fd_rungetc(c, fd);

    switch (bp->in.save)
    {
    case FDIS_START:
        return _fd_rungetc(c, fd);

    case FDIS_CR:
        return _fd_rungetc('\r', fd);

    case FDIS_LF:
        return _fd_rungetc('\n', fd);

    case FDIS_CRLF:
        (void) _fd_rungetc('\n', fd);
        return _fd_rungetc('\r', fd);

    case FDIS_LFCR:
        (void) _fd_rungetc('\r', fd);
        return _fd_rungetc('\n', fd);
    }

    return EOF;
}

int fd_ungetc(int c, int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int status;


    if (FD_INVALID(fd))
        fd_error("fd_ungetc(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->in.lock);
    if (bp->in.state == FDS_FREE)
        fd_error("fd_ungetc(%d): Not opened\n", fd);

    status = _fd_ungetc(c, fd);

    mutex_unlock(&bp->in.lock);

    return status;
}



static int _fd_rgetc(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int avail;


    avail = FD_AVAIL(bp);

    if (avail == 0)
        avail = _fd_bfill(fd);

    if (avail > 0)
        return bp->in.buf[bp->in.start++];;

    return EOF;
}


static int _fd_getc(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int c, c2;


    c = _fd_rgetc(fd);
    if (c == EOF)
        return c;

    if (bp->type == FDT_SOCKET)
    {
        if (bp->in.save == FDIS_START &&
                (c == '\r' || c == '\n'))
        {
            if (c == '\n')
                bp->in.save = FDIS_LF_START;
            else
                bp->in.save = FDIS_CR_START;

            return '\n';
        }

        if (bp->in.save == FDIS_LF_START)
        {
            if (c == '\r')
            {
                bp->in.save = FDIS_LFCR;
                return _fd_getc(fd);
            }
            else
                bp->in.save = FDIS_LF;

            return c;
        }

        if (bp->in.save == FDIS_CR_START)
        {
            if (c == '\n')
            {
                bp->in.save = FDIS_CRLF;
                return _fd_getc(fd);
            }
            else
            {
                bp->in.save = FDIS_CR;
                if (c == '\r')
                    return '\n';
            }

            return c;
        }

        if (bp->in.save == FDIS_CRLF && c == '\r')
        {
            c2 = _fd_getc(fd);

            if (c2 != '\n')
                _fd_ungetc(c2, fd);

            return '\n';
        }

        if (bp->in.save == FDIS_LFCR && c == '\n')
        {
            c2 = _fd_getc(fd);

            if (c2 != '\r')
                _fd_ungetc(c2, fd);

            return '\n';
        }

        if (bp->in.save == FDIS_CR && c == '\r')
            return '\n';

        if (bp->in.save == FDIS_LF && c == '\n')
            return '\n';
    }

    return c;
}


int fd_getc(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int c;


    if (FD_INVALID(fd))
        fd_error("fd_getc(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->in.lock);

    if (bp->in.state == FDS_FREE)
        fd_error("fd_getc(%d): Not opened\n", fd);

    c = _fd_getc(fd);

    mutex_unlock(&bp->in.lock);

    return c;
}


/* ********************************************************************** */

static int _fd_flush(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int avail, status;


    if (bp->out.state == FDS_EOF)
        return -1;

    while ((avail = bp->out.end - bp->out.start) > 0)
    {
        status = s_write(fd, bp->out.buf - bp->out.start, avail);
        if (status < 0)
        {
            syslog((errno == EPIPE || errno == ECONNRESET || errno == ENXIO) ?
                   LOG_INFO : LOG_ERR,
                   "_fd_flush(): s_write(%d, ..., %d) failed: %m",
                   fd, avail);

            bp->out.state = FDS_EOF;
            return -1;
        }

        if (status == 0)
            syslog(LOG_ERR, "_fd_flush(): write(%d, ..., %d): Wrote zero",
                   fd, avail);

        bp->out.count += status;
        bp->out.start += status;
    }

    bp->out.start = 0;
    bp->out.end = 0;

    return 0;
}



void *flush_thread(void *data)
{
    int fd = (int) data;
    struct fd_buffer *bp = buffer[fd];

    _fd_flush(fd);

    mutex_unlock(&bp->out.lock);
    return NULL;
}


int fd_flush(int fd)
{
    struct fd_buffer *bp = buffer[fd];


    if (FD_INVALID(fd))
        fd_error("fd_flush(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->out.lock);

    if (bp->in.state == FDS_FREE)
        fd_error("fd_flush(%d): Not opened\n", fd);

    if (thr_create(NULL,
                   0,
                   (void *(*)(void *)) flush_thread,
                   (void *) fd,
                   THR_DETACHED+THR_DAEMON,
                   NULL))
    {
        syslog(LOG_ERR, "thr_create(flush_thread) failed: %m");
        exit(1);
    }

    return 0;
}


static int _fd_write(int fd, const char *buf, int len)
{
    struct fd_buffer *bp = buffer[fd];
    int avail, status;


    avail = bp->out.size - bp->out.end;
    if (avail < len)
    {
        if (_fd_flush(fd) < 0)
            return -1;

        avail = bp->out.size;
    }

    if (len < avail)
    {
        memcpy(bp->out.buf + bp->out.end, buf, len);
        bp->out.end += len;
    }
    else
    {
        while (len > 0)
        {
            status = s_write(fd, buf, len);
            if (status < 0)
            {
                syslog((errno == EPIPE ||
                        errno == ECONNRESET ||
                        errno == ENXIO) ? LOG_INFO : LOG_ERR,
                       "_fd_write(): write(%d, ..., %d) failed: %m",
                       fd, avail);

                bp->out.state = FDS_EOF;
                return -1;
            }

            if (status == 0)
                syslog(LOG_ERR, "_fd_write(): write(%d, ..., %d): Wrote zero",
                       fd, avail);

            bp->out.count += status;
            buf += status;
            len -= status;
        }
    }

    return 0;
}




int fd_write(int fd, const char *buf, int len)
{
    struct fd_buffer *bp = buffer[fd];
    int status;


    if (FD_INVALID(fd))
        fd_error("fd_write(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->out.lock);

    if (bp->in.state == FDS_FREE)
        fd_error("fd_write(%d): Not opened\n", fd);

    status = _fd_write(fd, buf, len);

    mutex_unlock(&bp->out.lock);

    return status;
}


static int _fd_rputc(int c, int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int avail;


    avail = bp->out.size - bp->out.end;
    if (avail == 0)
    {
        if (_fd_flush(fd) < 0)
            return -1;
    }

    bp->out.buf[bp->out.end++] = (char) c;

    return c;
}


static int _fd_putc(int c, int fd)
{
    struct fd_buffer *bp = buffer[fd];


    if (bp->type == FDT_SOCKET)
    {
        if (c == '\n')
            if (bp->out.save != '\r')
                if (_fd_rputc('\r', fd) == EOF)
                    return EOF;
    }

    bp->out.save = c;

    return _fd_rputc(c, fd);
}


int fd_putc(int c, int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int status;


    if (FD_INVALID(fd))
        fd_error("fd_putc(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->out.lock);

    if (bp->in.state == FDS_FREE)
        fd_error("fd_putc(%d): Not opened\n", fd);

    status = _fd_putc(c, fd);

    mutex_unlock(&bp->out.lock);

    return status;
}


/* ********************************************************************** */


static char *fd_mgets(int maxsize, int fd)
{
    int c, i;
    char *buf;
    int csize = 0;


    buf = s_malloc(csize = 1024);

    for (i = 0; (maxsize ? (i < maxsize-1) : 1) &&
            (c = fd_getc(fd)) != EOF; i++)
    {
        if (i >= csize-1)
            buf = s_realloc(buf, csize+=1024);

        buf[i] = c;
        if (c == '\n' || c == '\r')
        {
            i++;
            break;
        }
    }

    if (i == 0)
        return NULL;

    buf[i] = '\0';

    return buf;
}


/* Get a line of text from the file
**
** If buf == NULL, allocate it dynamically
** If size != 0, use a maximum size limit of buffer
*/
char *fd_gets(char *buf, int size, int fd)
{
    int c, i;


    if (buf == NULL)
        return fd_mgets(size, fd);

    for (i = 0; i < size-1 && (c = fd_getc(fd)) != EOF; i++)
    {
        buf[i] = c;
        if (c == '\n' || c == '\r')
        {
            i++;
            break;
        }
    }

    if (i == 0)
        return NULL;

    buf[i] = '\0';

    return buf;
}




int fd_puts(const char *str, int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int len = strlen(str);


    if (FD_INVALID(fd))
        fd_error("fd_puts(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->out.lock);

    if (bp->out.state == FDS_FREE)
        fd_error("fd_puts(%d): Not opened\n", fd);

    while (*str)
        if (_fd_putc(*str++, fd) == EOF)
        {
            mutex_unlock(&bp->out.lock);
            return -1;
        }

    mutex_unlock(&bp->out.lock);

    return len;
}




int fd_puti(int val, int fd)
{
    char buf[256];


    s_sprintf(buf, sizeof(buf), "%d", val);
    return fd_puts(buf, fd);
}


int fd_puts2nl(const char *s1, const char *s2, int fd)
{
    fd_puts(s1, fd);
    fd_puts(s2, fd);
    return fd_putc('\n', fd);
}

int fd_putsinl(const char *s1, int val, int fd)
{
    fd_puts(s1, fd);
    fd_puti(val, fd);
    return fd_putc('\n', fd);
}

/*
** XXX: Fixme: I _hate_ statically allocated buffers...
*/
int fd_vprintf(int fd,
               const char *format,
               va_list ap)
{
    int len, ret;
    char *buf;


    buf = s_malloc(20480);
    len = s_vsprintf(buf, 20480, format, ap);
    if (len < 0)
    {
        s_free(buf);
        return -1;
    }

    ret = fd_puts(buf, fd);
    s_free(buf);

    return ret;
}


int fd_printf(int fd,
              const char *format,
              ...)
{
    va_list ap;
    int len;


    va_start(ap, format);

    len = fd_vprintf(fd, format, ap);

    va_end(ap);

    return len;
}




int fd_written(int fd)
{
    struct fd_buffer *bp = buffer[fd];
    int length;


    if (FD_INVALID(fd))
        fd_error("fd_written(%d): Invalid file descriptor!\n", fd);

    mutex_lock(&bp->out.lock);
    length = bp->out.count + (bp->out.end - bp->out.start);
    mutex_unlock(&bp->out.lock);

    return length;
}


struct relayinfo
{
    int read_fd;
    int write_fd;
    int bytes;
};



/* Relay a stream from one fd to another */
static void *relay_data(struct relayinfo *rip)
{
    char buf[8192];
    int len, status;
    int to_read;


    if (debug > 6)
        fprintf(stderr, "fdstdio: relay_data(%d -> %d): Start\n",
                rip->read_fd, rip->write_fd);

    len = 0;
    to_read = rip->bytes;

    while (to_read != 0 &&
            (len = fd_read(rip->read_fd,
                           buf,
                           (to_read > 0 && to_read < sizeof(buf))
                           ? to_read : sizeof(buf))) > 0)
    {
        if (debug > 7)
            fprintf(stderr, "fd_relay(): fd_read(%d), len=%d\n",
                    rip->read_fd, len);

        if (to_read > 0)
            to_read -= len;

        if (debug > 8)
            fprintf(stderr, "fd_relay(): before fd_write(%d, ..., %d)\n",
                    rip->write_fd, len);

        status = fd_write(rip->write_fd, buf, len);
        if (status < 0)
            break;

        if (debug > 8)
            fprintf(stderr, "fd_relay(): before fd_flush(%d)\n",
                    rip->write_fd);

        fd_flush(rip->write_fd);
    }

    if (debug > 7)
        fprintf(stderr, "fdstdio: relay_data(): shutting down fd %d\n",
                rip->write_fd);

    fd_shutdown(rip->write_fd, 1);

    if (debug > 7)
        fprintf(stderr, "fdstdio: relay_data(): Stop\n");

    return NULL;
}


int fd_relay(int fd1, int fd2, int bidir)
{
    thread_t tid1, tid2;
    struct relayinfo rb1, rb2;


    if (debug > 6)
        fprintf(stderr, "fd_relay(%d,%d,%d): Start\n",
                fd1, fd2, bidir);

    if (bidir)
    {
        rb1.read_fd = fd2;
        rb1.write_fd = fd1;
        rb1.bytes = -1;

        if (thr_create(NULL, 0,
                       (void *(*)(void *)) relay_data, (void *) &rb1,
                       0,
                       &tid1))
        {
            syslog(LOG_ERR, "fd_relay(): thr_create() failed");
            return -1;
        }
    }

    rb2.read_fd = fd1;
    rb2.write_fd = fd2;
    rb2.bytes = -1;

    if (thr_create(NULL, 0,
                   (void *(*)(void *)) relay_data, (void *) &rb2,
                   0,
                   &tid2))
    {
        syslog(LOG_ERR, "fd_relay(): thr_create() failed");
        return -1;
    }

    if (bidir)
    {
        if (debug > 6)
            fprintf(stderr, "fd_relay(), before thr_join(tid1)\n");

        thr_join(tid1, NULL, NULL);
    }

    if (debug > 6)
        fprintf(stderr, "fd_relay(), before thr_join(tid2)\n");

    thr_join(tid2, NULL, NULL);

    return 0;
}


int fd_nrelay(int fd1, int fd2, int bidir, int bytes)
{
    thread_t tid1, tid2;
    struct relayinfo rb1, rb2;


    if (debug > 6)
        fprintf(stderr, "fd_nrelay(%d,%d,%d,%d): Start\n",
                fd1, fd2, bidir, bytes);

    if (bidir)
    {
        rb1.read_fd = fd2;
        rb1.write_fd = fd1;
        rb1.bytes = bytes;

        if (thr_create(NULL, 0,
                       (void *(*)(void *)) relay_data, (void *) &rb1,
                       0,
                       &tid1))
        {
            syslog(LOG_ERR, "fd_nrelay(): thr_create() failed");
            return -1;
        }
    }

    rb2.read_fd = fd1;
    rb2.write_fd = fd2;
    rb2.bytes = bytes;

    if (thr_create(NULL, 0,
                   (void *(*)(void *)) relay_data, (void *) &rb2,
                   0,
                   &tid2))
    {
        syslog(LOG_ERR, "fd_relay(): thr_create() failed");
        return -1;
    }

    if (bidir)
    {
        if (debug > 6)
            fprintf(stderr, "fd_relay(), before thr_join(tid1)\n");

        thr_join(tid1, NULL, NULL);
    }

    if (debug > 6)
        fprintf(stderr, "fd_relay(), before thr_join(tid2)\n");

    thr_join(tid2, NULL, NULL);

    return 0;
}
