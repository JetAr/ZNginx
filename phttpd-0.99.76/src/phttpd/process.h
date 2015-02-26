/*
** process.h
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

#ifndef PHTTPD_PROCESS_H
#define PHTTPD_PROCESS_H

#define PROC_EXIT   0
#define PROC_SIGNAL 1
#define PROC_OTHER  2

#include <sys/resource.h>

extern int proc_run(char *path,
                    uid_t uid, gid_t gid, char *cgi_newroot,
                    rlim_t niceval, rlim_t vmem, rlim_t fd_max, rlim_t maxcputime,
                    int stdin_fd, int stdout_fd, int stderr_fd,
                    char **argv, char **envp,
                    char *wdir);

extern int proc_wait(pid_t pid, int *code);

#endif
