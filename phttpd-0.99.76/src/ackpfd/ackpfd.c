/*
** ackpfd.c         "Accepting Child Keepalive Protocol Forking Daemon"
**
** Copyright (c) 1995 Peter Eriksson <pen@signum.se>
**
** Special thanks go to Per Cederqvist <ceder@signum.se> for coming up
** with what "ackpfd" stands for.
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
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <time.h>
#include <syslog.h>
#include <signal.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <setjmp.h>

#include <sys/time.h>
#include <sys/resource.h>

#include <sys/wait.h>

#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

int n_processes = 1;
int backlog = 1024;
int port = 80;
int debug = 0;
unsigned long address = INADDR_ANY;
struct passwd *pwp = NULL;
int uid = -1;
int gid = -1;
char *root_dir = NULL;
int subservers = 0;
pid_t pid;



void xsleep(int timeout)
{
    time_t start, now;
    int len;


    time(&start);
    len = timeout;

    while (len > 0)
    {
        sleep(len);
        time(&now);

        len = timeout - (now - start);
    }
}


void help(void)
{
    fprintf(stderr,
            "Syntax: ackpfd [<ackpfd options>] <program path> [<program options>]\n");

    fprintf(stderr, "\nWhere <ackpfd options> may be:\n\n");
    fprintf(stderr, "\t-d ................ Enable debugging\n");
    fprintf(stderr, "\t-b<backlog> ....... Change the listen() backlog\n");
    fprintf(stderr, "\t-a<address> ....... Bind to this address only\n");
    fprintf(stderr, "\t-p<port> .......... Bind to this port\n");
    fprintf(stderr, "\t-u<uid> ........... Run program as this user\n");
    fprintf(stderr, "\t-g<gid> ........... Run program as this group\n");
    fprintf(stderr, "\t-r<rootdir> ....... Chroot to this directory\n");

    fprintf(stderr, "\nThe defaults are:\n\n");
    fprintf(stderr, "\tBacklog ........... %d\n", backlog);
    fprintf(stderr, "\tAddress ........... %s\n",
            address == INADDR_ANY ? "*" :
            inet_ntoa(* (struct in_addr *) &address));
    fprintf(stderr, "\tPort .............. %d\n", port);
}


int parse_options(int argc,
                  char *argv[])
{
    int i;


    for (i = 1; i < argc && argv[i][0] == '-'; i++)
        switch (argv[i][1])
        {
        case 'd':
            debug = 1;
            break;

        case 'b':
            backlog = atoi(argv[i]+2);
            break;

        case 'a':
            if (isdigit(argv[i][2]))
                address = inet_addr(argv[i]+2);
            else
            {
                struct hostent *hp;

                hp = gethostbyname(argv[i]+2);
                if (hp == NULL)
                {
                    syslog(LOG_ERR, "gethostbyname(\"%s\") failed", argv[i]+2);
                    exit(1);
                }

                address = * (unsigned long *) (hp->h_addr_list[0]);
            }

            break;

        case 'P':
            n_processes = atoi(argv[i]+2);
            break;

        case 'p':
            port = atoi(argv[i]+2);
            break;

        case 'u':
            if (isdigit(argv[i][2]))
                uid = atoi(argv[i]+2);
            else
            {
                pwp = getpwnam(argv[i]+2);
                if (pwp == NULL)
                {
                    syslog(LOG_ERR, "getpwnam(\"%s\") failed", argv[i]+2);
                    exit(1);
                }

                uid = pwp->pw_uid;
                gid = pwp->pw_gid;
            }
            break;

        case 'g':
            if (isdigit(argv[i][2]))
                gid = atoi(argv[i]+2);
            else
            {
                struct group *grp;

                grp = getgrnam(argv[i]+2);
                if (grp == NULL)
                {
                    syslog(LOG_ERR, "getgrnam(\"%s\") failed", argv[i]+2);
                    exit(1);
                }

                gid = grp->gr_gid;
            }
            break;

        case 'r':
            root_dir = argv[i];
            break;

        case 'h':
        case '?':
            help();
            exit(0);

        default:
            syslog(LOG_ERR, "unknown command line switch: %s\n", argv[i]);
            exit(1);
        }

    return i;
}

void become_daemon(void)
{
    pid_t pid;
    int fd, i;
    struct rlimit rlb;


    pid = fork();
    if (pid < 0)
    {
        syslog(LOG_ERR, "fork() failed: %m");
        exit(1);
    }
    else if (pid > 0)
        exit(0);


    if (getrlimit(RLIMIT_NOFILE, &rlb) < 0)
    {
        syslog(LOG_ERR, "getrlimit() failed: %m");
        perror("ackpfd: getrlimit()");
        exit(1);
    }

    rlb.rlim_cur = rlb.rlim_max;

    if (setrlimit(RLIMIT_NOFILE, &rlb) < 0)
    {
        syslog(LOG_ERR, "setrlimit() failed: %m");

        perror("ackpfd: setrlimit()");
        exit(1);
    }

    for (i = 0; i < rlb.rlim_cur; i++)
    {
        if (!(debug && i == 2))
            while (close(i) < 0 && errno == EINTR)
                ;
    }

    while ((fd = open("/dev/null", O_RDWR)) < 0 && errno == EINTR)
        ;

    if (fd < 0)
    {
        syslog(LOG_ERR, "open(\"/dev/null\") failed: %m");
        perror("open(\"/dev/null\")");
        exit(1);
    }

    dup2(fd, 1);

    if (!debug)
        dup2(fd, 2);

    setsid();
}


int listen_socket(unsigned long address,
                  int port,
                  int backlog)
{
    int sock_fd, val;
    struct sockaddr_in sin;


    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0)
    {
        syslog(LOG_ERR, "socket() failed: %m");
        exit(1);
    }

    val = 1;
    if (setsockopt(sock_fd,
                   SOL_SOCKET,
                   SO_REUSEADDR,
                   (void *) &val,
                   sizeof(val)) < 0)
        syslog(LOG_WARNING, "setsockopt(SO_REUSEADDR) failed: %m");

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = address;
    sin.sin_port = htons(port);

    if (bind(sock_fd, (struct sockaddr *) &sin, sizeof(sin)))
    {
        syslog(LOG_ERR, "bind(%s, %d) failed: %m",
               (address == INADDR_ANY ? "*" :
                inet_ntoa(* (struct in_addr *) &address)),
               port);
        exit(1);
    }

    if (listen(sock_fd, backlog))
        syslog(LOG_WARNING, "listen(..., %d) failed: %m", backlog);

    return sock_fd;
}


int child_process(int sock_fd, char **argv)
{
    int status;


    if (sock_fd != 0)
    {
        if (dup2(sock_fd, 0) < 0)
        {
            syslog(LOG_ERR, "dup2(%d, 0) failed: %m", sock_fd);
            return -errno;
        }

        while (close(sock_fd) < 0 && errno == EINTR)
            ;
    }

    if (pwp != NULL)
    {
        if (initgroups(pwp->pw_name, pwp->pw_gid) < 0)
        {
            syslog(LOG_ERR, "initgroups(\"%s\", %d) failed: %m",
                   pwp->pw_name, pwp->pw_gid);
            return -errno;
        }
    }

    if (gid != -1)
        if (setgid(gid) < 0)
        {
            syslog(LOG_ERR, "setgid(%d) failed: %m", gid);
            return -errno;
        }

    if (uid != -1)
        if (setuid(uid) < 0)
        {
            syslog(LOG_ERR, "setuid(%d) failed: %m", uid);
            return -errno;
        }

    if (root_dir)
    {
        while ((status = chroot(root_dir)) < 0 && errno == EINTR)
            ;

        if (status < 0)
        {
            syslog(LOG_ERR, "chroot(\"%s\") failed: %m", root_dir);
            return -errno;
        }

        while ((status = chdir("/")) < 0 && errno == EINTR)
            ;

        if (status < 0)
        {
            syslog(LOG_ERR, "chdir(\"/\") failed: %m");
            return -errno;
        }
    }

    syslog(LOG_NOTICE, "starting: %s", argv[0]);

    execv(argv[0], argv);

    syslog(LOG_ERR, "execv(\"%s\") failed: %m", argv[0]);
    return -errno;
}



static void sigterm_handler()
{
    syslog(LOG_NOTICE, "sigterm received, exiting");
    if (pid > 2)
        kill(pid, SIGTERM);
    exit(1);
}


int main(int argc,
         char *argv[])
{
    int i, sock_fd, status;
    struct sigaction actb;
    time_t start_time, cur_time;

    openlog("ackpfd", LOG_PID, LOG_DAEMON);

    i = parse_options(argc, argv);

    if (i >= argc)
    {
        syslog(LOG_ERR, "missing program path");
        exit(1);
    }

    become_daemon();

    actb.sa_handler = sigterm_handler;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGTERM, &actb, NULL);

    sock_fd = listen_socket(address, port, backlog);


Loop:
    time(&start_time);

    subservers++;
    pid = fork();
    if (pid < 0)
        syslog(LOG_ERR, "fork() failed: %m");

    else if (pid == 0)
        _exit(child_process(sock_fd, argv+i));

    /* Parent process */

    actb.sa_handler = SIG_IGN;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGCHLD, &actb, NULL);

