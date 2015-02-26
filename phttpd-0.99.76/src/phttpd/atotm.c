/************************************************************************/
/* atotm - converts a date in ASCII format to both a tm structure and	*/
/* the time in seconds. Can be used to compare two times.		*/
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
/*									*/
/* This function is extremely liberal in what it accepts as date or	*/
/* time. Essentially, it looks at certain patterns and tries to		*/
/* interpret them in reasonable ways. For example, any string of	*/
/* digits with one or two colons is a time of the format hh:mm or	*/
/* hh:mm:ss (with at least one digit required for hh, mm, or ss). If	*/
/* the seconds are omitted, they are assumed to be zero.		*/
/* All numbers that are not part of a time are assumed to be part of a	*/
/* date (i.e. either day, month, or year). Thus, there must be either	*/
/* three such numbers or two such numbers and a month name. A month	*/
/* name is a (possibly abbreviated to at least three characters)	*/
/* English name of a month.						*/
/************************************************************************/

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "phttpd.h"



static const char *days[] =
{
    "Sunday", "Monday", "Tuesday", "Wednesday",
    "Thursday", "Friday", "Saturday"
};

struct month_t
{
    const char *name;
    int length;
};

static const struct month_t months[] =
{
    {"January", 31}, {"February", 28}, {"March", 31}, {"April", 30},
    {"May", 31}, {"June", 30}, {"July", 31}, {"August", 31},
    {"September", 30}, {"October", 31}, {"November", 30},
    {"December", 31}
};

