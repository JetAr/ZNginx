/************************************************************************/
/* strmatch - matches two strings					*/
/*									*/
/* Author: Marcus E. Hennecke <marcush@leland.stanford.edu>		*/
/* Copyright (C) 1995 by Marcus E. Hennecke				*/
/* This program is free software; you can redistribute it and/or modify	*/
/* it under the terms of the GNU General Public License as published by	*/
/* the Free Software Foundation; either version 2 of the License, or	*/
/* (at your option) any later version.					*/
/*									*/
/* This program is distributed in the hope that it will be useful,	*/
/* but WITHOUT ANY WARRANTY; without even the implied warranty of	*/
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	*/
/* GNU General Public License for more details.				*/
/* You should have received a copy of the GNU General Public License	*/
/* along with this program; if not, write to the Free Software		*/
/* Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.		*/
/************************************************************************/

#include <stdio.h>

int strmatch(const char *string, const char *pattern)
{
    const char *ssubstart = NULL, *psubstart = NULL;
    char sch, pch;

    for (; (sch = *string); string++, pattern++)
    {
        pch = *pattern;
        if (pch == '*')
        {
            ssubstart = string;
            do
            {
                psubstart = pattern;
                pch = *++pattern;
            }
            while (pch == '*');
            if (pch == '\0')
                return 1;
        }

        if (pch == '?')
            continue;

        if (pch == '\\')
            pch = *++pattern;

        if (pch == sch)
            continue;

        if (psubstart == NULL)
            return 0;

        pattern = psubstart;
        string = ssubstart++;
    }

    while (*pattern == '*')
        pattern++;
    return (*pattern == '\0');
}
