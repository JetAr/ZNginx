/*
** html.c
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
#include <stdarg.h>

#include "phttpd.h"


void html_href(int fd,
               const char *href, const char *text)
{
    char buf[2048];


    fd_printf(fd, "<A HREF=\"%s\">",
              html_strquote(href, buf, sizeof(buf)));
    fd_printf(fd, "%s</A> ",
              html_strquote(text, buf, sizeof(buf)));
}


void html_email(int fd,
                const char *address)
{
    char buf[2048];

    html_strquote(address, buf, sizeof(buf));

    if (strchr(address, '<') != NULL)
        fd_printf(fd, "<A HREF=\"mailto:%s\">%s</A>\n",
                  buf, buf);
    else
        fd_printf(fd, "<A HREF=\"mailto:%s\">&lt;%s&gt;</A>\n",
                  buf, buf);
}


void html_sysheader(int fd,
                    const char *htype,
                    const char *format, ...)
{
    va_list ap;


    fd_printf(fd, "<html>\n<head>\n<title>");

    va_start(ap, format);
    fd_vprintf(fd, format, ap);
    va_end(ap);

    fd_printf(fd, "</title>\n</head>\n<body>\n");
    fd_printf(fd, "<%s ALIGN=CENTER>", htype);

    va_start(ap, format);
    fd_vprintf(fd, format, ap);
    va_end(ap);

    fd_printf(fd, "</%s>\n<hr noshade>\n", htype);
}


void html_sysfooter(int fd)
{
    fd_puts("\n<hr noshade>\n<center><address>\n", fd);

    fd_puts("This page was produced by the ", fd);
    html_href(fd, "/phttpd/", "Phttpd");
    fd_puts(" (version ", fd);
    fd_puts(server_version, fd);
    fd_puts(") server.\n", fd);

    fd_puts("</address></center>\n</body>\n</html>\n", fd);
}



void html_error(int fd,
                const char *type,
                const char *format,
                va_list ap)
{
    html_sysheader(fd, "H2", "Error: %s", type);

    fd_vprintf(fd, format, ap);
    fd_putc('\n', fd);

    if (web_admin_email || (web_admin_name && web_admin_home))
    {
        fd_puts("<P>Please contact the Webmaster for this server\n", fd);
        fd_puts("if you have any questions about this error message.\n", fd);
        fd_puts("<P>The Webmaster can be reached at: ", fd);

        if (web_admin_name)
        {
            if (web_admin_home)
                html_href(fd, web_admin_home, web_admin_name);
            else
                fd_puts(web_admin_name, fd);

            if (web_admin_email)
                fd_putc(' ', fd);
        }

        if (web_admin_email)
            html_email(fd, web_admin_email);
    }

    html_sysfooter(fd);
}


static int insert(char *buf,
                  int *pos,
                  int bufsize,
                  char *str)
{
    int len = strlen(str);


    if (*pos + len + 1 > bufsize)
        return -1;

    /* FIXME: Should add a check for buffer terminated overflow here */
    if (s_strcpy(buf+*pos, bufsize-*pos, str) < 0)
        return -1;

    *pos += len;

    return *pos;
}


char *html_strquote(const char *str,
                    char *buf,
                    int size)
{
    int i;


    for (i = 0; *str && i+1 < size; str++)
    {
        switch (*str)
        {
        case '"':
            insert(buf, &i, size, "&quot;");
            break;

        case '&':
            insert(buf, &i, size, "&amp;");
            break;

        case '<':
            insert(buf, &i, size, "&lt;");
            break;

        case '>':
            insert(buf, &i, size, "&gt;");
            break;

        default:
            buf[i++] = *str;
        }
    }

    buf[i] = '\0';
    return buf;
}