int atotm(const char *date_str, struct tm *date)
{
    const char *token;
    int i, ch, len, tmp;
    int value, values[3], value_ctr;
    int colons, slashes, dashes;
    int date_values[3], date_ctr, mday, mon, year, yday, seconds, leapday;
    int safe_month = -1;
    int ispm = 0;
    int isam = 0;
    struct tm dummy;

    if ( date == NULL )
        date = &dummy;

    mon = 0;

    date->tm_sec = date->tm_min = date->tm_hour = date->tm_mday =
                                      date->tm_mon = date->tm_year = date->tm_wday = date->tm_yday =
                                              date->tm_isdst = -1;

    date_ctr = 0;
    while ( (ch = *date_str++) != '\0' )
    {
        if ( !s_isalnum(ch) ) continue;

        if ( s_isdigit(ch) )  	/* Is a number */
        {

            /* Read in number and maybe following numbers */
            value = ch - '0';
            len = 1;
            value_ctr = colons = slashes = dashes = 0;
            ch = *date_str;
            while ( s_isdigit(ch) || ch == ':' || ch == '/' || ch == '-')
            {
                if ( s_isdigit(ch) )
                {
                    value = value*10 + ch - '0';
                    len++;
                }
                else
                {
                    if ( value_ctr >= 2 ) return(-1);	/* Too many numbers */
                    if ( len == 0 ) return(-1);		/* Empty number (e.g. --) */
                    switch (ch)
                    {
                    case ':':
                        colons++;
                        break;
                    case '/':
                        slashes++;
                        break;
                    case '-':
                        dashes++;
                        break;
                    };
                    values[value_ctr++] = value;
                    value = len = 0;
                };
                ch = *++date_str;
            };
            if ( len )
                values[value_ctr] = value;
            else
                value_ctr--;

            /* Try to make sense of the numbers read. */
            if ( colons >= 1 )  	/* Must have been time */
            {
                if ( colons != value_ctr )
                    return(-1);	/* Mixed colons, dashes, and/or slashes */
                if ( date->tm_hour >= 0 ) return(-1);	/* Two times */
                if ( values[0] >= 24 ) return(-1);
                date->tm_hour = values[0];
                if ( values[1] >= 60 ) return(-1);
                date->tm_min = values[1];
                if ( colons == 2 )
                {
                    if ( values[2] > 61 )	/* 61 to allow for leap seconds */
                        return(-1);
                    date->tm_sec = values[2];
                }
                else
                    date->tm_sec = 0;
            }
            else  			/* Must have been date */
            {
                if ( value_ctr == 2 )  	/* We have a complete date */
                {
                    if ( date_ctr > 0 ) return(-1);	/* Already had date */
                    if ( slashes != 2 && dashes != 2 )
                        return(-1);		/* Mixed slashes and dashes */
                    date_values[0] = values[0];
                    date_values[1] = values[1];
                    date_values[2] = values[2];
                    date_ctr = 3;
                }
                else if ( value_ctr == 0 )  	/* Just one number */
                {
                    if ( date_ctr >= 3 ) return(-1);
                    date_values[date_ctr++] = values[0];
                }
                else
                    return(-1);
            };

        }
        else  			/* Is a letter string */
        {
            token = date_str - 1;	/* Remember starting position */
            while (s_isalpha(*date_str))
                date_str++;	/* Skip letters */

            len = date_str - token;	/* Compute length */

            if ( len == 2 && token[1] == 'm' )
                if ( token[0] == 'p' )
                    ispm = 1;
                else if ( token[0] == 'a' )
                    isam = 1;

            if ( len < 3 ) continue;

            for ( i = 0; i < 7; i++ )
                if ( !strncasecmp(token, days[i], len) )
                    if ( date->tm_wday < 0 || date->tm_wday == i )
                    {
                        date->tm_wday = i;
                        break;
                    }
                    else
                        return(-1);		/* More than one weekday */

            if ( i < 7 ) continue;
            for ( i = 0; i < 12; i++ )
                if ( !strncasecmp(token, months[i].name, len) )
                {
                    if ( safe_month >= 0 && safe_month != i )
                        return(-1);		/* More than one month */
                    date->tm_mon = mon = safe_month = i;
                    break;
                }
            if ( i < 12 ) continue;
        };
    };

    /* If we didn't find a time, check out if maybe the date was only an */
    /* integer number (i.e. no month and no weekday and no more than one */
    /* number). If there was only one number, use it as number of */
    /* seconds and return immediately. Otherwise, report an error. */
    if ( date->tm_hour < 0 )	/* There was no time */
        if ( date_ctr == 1 && safe_month < 0 && date->tm_wday < 0 )
            return(date_values[0]);
        else
            return(-1);

    if ( isam || ispm )
    {
        if ( ispm && isam ) return(-1);
        if ( date->tm_hour >= 12 )
            date->tm_hour -= 12;
        if (ispm)
            date->tm_hour += 12;
    };

    if ( safe_month >= 0 )
    {
        if ( date_ctr < 2 ) return(-1);
        mday = date_values[0];
        year = date_values[1];
        if ( date_ctr == 3 )
            if        ( date_values[0] == safe_month )
            {
                mday = date_values[1];
                year = date_values[2];
            }
            else if ( date_values[1] == safe_month )
                year = date_values[2];
        leapday = (mon == 1 && (year & 0x3) ? 0 : 1);
        if ( mday > months[mon].length+leapday &&
                year <= months[mon].length+leapday  )
        {
            tmp = mday;
            mday = year;
            year = tmp;
        };
        leapday = (mon == 1 && (year & 0x3) ? 0 : 1);
        if ( mday > months[mon].length+leapday ) return(-1);
    }
    else
    {
        if ( date_ctr != 3 ) return(-1);
        mon  = date_values[0] - 1;
        mday = date_values[1];
        year = date_values[2];
    };

    if ( year >= 1900 ) year -= 1900;
    if ( year < 70 )    year += 100;

    date->tm_mday = mday;
    date->tm_mon  = mon;
    date->tm_year = year;

    yday = mday-1;
    for ( i = 0; i < mon; i++ ) yday += months[i].length;
    if ( mon > 1 && (year & 0x3) == 0 ) yday++;
    date->tm_yday = yday;

    seconds = year - 70;
    if ( seconds < 0 ) seconds += 100;
    seconds = seconds * 365 + (seconds+1)/4;
    seconds += yday;
    seconds *= 24*60*60;
    seconds += (date->tm_hour * 60 + date->tm_min) * 60 + date->tm_sec;

    return( seconds );
}
