/*
** config.c
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <pwd.h>
#include <grp.h>

#include "phttpd.h"


static int get_bool(const char *str)
{
    return (strcasecmp(str, "yes") == 0 ||
            strcasecmp(str, "true") == 0 ||
            strcasecmp(str, "on") == 0 ||
            strcasecmp(str, "1") == 0 ||
            strcasecmp(str, "light") == 0 ||
            strcasecmp(str, "up") == 0);
}


static int get_number(const char *str)
{
    int num = 0;
    int neg_flag = 0;


    if (*str == '-')
    {
        neg_flag = !neg_flag;
        ++str;
    }

    while (*str && s_isdigit(*str))
    {
        num *= 10;
        num += (*str - '0');

        ++str;
    }

    switch (*str)
    {
    case 'k':
        num *= 1000;
        break;

    case 'K':
        num *= 1024;
        break;

    case 'M':
        num *= 1024*1024;
        break;
    }

    return (neg_flag ? -num : num);
}


static char **mk_pair(const char *a0, const char *a1)
{
    char **pair;

    pair = s_malloc(2 * sizeof(char *));

    pair[0] = s_strdup(a0);
    pair[1] = s_strdup(a1);

    return pair;
}




static int get_value(struct options *op,
                     int index,
                     const char *value)
{
    switch (op[index].type)
    {
    case T_NUMBER:
        if (value == NULL)
            * (int *) op[index].var = op[index].def_number;
        else
            * (int *) op[index].var = get_number(value);
        return 0;

    case T_STRING:
        if (value == NULL)
            * (char **) op[index].var = s_strdup(op[index].def_string);
        else
            * (char **) op[index].var = s_strdup(value);
        return 0;

    case T_BOOL:
        if (value == NULL)
            * (int *) op[index].var = op[index].def_number;
        else
            * (int *) op[index].var = get_bool(value);
        return 0;

    case T_FILE:
        if (value)
            return config_parse_file(value, op, 1);

    default:
        return -1;
    }

}


int config_parse_file(const char *file,
                      struct options *op,
                      int verbose)
{
    int fd;
    char buf[2048], *cp, *cmd, *val, *last;
    int line, i = 0;
    struct table *tp = NULL;
    hashtable_t *htp = NULL;


    if (debug > 2)
        fprintf(stderr, "config_parse_file, path=%s\n", file);

    line = 0;

    fd = fd_open(file, O_RDONLY);
    if (fd < 0)
    {
        if (verbose)
        {
            fprintf(stderr, "Config file %s not found.\n", file);
            return -1;
        }
        else
            return 0;
    }

    while (fd_gets(buf, sizeof(buf), fd))
    {
        line++;

        cp = strchr(buf, '#');
        if (cp)
            *cp = '\0';

        cp = buf + strlen(buf) - 1;
        while (cp >= buf && s_isspace(*cp))
            --cp;

        ++cp;
        *cp = '\0';

        last = buf;

        cmd = strtok_r(buf, " =\t\n\r", &last);

        if (tp || htp)
            val = strtok_r(NULL, "\n\r", &last);
        else
            val = strtok_r(NULL, " \t\n\r", &last);

        if (cmd == NULL)
            continue;

        if (val)
            while (s_isspace(*val))
                ++val;

        if (htp)
        {
            if (strcmp(cmd, "}") == 0)
            {
                htp = NULL;
            }
            else
            {
                if (op[i].var_cr_f)
                {
                    void *var;

                    if (op[i].var_cr_f(cmd, val, &var) == 1)
                        ht_insert(htp, cmd, 0, var, 0, s_free);
                }
                else
                {
                    ht_insert(htp, cmd, 0, s_strdup(val), 0, s_free);
                }
            }

            continue;
        }

        if (tp)
        {
            if (strcmp(cmd, "}") == 0)
            {
                tp = NULL;
            }
            else
            {
                if (op[i].var_cr_f)
                {
                    void *var;

                    if (op[i].var_cr_f(cmd, val, &var) == 1)
                        tbl_append(tp, var);
                }
                else
                {
                    char **pair = mk_pair(cmd, val);

                    if (pair == NULL)
                        continue;

                    tbl_append(tp, pair);
                }
            }

            continue;
        }

        if (val && strcmp(val, "=") == 0)
            val = strtok_r(NULL, "\t\n\r", &last);

        for (i = 0; op[i].name; i++)
            if (strcmp(op[i].name, cmd) == 0)
                break;

        if (op[i].name)
            switch (op[i].type)
            {
            case T_FILE:
            case T_BOOL:
            case T_NUMBER:
            case T_STRING:
                get_value(op, i, val);
                break;

            case T_TABLE:
                if (val && strcmp(val, "{") == 0)
                {
                    tp = tbl_create(0);
                    * (struct table **) op[i].var = tp;
                }
                else
                {
                    if (debug > 2)
                        fprintf(stderr,
                                "Illegal table definition on line %d.\n",
                                line);
                }
                break;

            case T_HTABLE:
                if (val && strcmp(val, "{") == 0)
                {
                    htp = s_malloc(sizeof(hashtable_t));
                    ht_init(htp, 0, NULL);

                    * (hashtable_t **) op[i].var = htp;
                }
                else
                {
                    if (debug > 2)
                        fprintf(stderr,
                                "Illegal table definition on line %d.\n",
                                line);
                }
                break;
            }
        else
        {
            if (debug > 2)
                fprintf(stderr, "Unknown command '%s' on line %d.\n",
                        cmd, line);
        }
    }

    fd_close(fd);
    if (debug > 2)
        fprintf(stderr, "config_parse_file, exiting\n");
    return 0;
}

static char *type2str(int type)
{
    switch (type)
    {
    case T_BOOL:
        return "Boolean";

    case T_NUMBER:
        return "Number";

    case T_STRING:
        return "String";

    case T_FILE:
        return "String";

    default:
        return NULL;
    }
}

static int help(struct options *op)
{
    int i;
    char *type;


    fprintf(stderr, "Available options/switches:\n");

    for (i = 0; op[i].name; i++)
    {
        type = type2str(op[i].type);
        if (type)
            fprintf(stderr, "   %s%c\t--%-20s\t%s\t  %s\n",
                    op[i].flag ? "-" : "",
                    op[i].flag ? op[i].flag : ' ',
                    op[i].name,
                    type,
                    op[i].help ? op[i].help : "");
    }

    return 1;
}

int config_parse_argv(const char **argv,
                      struct options *op)
{
    int i, j, len;
    const char *cp;


    if (argv == NULL)
        return 0;

    for (i = 0; argv[i] && argv[i][0] == '-'; i++)
    {
        if (argv[i][1] == '-')
        {
            /* --long-option */

            if (debug > 2)
                fprintf(stderr, "config_parse_argv: Got long option: %s\n", argv[i]);

            if (argv[i][2] == '\0')
                return ++i;

            cp = strchr(argv[i]+2, '=');
            len = cp - (argv[i]+2);

            if (cp)
                ++cp;
            else
            {
                if (argv[i+1] && argv[i+1][0] != '-')
                    cp = argv[++i];
                else
                    cp = NULL;
            }

            for (j = 0; op[j].name &&
                    !(strncmp(op[j].name, argv[i]+2, len) == 0 &&
                      op[j].name[len] == '\0'); j++)
                ;

            if (op[j].name)
                get_value(op, j, cp);
            else
            {
                if (strcmp(argv[i]+2, "help") == 0)
                    exit(help(op));

                fprintf(stderr, "Illegal command line option: %s\n", argv[i]);

                return -i;
            }
        }
        else
        {
            /* -s hort option */
            if (debug > 2)
                fprintf(stderr, "config_parse_argv: Got short option: %s\n", argv[i]);

            for (j = 0; op[j].name && argv[i][1] != op[j].flag; j++)
                ;

            if (op[j].name)
            {
                if (argv[i][2])
                    get_value(op, j, argv[i]+2);
                else if (argv[i+1] && argv[i+1][0] != '-')
                    get_value(op, j, argv[++i]);
                else
                    get_value(op, j, NULL);

            }
            else
            {
                if (argv[i][1] == '?' || argv[i][1] == 'h' || argv[i][1] == 'H')
                    exit(help(op));

                fprintf(stderr, "Illegal command line option: %s\n", argv[i]);
                return -i;
            }
        }
    }

    return i;
}
