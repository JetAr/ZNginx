/*
** fdstdio.c
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

#ifndef PHTTPD_FDSTDIO_H
#define PHTTPD_FDSTDIO_H

#include <synch.h>
#include <stdarg.h>

extern int so_sndbuf;
extern int so_rcvbuf;
extern int select_timeout;
extern int max_fds;

#define BUFFER_SIZE 8192

#define FDIS_EOF     -1  /* Connection closed */
#define FDIS_START    0  /* Type of connection not established */
#define FDIS_CR_START 1  /* Got a CR, but type is still unknown */
#define FDIS_LF_START 2  /* Got a LF, but type is still unknown */
#define FDIS_CR       3  /* Single CR end-of-line (Macintosh) */
#define FDIS_LF       4  /* Single LF end-of-line (Unix) */
#define FDIS_CRLF     5  /* CR+LF end-of-line (HTTP Protocol) */
#define FDIS_LFCR     6  /* LF+CR end-of-line (Stupid) */

struct fd_stream
{
    int (*read)(struct fd_stream *fs, int fd, char *buf, int len);
    int (*write)(struct fd_stream *fs, int fd, const char *buf, int len);
    int (*close)(struct fd_stream *fs, int fd);
    int (*shutdown)(struct fd_stream *fs, int fd, int how);
    int (*avail)(struct fd_stream *fs, int fd);
    int (*wait)(struct fd_stream *fs, int fd, int timeout);

    struct fd_stream *up;
};


struct fd_buffer
{
    int type;

    struct fd_stream *fs;

    struct
    {
        int start;
        int end;

        int save;
        int count;

        mutex_t lock;
        int state;

        int size;
        char *buf;
    } in, out;
};

#define FDS_EOF         -1
#define FDS_FREE        0
#define FDS_OPEN        1


extern int fd_foreach(int (*fcnp)(int fd,
                                  struct fd_buffer *bp,
                                  void *misc),
                      void *misc);


#define FDT_SOCKET      0
#define FDT_FILE        1
#define FDT_TTY         2
#define FDT_PIPE        3
#define FDT_STRING      4


extern void fd_init(void);
extern void fd_reopen(int fd, int how, int type);
extern int  fd_close(int fd);
extern int  fd_shutdown(int fd, int how);

extern int fd_open(const char *file, int how, ...);
extern int fd_connect(struct sockaddr *sap, int len);
extern int fd_sconnect(const char *host, char *service);

struct sockaddr_in;
extern int fd_mksockaddr_in(const char *host,
                            char *service,
                            struct sockaddr_in *sin);

extern int fd_flush(int fd);
extern int fd_putc(int c, int fd);
extern int fd_puts(const char *str, int fd);
extern int fd_puti(int val, int fd);
extern int fd_puts2nl(const char *s1, const char *s2, int fd);
extern int fd_putsinl(const char *s1, int val, int fd);

extern int fd_vprintf(int fd, const char *format, va_list ap);
extern int fd_printf(int fd, const char *format, ...);

extern int fd_getc(int fd);
extern int fd_ungetc(int c, int fd);
extern char *fd_gets(char *buf, int size, int fd);

extern int fd_write(int fd, const char *buf, int len);
extern int fd_read(int fd, char *buf, int len);

extern int fd_relay(int fd1, int fd2, int bidir);
extern int fd_nrelay(int fd1, int fd2, int bidir, int bytes);

extern int fd_written(int fd);

extern int fd_avail(int fd);
extern int fd_wait(int fd);

#endif
