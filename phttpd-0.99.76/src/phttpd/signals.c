/*
** signals.c
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

#include <signal.h>
#include <syslog.h>

#include "phttpd.h"


static void dummy_handler()
{
    /* Nothing */
}

#if 0
static void sigusr1_handler()
{
    debug++;
}

static void sigusr2_handler()
{
    debug--;
}


static void sigterm_handler()
{
    syslog(LOG_NOTICE, "server terminating by SIGTERM request");
    exit(0);
}
#endif



void signals_setup(sigset_t *sig_set)
{
    struct sigaction actb;


    sigemptyset(sig_set);
    sigaddset(sig_set, SIGHUP);
    sigaddset(sig_set, SIGUSR1);
    sigaddset(sig_set, SIGUSR2);
    sigaddset(sig_set, SIGTERM);
    sigaddset(sig_set, SIGALRM);
    sigaddset(sig_set, SIGPIPE);
    sigaddset(sig_set, SIGCHLD);
    thr_sigsetmask(SIG_BLOCK, sig_set, NULL);


    /* Mainly for SGI/IRIX */
    actb.sa_handler = SIG_IGN;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGALRM, &actb, NULL);

    actb.sa_handler = SIG_IGN;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGPIPE, &actb, NULL);

    actb.sa_handler = SIG_IGN;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGCHLD, &actb, NULL);

    actb.sa_handler = dummy_handler;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGHUP, &actb, NULL);

    actb.sa_handler = dummy_handler;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGUSR1, &actb, NULL);

    actb.sa_handler = dummy_handler;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGUSR2, &actb, NULL);

    actb.sa_handler = dummy_handler;
    actb.sa_flags = 0;
    sigemptyset(&actb.sa_mask);
    sigaction(SIGTERM, &actb, NULL);
}
