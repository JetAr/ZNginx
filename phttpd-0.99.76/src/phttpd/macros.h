/*
** macros.h
**
** Copyright (c) 1995-1997 Peter Eriksson <pen@signum.se>
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

#ifndef PHTTPD_MACROS_H
#define PHTTPD_MACROS_H

#define S_STRALLOCA(src,dst,xtra,size)	\
{					\
    int len;				\
					\
    len = strlen(src);			\
    *(dst) = alloca(len+1+(xtra));	\
    if (*(dst) == NULL)			\
        s_abort();			\
    memcpy(*(dst), (src), len+1);	\
    if (size)				\
        *(size) = len+1+(xtra);		\
}

#endif
