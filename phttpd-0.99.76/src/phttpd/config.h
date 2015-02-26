/*
** config.h
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

#ifndef __CONFIG_H__
#define __CONFIG_H__



#define T_NUMBER 1
#define T_STRING 2
#define T_TABLE  3
#define T_BOOL   4
#define T_HTABLE 5
#define T_FILE   6



struct options
{
    char *name;
    int type;
    void *var;
    int (*var_cr_f)(char *cmd, char *buf, void *var);
    char flag;
    int def_number;
    char *def_string;
    char *help;
};

extern int config_parse_file(const char *file,
                             struct options *op,
                             int verbose);

extern int config_parse_argv(const char **argv,
                             struct options *op);

#endif