Wait:
    status = 0;
    while  ((pid = waitpid(-1, &status, 0)) < 0 && errno == EINTR)
        ;

    time(&cur_time);

    if (pid < 0)
        syslog(LOG_ERR, "waitpid() failed: %m");
    else
    {
        if (WIFEXITED(status))
        {
            status = WEXITSTATUS(status);
            if (status != 0)
                syslog(LOG_ERR,
                       "process %d exited with code #%d",
                       pid, status);
            subservers--;
        }

        else if (WIFSIGNALED(status))
        {
            status = WTERMSIG(status);
            syslog(LOG_ERR,
                   "process %d exited with signal #%d",
                   pid, status);
            subservers--;
        }

        else if (WIFSTOPPED(status))
        {
            status = WSTOPSIG(status);
            syslog(LOG_ERR,
                   "process %d stopped due to signal #%d",
                   pid, status);
            goto Wait;
        }

        else if (WIFCONTINUED(status))
        {
            syslog(LOG_ERR, "process %d continued",
                   pid);
            goto Wait;
        }

        else
        {
            syslog(LOG_ERR, "strange process event notification: %d\n",
                   status);
            goto Wait;
        }
    }

    if (subservers > 0)
        goto Wait;

    if (cur_time == start_time)
    {
        syslog(LOG_NOTICE, "process looping - sleeping one minute");
        xsleep(60);
    }

    goto Loop;
}

