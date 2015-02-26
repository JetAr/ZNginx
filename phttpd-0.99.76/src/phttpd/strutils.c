/*
** strutils.c
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

#include <string.h>
#include "phttpd.h"

char **strsplit(char *path, int delim, int unquote)
{
    char *cp;
    char **av;
    int ac;
    int size;
    int len;


    size = 32;
    ac = 0;

    av = s_malloc(size * sizeof(char *));

    while ((cp = strchr(path, delim)) != NULL)
    {
        if (ac+1 >= size)
            av = s_realloc(av, sizeof(char *) * (size += 32));

        len = cp - path;
#if 0
        av[ac] = s_malloc(len + 1);
        memcpy(av[ac], path, len);
        av[ac][len] = '\0';
#else
        av[ac] = path;
        *cp = '\0';
#endif
        if (unquote)
            url_unquote(av[ac], unquote-1);

        path = cp+1;
        ac++;
    }

    if (ac+1 >= size)
        av = s_realloc(av, sizeof(char *) * (size += 32));

#if 0
    av[ac] = s_strdup(path);
#else
    av[ac] = path;
#endif

    if (unquote)
        url_unquote(av[ac], unquote-1);

    ac++;
    av[ac] = NULL;

    return av;
}

#if 0
void argv_free(char **argv)
{
    int i;


    if (argv == NULL)
        return;

    for (i = 0; argv[i]; i++)
        s_free(argv[i]);
    s_free(argv);
}
#endif
