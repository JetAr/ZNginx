/*
** tester.c
**
** Copyright (c) 1994 Peter Eriksson <pen@signum.se>
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
#include <signal.h>
#include <unistd.h>

#include <wait.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <arpa/inet.h>


void do_test(struct sockaddr *sa, char *object)
{
    FILE *fp_in, *fp_out;
    int fd;
    int j;
    int len = (unsigned) rand() % 32768 + 236;


    fd = socket(AF_INET, SOCK_STREAM, 0);

    if (connect(fd, sa, sizeof(struct sockaddr_in)) < 0)
    {
        perror("connect");
        exit(1);
    }

    fp_out = fdopen(fd, "w");
    fp_in  = fdopen(dup(fd), "r");

    fprintf(fp_out, "GET %s HTTP/1.0\r\n", object);
    fprintf(fp_out, "From: Ptester\r\n\r\n");
    fflush(fp_out);

    for (j = 0; j < len && getc(fp_in) != EOF; j++)
        ;

    fclose(fp_in);
    fclose(fp_out);
}


int main(int argc, char *argv[])
{
    struct sockaddr_in sin;
    int n_testers, i, j, k;
    int n_tests;
    int status;


    if (argc < 6)
    {
        fprintf(stderr,
                "Usage: %s <address> <port> <testers> <tests> <object>\n",
                argv[0]);
        exit(1);
    }

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr(argv[1]);
    sin.sin_port = htons(atoi(argv[2]));

    n_testers = atoi(argv[3]);
    n_tests   = atoi(argv[4]);

    printf("n_tests = %d\n", n_tests);

    for (i = 0; i < n_testers; i++)
    {
        if (fork() == 0)
        {
            for (k = 0; k < n_tests; k++)
            {
                for (j = 5; j < argc; j++)
                    do_test((struct sockaddr *) &sin, argv[j]);
            }
            exit(0);
        }
    }

    while (wait(&status) >= 0)
        ;

    exit(0);
}
