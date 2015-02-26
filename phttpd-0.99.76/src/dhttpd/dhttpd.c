/*
** dhttpd.c		A dummy HTTP server for benchmarking purposes
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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <thread.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/resource.h>

char *mbuf;
int mbufsize = 4096;
int concurrency = 32;
int port = 80;
int bound = 0;
int stack_size = 0;
int use_exit = 0;
int n_processes = 0;


static void *request_thread(void *misc)
{
    int fd = * (int *) misc;
    char buf[4096], *cp;
    int len, val, tosend;


    val = 1;
    setsockopt(fd, 6, TCP_NODELAY, (void *) &val, sizeof(val));

    while ((len = read(fd, buf, sizeof(buf))) < 0 && errno == EINTR)
        ;

    if (len < 0)
        perror("read");

    tosend = mbufsize;
    cp = mbuf;
    while (len > 0 && tosend > 0)
    {
        while ((len = write(fd, cp, tosend)) < 0 && errno == EINTR)
            ;

        if (len < 0)
            perror("dhttpd: write");

        if (len > 0)
        {
            tosend -= len;
            cp += len;
        }
    }

    while ((len = close(fd)) < 0 && errno == EINTR)
        ;

    if (len < 0)
        perror("close");

    free(misc);

    if (use_exit)
        thr_exit(NULL);

    return NULL;
}


int help(char *argv0)
{
    printf("Usage: %s [<options>]*\n\n", argv0);

    puts("Option  \tDefault\t\tDescription");
    puts("------- \t---------------\t-----------------------------------");
    puts("-H      \t\t\tDisplay this information");
    printf("-p<port>\t%-15d Server port to bind to\n", port);
    printf("-b      \t\t\tUse bound threads\n");
    printf("-Q<num> \t%-15d Parallell processes (if nonzero)\n",
           n_processes);
    printf("-c<num> \t%-15d Thread concurrency level if nonzero\n",
           concurrency);
    printf("-s<size>\t%-15d Thread stack size, if nonzero\n",
           stack_size);
    printf("-l<size>\t%-15d Reply buffer size\n",
           mbufsize);

    return 0;
}

int main(int argc, char *argv[])
{
    mutex_t *accept_lock = NULL;
    int *int_val = NULL;
    int sock_fd, fd;
    int one = 1;
    struct sockaddr_in sin;
    int *fdp;
    int len, i;
    struct rlimit rlb;


    for (i = 1; i < argc && argv[i][0] == '-'; i++)
        switch (argv[i][1])
        {
        case '?':
        case 'H':
            exit(help(argv[0]));
            break;

        case 'Q':
            n_processes = atoi(argv[i]+2);
            break;

        case 'p':
            port = atoi(argv[i]+2);
            break;

        case 's':
            stack_size = atoi(argv[i]+2);
            break;

        case 'b':
            bound = 1;
            break;

        case 'E':
            use_exit = 1;
            break;

        case 'l':
            mbufsize = atoi(argv[i]+2);
            if (mbufsize < 128)
            {
                fprintf(stderr, "%s: buffer length (%d) is too small\n",
                        argv[0], mbufsize);
                exit(1);
            }
            break;

        case 'c':
            concurrency = atoi(argv[i]+2);
            break;

        default:
            fprintf(stderr, "%s: unknown switch '%s', try -H for help\n",
                    argv[0], argv[1]);
            exit(1);
        }

    mbuf = malloc(mbufsize);
    if (mbuf == NULL)
    {
        perror("malloc");
        exit(1);
    }

    if (getrlimit(RLIMIT_NOFILE, &rlb) < 0)
    {
        perror("getrlimit()");

        rlb.rlim_max = 64;
    }

    rlb.rlim_cur = rlb.rlim_max;

    if (setrlimit(RLIMIT_NOFILE, &rlb) < 0)
        perror("setrlimit()");

    sprintf(mbuf, "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\n");
    len = strlen(mbuf);
    memset(mbuf+len, 'A', mbufsize-len);

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(port);

    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("sock_fd");
        exit(1);
    }

    setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, (void *) &one, sizeof(one));

    if (bind(sock_fd, (struct sockaddr *) &sin, sizeof(sin)) < 0)
    {
        perror("bind");
        exit(1);
    }

    if (listen(sock_fd, 1024) < 0)
    {
        perror("listen");
        exit(1);
    }

    if (n_processes > 0)
    {
        pid_t pid;
        int child_status, fd;


        while ((fd = open("/dev/zero", 2)) < 0 && errno == EINTR)
            ;

        if (fd < 0)
        {
            perror("open(\"/dev/zero\")");
            exit(1);
        }

        accept_lock = (mutex_t *) mmap((caddr_t) NULL,
                                       sizeof(mutex_t),
                                       PROT_READ+PROT_WRITE,
                                       MAP_SHARED, fd, 0);

        int_val = (int *) mmap((caddr_t) NULL,
                               sizeof(int),
                               PROT_READ+PROT_WRITE,
                               MAP_SHARED, fd, 0);

        while (close(fd) < 0 && errno == EINTR)
            ;

        mutex_init(accept_lock, USYNC_PROCESS, NULL);
        *int_val = 0;

        for (i = 0; i < n_processes; i++)
        {
            if (fork() == 0)
                goto Request;
        }

        while (i > 0)
        {
            while ((pid = waitpid(-1, &child_status, 0)) < 0 && errno == EINTR)
                ;

            printf("Process %d exited\n", (int) pid);
            --i;
        }
        exit(0);
    }

Request:
    thr_setconcurrency(concurrency);

Loop:
    if (n_processes > 0)
        mutex_lock(accept_lock);

    while ((fd = accept(sock_fd, NULL, NULL)) < 0 &&
            (errno == EINTR || errno == EPROTO))
    {
        if (errno == EPROTO)
            putc('.', stderr);
    }

    if (n_processes > 0)
        mutex_unlock(accept_lock);

    if (fd < 0)
    {
        perror("accept");
        _exit(1);
    }

    fdp = (int *) malloc(sizeof(int));
    if (fdp == NULL)
    {
        perror("malloc");
        _exit(1);
    }

    *fdp = fd;

    if (thr_create(NULL,
                   stack_size,
                   request_thread,
                   fdp,
                   THR_DETACHED+THR_DAEMON+(bound?THR_BOUND:0),
                   NULL) < 0)
    {
        perror("thr_create");
        _exit(1);
    }
    goto Loop;
}


