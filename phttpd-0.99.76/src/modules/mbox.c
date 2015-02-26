/*
** mbox.c
**
** Copyright (c) 1994-1995 Peter Eriksson <pen@signum.se>
**			   Mats Öhrman    <matoh@lysator.liu.se>
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
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "phttpd.h"

#ifndef MAP_FAILED
#define MAP_FAILED ((caddr_t) -1)
#endif

#ifndef MAP_NORESERVE
#define MAP_NORESERVE 0
#endif

#define SF_ARCHIVE

#define MAX_HDR_LINES 96

#define SEP "From "
#define SEPLEN 5		/* = strlen(SEP) */

typedef struct
{
    char *subj;
    char *from;
    char *orgz;
    char *date;
    char *id;
} header_info;



int pm_init(const char **argv)
{
    const char *name = argv[0];


    if (debug > 1)
        fprintf(stderr, "*** mbox/pm_init(\"%s\") called ***\n", name);

    return 0;
}

void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** mbox/pm_exit() called ***\n");
}

static int all_digits(char *ptr)
{
    for ( ; *ptr != '\0'; ptr++)
        if (!s_isdigit(*ptr))
            return 0;

    return -1;
}

static void fd_print_msgid_anchor(int fd_out, char *id)
{
    char *begin;
    char *end;
    int len;

    if ((begin = strchr(id, '<')) == NULL)
        begin = id;
    else
        begin++;

    if ((end = strrchr(begin, '>')) != NULL)
        len = end - begin;
    else
        len = strlen(begin);

    fd_puts("<a name=\"", fd_out);
    fd_write( fd_out, begin, len);
    fd_puts("\">\n", fd_out);
}

static void fd_print_msgid_link(int fd_out, char *url, char *id)
{
    char *begin;
    char *end;
    int len;

    if ((begin = strchr(id, '<')) == NULL)
        begin = id;
    else
        begin++;

    if ((end = strrchr(begin, '>')) != NULL)
        len = end - begin;
    else
        len = strlen(begin);

    fd_printf(fd_out, "<a href=\"%s#", url);
    fd_write( fd_out, begin, len);
    fd_puts("\">\n", fd_out);
}

static int fd_escape_write(int fd_out, char *start, off_t len)
{
    char *begin;
    char *ptr;
    char *end;
    off_t chunklen;
    int retval;

    begin = start;
    end   = start + len;

    for (ptr = start; ptr < end; ptr++)
    {
        switch (*ptr)
        {
        case '&':
            if ((chunklen = ptr - begin) > 0)
                if ((retval = fd_write(fd_out, begin, chunklen)) != 0)
                    return retval;
            begin = ptr + 1;
            fd_puts("&amp;", fd_out);
            break;
        case '<':
            if ((chunklen = ptr - begin) > 0)
                if ((retval = fd_write(fd_out, begin, chunklen)) != 0)
                    return retval;
            begin = ptr + 1;
            fd_puts("&lt;", fd_out);
            break;
        case '>':
            if ((chunklen = ptr - begin) > 0)
                if ((retval = fd_write(fd_out, begin, chunklen)) != 0)
                    return retval;
            begin = ptr + 1;
            fd_puts("&gt;", fd_out);
            break;
        }
    }
    if ((chunklen = ptr - begin) > 0)
        if ((retval = fd_write(fd_out, begin, chunklen)) != 0)
            return retval;

    return 0;
}

static int fd_escape_puts(char *str, int fd_out)
{
    off_t len;

    len = strlen(str);
    return fd_escape_write(fd_out, str, len);
}

static int letter_head_len(char *start, char *limit)
{
    char *ptr;

    for (ptr = start; ptr < limit; ptr++)
    {
        if (*ptr != '\n')
            continue;

        ptr++;

        if (ptr >= limit || *ptr == '\n')
            break;
    }

    return ptr - start;
}

static char* malloc_letter_head(char *start,   int len,
                                char *lines[], int maxlines)
{
    char *mem;
    int i;
    int nlines;

    if (len <= 0 ||
            (mem = s_malloc(len)) == NULL)
        return NULL;

    memcpy(mem, start, len);

    nlines = 0;
    if (nlines < maxlines)
        lines[nlines++] = mem;

    for (i = 0; i < len; i++)
    {
        if (mem[i] != '\n')
            continue;

        if (i+1 < len && s_isspace(mem[i+1]))
            continue;

        mem[i] = '\0';

        if (nlines < maxlines && i+1 < len)
            lines[nlines++] = mem + i + 1;

    }

    if (nlines < maxlines)
        lines[nlines++] = NULL;
    else
        lines[maxlines - 1] = NULL;

    return mem;
}

static char* find_header_val(char *key, char *lines[])
{
    int i;
    int keylen;
    char *ptr;

    keylen=strlen(key);
    for (i = 0; lines[i] != NULL; i++)
    {
        ptr = lines[i];
        if (strncasecmp(key, ptr, keylen) == 0 &&
                ptr[keylen] == ':')
        {
            ptr += keylen + 1;
            while (s_isspace(*ptr))
                ptr++;
            return ptr;
        }
    }

    return NULL;
}

static void set_header_info(header_info *hinf, char *lines[])
{
    hinf->subj = find_header_val("subject", lines);
    hinf->from = find_header_val("from", lines);
    hinf->orgz = find_header_val("organization", lines);
    hinf->date = find_header_val("date", lines);
    hinf->id   = find_header_val("message-id", lines);
}

static void fd_print_subject(int fd_out, char *subj, char *id)
{
    if (id)
        fd_print_msgid_anchor(fd_out, id);

    if (subj)
    {
        fd_puts("<strong><font size=+1>", fd_out);
        fd_escape_puts(subj, fd_out);
        fd_puts("</font></strong>\n", fd_out);
    }

    if (id)
        fd_puts("</a>\n", fd_out);
}

static char *do_letter_head(char *start, char *limit, int fd_out)
{
    char *mem;
    int len;
    char *lines[MAX_HDR_LINES];
    header_info hinf;

    fd_puts("<!-- head -->\n", fd_out);

    if (start >= limit || start == NULL)
        return NULL;

    if ((len = letter_head_len(start, limit)) <= 0 ||
            (mem = malloc_letter_head(start, len, lines, MAX_HDR_LINES)) == NULL)
        return NULL;

    set_header_info(&hinf, lines);

    fd_puts("<dt>\n", fd_out);

    fd_print_subject(fd_out, hinf.subj, hinf.id);
    fd_puts("<br>", fd_out);

    html_email(fd_out, hinf.from);
    fd_puts("<br>", fd_out);

    if (hinf.orgz)
    {
        fd_escape_puts(hinf.orgz, fd_out);
        fd_puts("<br>", fd_out);
    }

    if (hinf.date)
    {
        fd_escape_puts(hinf.date, fd_out);
        fd_puts("<br>", fd_out);
    }

    s_free(mem);
    return start + len;
}

static char *do_index_entry(char *start, char *limit,
                            int fd_out, int num, char *url)
{
    char *mem;
    int len;
    char *lines[MAX_HDR_LINES];
    header_info hinf;

    fd_puts("<!-- index entry -->\n", fd_out);

    if (start >= limit || start == NULL)
        return NULL;

    if ((len = letter_head_len(start, limit)) <= 0 ||
            (mem = malloc_letter_head(start, len, lines, MAX_HDR_LINES)) == NULL)
        return NULL;

    set_header_info(&hinf, lines);

    fd_puts("<dt>\n", fd_out);
    fd_printf(fd_out, "<a href=\"%s?%d\">\n", url, num);
    fd_printf(fd_out, "<strong><font size=+1>%d</font></strong>", num);
    fd_puts("</a>\n", fd_out);
    fd_puts("<dd>\n", fd_out);

    if (hinf.subj)
    {
        fd_puts("<strong><font size=+1>", fd_out);
        fd_escape_puts(hinf.subj, fd_out);
        fd_puts("</font></strong><br>\n", fd_out);
    }
    if (hinf.from)
    {
#if 0
        fd_escape_puts(hinf.from, fd_out);
#else
        html_email(fd_out, hinf.from);
#endif
        fd_puts("<br>\n", fd_out);
    }
    if (hinf.date)
    {
        fd_escape_puts(hinf.date, fd_out);
        fd_puts("<br>\n", fd_out);
    }

    s_free(mem);
    return start + len;
}

static char *do_rindex_entry(char *start, char *limit,
                             int fd_out, int num, char *url)
{
    char *mem;
    int len;
    char *lines[MAX_HDR_LINES];
    header_info hinf;

    fd_puts("<!-- index entry -->\n", fd_out);

    if (start >= limit || start == NULL)
        return NULL;

    if ((len = letter_head_len(start, limit)) <= 0 ||
            (mem = malloc_letter_head(start, len, lines, MAX_HDR_LINES)) == NULL)
        return NULL;

    set_header_info(&hinf, lines);

    fd_puts("<dt>\n", fd_out);
    fd_printf(fd_out, "<a href=\"%s?r%d\">\n", url, num);
    fd_printf(fd_out, "<strong><font size=+1>%d</font></strong>", num);
    fd_puts("</a>\n", fd_out);
    fd_puts("<dd>\n", fd_out);
    if (hinf.subj)
    {
        fd_puts("<strong><font size=+1>", fd_out);
        fd_escape_puts(hinf.subj, fd_out);
        fd_puts("</font></strong><br>\n", fd_out);
    }
    if (hinf.from)
    {
#if 0
        fd_escape_puts(hinf.from, fd_out);
#else
        html_email(fd_out, hinf.from);
#endif
        fd_puts("<br>\n", fd_out);
    }
    if (hinf.date)
    {
        fd_escape_puts(hinf.date, fd_out);
        fd_puts("<br>\n", fd_out);
    }

    s_free(mem);
    return start + len;
}

static char *do_id_index_entry(char *start, char *limit,
                               int fd_out, int num, char *url)
{
    char *mem;
    int len;
    char *lines[MAX_HDR_LINES];
    header_info hinf;

    fd_puts("<!-- index entry -->\n", fd_out);

    if (start >= limit || start == NULL)
        return NULL;

    if ((len = letter_head_len(start, limit)) <= 0 ||
            (mem = malloc_letter_head(start, len, lines, MAX_HDR_LINES)) == NULL)
        return NULL;

    set_header_info(&hinf, lines);

    fd_puts("<dt>\n", fd_out);
    fd_printf(fd_out, "<strong><font size=+1>%d</font></strong>\n", num);
    fd_puts("<dd>\n", fd_out);
    if (hinf.id)   fd_print_msgid_link(fd_out, url, hinf.id);
    if (hinf.subj)
    {
        fd_puts("<strong><font size=+1>", fd_out);
        fd_escape_puts(hinf.subj, fd_out);
        fd_puts("</font></strong><br>\n", fd_out);
    }
    if (hinf.id)   fd_puts("</a>\n", fd_out);
    if (hinf.from)
    {
        fd_escape_puts(hinf.from, fd_out);
        fd_puts("<br>\n", fd_out);
    }
    if (hinf.date)
    {
        fd_escape_puts(hinf.date, fd_out);
        fd_puts("<br>\n", fd_out);
    }

    s_free(mem);
    return start + len;
}

static char *do_letter_body(char *start, char *limit, int fd_out)
{
    char *ptr;
    char *end;

    if (start >= limit || start == NULL)
        return NULL;

    end = limit - strlen("\nFrom ");

    for (ptr = start; ptr < end; ptr++)
    {
        if (*ptr == '\n')
        {

            if (strncmp(ptr+1, SEP, SEPLEN) == 0)
            {
                limit = ptr+1;
                break;
            }
        }
    }

    fd_puts("<dd>\n", fd_out);
    fd_puts("<pre><!-- body -->\n", fd_out);

    if (fd_escape_write(fd_out, start, limit - start) < 0)
        if (debug)
        {
            fprintf(stderr, "ERROR:");
            perror("write");
        }
    fd_puts("</pre>\n", fd_out);

    return limit;
}

char *skip_letter_forward(int n, char *start, char *limit)
{
    char *ptr;
    char *end;

    if (start >= limit || start == NULL)
        return NULL;

    if (n < 1)
        return start;

    end   = limit - SEPLEN - 1;
    for (ptr = start; ptr < end; ptr++)
    {
        if (*ptr == '\n')
        {
            if (strncmp(ptr+1, SEP, SEPLEN) == 0 &&
                    --n <= 0)
            {
                return ptr+1;
            }
        }
    }

    return limit;
}

char *skip_letter_backward(int n, char *start, char *lower_limit)
{
    char *ptr;
    char *begin;



    if (start == NULL || (begin = start - SEPLEN) < lower_limit)
        return NULL;

    if (n < 1)
        return start;

    for (ptr = begin; ptr >= lower_limit; ptr--)
    {
        if (*ptr == '\n')
        {
            if (strncmp(ptr+1, SEP, SEPLEN) == 0 &&
                    --n <= 0)
            {
                return ptr+1;
            }
        }
    }

    if (strncmp(lower_limit, SEP, SEPLEN) == 0)
        return lower_limit;

    return NULL;
}


static char *mmap_mbox(int fd_in, off_t size)
{
    char *cp;

    cp = mmap((caddr_t) NULL,
              size,
              PROT_READ,
              MAP_PRIVATE+MAP_NORESERVE,
              fd_in, 0);

    if (cp == (char *) MAP_FAILED)
    {
        if (debug)
        {
            fprintf(stderr, "ERROR: ");
            perror("mmap");
        }
    }
    else
    {
        madvise(cp, size, MADV_NORMAL);
        madvise(cp, size, MADV_WILLNEED);
    }
    return cp;
}


static void do_letter(int fd_in, int fd_out, off_t size)
{
    char *cp;
    char *ptr;

    if ((cp = mmap_mbox(fd_in, size)) != NULL)
    {
        for (ptr = cp; ptr != NULL;)
        {
            ptr = do_letter_head(ptr, cp+size, fd_out);
            ptr = do_letter_body(ptr, cp+size, fd_out);
        }
        munmap(cp, size);
    }
}

static void do_num_letter(int fd_in, int fd_out, off_t size, int n)
{
    char *cp;
    char *ptr;

    if ((cp = mmap_mbox(fd_in, size)) != NULL)
    {
        ptr = skip_letter_forward(n-1, cp, cp+size);
        ptr = do_letter_head(ptr, cp+size, fd_out);
        ptr = do_letter_body(ptr, cp+size, fd_out);

        munmap(cp, size);
    }
}

static void do_rnum_letter(int fd_in, int fd_out, off_t size, int n)
{
    char *cp;
    char *ptr;

    if ((cp = mmap_mbox(fd_in, size)) != NULL)
    {
        ptr = skip_letter_backward(n, cp+size, cp);
        ptr = do_letter_head(ptr, cp+size, fd_out);
        ptr = do_letter_body(ptr, cp+size, fd_out);

        munmap(cp, size);
    }
}

static void do_index_entries(int fd_in, int fd_out, off_t size, char *url)
{
    char *cp;
    char *ptr;
    int count;

    if ((cp = mmap_mbox(fd_in, size)) != NULL)
    {
        count = 1;
        for (ptr = cp; ptr != NULL;)
        {
            ptr = do_index_entry(ptr, cp+size, fd_out, count++, url);
            ptr = skip_letter_forward(1, ptr, cp+size);
        }
        munmap(cp, size);
    }
}

static void do_rindex_entries(int fd_in, int fd_out, off_t size, char *url)
{
    char *cp;
    char *ptr;
    int count;

    if ((cp = mmap_mbox(fd_in, size)) != NULL)
    {
        count = 1;
        for (ptr = cp + size - 1; ptr != NULL;)
        {
            ptr = skip_letter_backward(1, ptr, cp);
            do_rindex_entry(ptr, cp+size, fd_out, count++, url);
        }
        munmap(cp, size);
    }
}

static void do_id_index_entries(int fd_in, int fd_out, off_t size, char *url)
{
    char *cp;
    char *ptr;
    int count;

    if ((cp = mmap_mbox(fd_in, size)) != NULL)
    {
        count = 1;
        for (ptr = cp; ptr != NULL;)
        {
            ptr = do_id_index_entry(ptr, cp+size, fd_out, count++, url);
            ptr = skip_letter_forward(1, ptr, cp+size);
        }
        munmap(cp, size);
    }
}

static void do_id_rindex_entries(int fd_in, int fd_out, off_t size, char *url)
{
    char *cp;
    char *ptr;
    int count;

    if ((cp = mmap_mbox(fd_in, size)) != NULL)
    {
        count = 1;
        for (ptr = cp + size - 1; ptr != NULL;)
        {
            ptr = skip_letter_backward(1, ptr, cp);
            do_id_index_entry(ptr, cp+size, fd_out, count++, url);
        }
        munmap(cp, size);
    }
}

#ifdef SF_ARCHIVE

static void sf_fix_title(char *title, char *buf, int buflen)
{
    char *begin;
    char *end;
    char *src;
    char *dst;
    char *dstend;

    if ((begin = strrchr(title, '/')) == NULL)
        begin = title;
    else
        begin++;

    if ((end = strrchr(begin, '.')) == NULL ||
            strcmp(end, ".mbox") != 0)
        end = begin + strlen(begin);

    dst = buf;
    dstend = buf + buflen;
    for (src = begin; src < end; src++)
    {

        switch (*src)
        {

        case '\0':
            *dst = '\0';
            return;

        case ',':
            if (dst + 2 > dstend)
            {
                *dst = '\0';
                return;
            }
            *dst++ = ',';
            *dst++ = ' ';
            break;

        case '_':
            *dst++ = ' ';
            break;

        default:
            *dst++ = *src;
            break;
        }

        if (dst+1 >= dstend)
        {
            *dst = '\0';
            return;
        }
    }
}

static void sf_do_file_head(int fd_out, char *title)
{
    fd_printf(fd_out, "<html>\n");
    fd_printf(fd_out, "<!-- SF archive mbox converter -->\n");
    fd_printf(fd_out, "<head>\n");
    fd_printf(fd_out, "<title>Linköping Science Fiction Archive:%s</title>\n", title);
    fd_printf(fd_out, "<link rev=\"made\" href=\"mailto:matoh@lysator.liu.se\">\n");
    fd_printf(fd_out, "</head>\n");
}

static void sf_do_file_body(int fd_in, int fd_out, off_t size, char *title)
{
    fd_printf(fd_out, "<body background=\"/sf_archive/background.gif\">\n");
    fd_printf(fd_out, "<dl>\n");
    fd_printf(fd_out, "<dt>\n");
    fd_printf(fd_out, "<a href=\"/sf_archive/sf_main.html\">\n");
    fd_printf(fd_out, "<img align=top src=\"/sf_archive/small_logo.gif\" alt=\"[Logotype]\" border=0></a>\n");
    fd_printf(fd_out, "<cite>The Linköping Science Fiction & Fantasy Archive</cite>\n");
    fd_printf(fd_out, "<dd>\n");
    fd_printf(fd_out, "<h1>%s</h1>\n", title);
    fd_printf(fd_out, "<hr noshade>\n");
    fd_printf(fd_out, "<dl>\n");

    do_letter(fd_in, fd_out, size);

    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "<hr noshade>\n");
    fd_printf(fd_out, "<address>\n");
    fd_printf(fd_out, "<a href=\"/sf_archive/sf_main.html\">Linköping SF Archive</a> /\n");
    fd_printf(fd_out, "<a href=\"/sf_archive/matoh.html\">Mats Öhrman</a>\n");
    fd_printf(fd_out, "</address>\n");
    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "</body>\n");
    fd_printf(fd_out, "</html>\n");
}

static void sf_do_all(int fd_in, int fd_out, char *url, off_t size)
{
    char title[256];

    sf_fix_title(url, title, sizeof(title));
    sf_do_file_head(fd_out, title);
    sf_do_file_body(fd_in, fd_out, size, title);
}

#endif


static void fix_title(char *title, char *buf, int buflen)
{
    char *begin;
    char *end;
    int len;
    int i;

    if ((begin = strrchr(title, '/')) == NULL)
        begin = title;
    else
        begin++;

    if ((end = strrchr(begin, '.')) == NULL ||
            strcmp(end, ".mbox") != 0)
        len = strlen(begin);
    else
        len = end - begin;

    if (len > buflen - 1)
        len = buflen - 1;

    for (i = 0; i < len; i++)
    {
        buf[i] = begin[i];
    }
    buf[i] = '\0';
}

static void do_file_head(int fd_out, char *title, char *url)
{
    fd_puts("<html>\n",    fd_out);
    fd_puts("<head>\n",    fd_out);
#if 0
    fd_printf(fd_out, "<base href=\"%s%s\">\n", server_url, url);
#endif
    fd_puts("<title>",     fd_out);
    fd_escape_puts(title, fd_out);
    fd_puts("</title>\n",  fd_out);
    fd_puts("</head>\n",   fd_out);
}

static void do_file_body(int fd_in, int fd_out, off_t size, char *title)
{
    fd_printf(fd_out, "<body>\n");
    fd_printf(fd_out, "<h1>");
    fd_escape_puts(title, fd_out);
    fd_printf(fd_out, "</h1>\n");
    fd_printf(fd_out, "<dl>\n");

    do_letter(fd_in, fd_out, size);

    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "</body>\n");
    fd_printf(fd_out, "</html>\n");
}

static void do_num_file_body(int fd_in, int fd_out, off_t size,
                             int n, char *title)
{
    fd_printf(fd_out, "<body>\n");
    fd_printf(fd_out, "<h1>");
    fd_escape_puts(title, fd_out);
    fd_printf(fd_out, ": %d</h1>\n", n);
    fd_printf(fd_out, "<dl>\n");

    do_num_letter(fd_in, fd_out, size, n);

    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "</body>\n");
    fd_printf(fd_out, "</html>\n");
}

static void do_rnum_file_body(int fd_in, int fd_out, off_t size,
                              int n, char *title)
{
    fd_printf(fd_out, "<body>\n");
    fd_printf(fd_out, "<h1>");
    fd_escape_puts(title, fd_out);
    fd_printf(fd_out, ": %d</h1>\n", n);
    fd_printf(fd_out, "<dl>\n");

    do_rnum_letter(fd_in, fd_out, size, n);

    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "</body>\n");
    fd_printf(fd_out, "</html>\n");
}

static void do_index_body(int fd_in, int fd_out, off_t size, char *title, char *url)
{
    fd_printf(fd_out, "<body>\n");
    fd_printf(fd_out, "<h1><a href=\"%s\">", url);
    fd_escape_puts(title, fd_out);
    fd_printf(fd_out, "</a>");
    fd_printf(fd_out, ": Index</h1>\n");
    fd_printf(fd_out, "<dl compact>\n");

    do_index_entries(fd_in, fd_out, size, url);

    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "</body>\n");
    fd_printf(fd_out, "</html>\n");
}

static void do_rindex_body(int fd_in, int fd_out, off_t size, char *title, char *url)
{
    fd_printf(fd_out, "<body>\n");
    fd_printf(fd_out, "<h1><a href=\"%s\">", url);
    fd_escape_puts(title, fd_out);
    fd_printf(fd_out, "</a>");
    fd_printf(fd_out, ": Reverse Index</h1>\n");
    fd_printf(fd_out, "<dl compact>\n");

    do_rindex_entries(fd_in, fd_out, size, url);

    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "</body>\n");
    fd_printf(fd_out, "</html>\n");
}

static void do_id_index_body(int fd_in, int fd_out, off_t size, char *title, char *url)
{
    fd_printf(fd_out, "<body>\n");
    fd_printf(fd_out, "<h1><a href=\"%s\">", url);
    fd_escape_puts(title, fd_out);
    fd_printf(fd_out, "</a>");
    fd_printf(fd_out, ": Index</h1>\n");
    fd_printf(fd_out, "<dl compact>\n");

    do_id_index_entries(fd_in, fd_out, size, url);

    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "</body>\n");
    fd_printf(fd_out, "</html>\n");
}

static void do_id_rindex_body(int fd_in, int fd_out, off_t size, char *title, char *url)
{
    fd_printf(fd_out, "<body>\n");
    fd_printf(fd_out, "<h1><a href=\"%s\">", url);
    fd_escape_puts(title, fd_out);
    fd_printf(fd_out, "</a>");
    fd_printf(fd_out, ": Index</h1>\n");
    fd_printf(fd_out, "<dl compact>\n");

    do_id_rindex_entries(fd_in, fd_out, size, url);

    fd_printf(fd_out, "</dl>\n");
    fd_printf(fd_out, "</body>\n");
    fd_printf(fd_out, "</html>\n");
}

static void do_all(int fd_in, int fd_out, char *url, off_t size)
{
    char title[256];

    fix_title(url, title, sizeof(title));
    do_file_head(fd_out, title, url);
    do_file_body(fd_in, fd_out, size, title);
}

static void do_index(int fd_in, int fd_out, char *url, off_t size)
{
    char title[256];

    fix_title(url, title, sizeof(title));
    do_file_head(fd_out, title, url);
    do_index_body(fd_in, fd_out, size, title, url);
}

static void do_rindex(int fd_in, int fd_out, char *url, off_t size)
{
    char title[256];

    fix_title(url, title, sizeof(title));
    do_file_head(fd_out, title, url);
    do_rindex_body(fd_in, fd_out, size, title, url);
}

static void do_id_index(int fd_in, int fd_out, char *url, off_t size)
{
    char title[256];

    fix_title(url, title, sizeof(title));
    do_file_head(fd_out, title, url);
    do_id_index_body(fd_in, fd_out, size, title, url);
}

static void do_id_rindex(int fd_in, int fd_out, char *url, off_t size)
{
    char title[256];

    fix_title(url, title, sizeof(title));
    do_file_head(fd_out, title, url);
    do_id_rindex_body(fd_in, fd_out, size, title, url);
}

static void do_num_search(int fd_in, int fd_out,
                          char *num, char *url, off_t size)
{
    char title[256];
    int n;

    n = atoi(num);

    fix_title(url, title, sizeof(title));
    do_file_head(fd_out, title, url);
    do_num_file_body(fd_in, fd_out, size, n, title);
}

static void do_rnum_search(int fd_in, int fd_out,
                           char *num, char *url, off_t size)
{
    char title[256];
    int n;

    n = atoi(num);

    fix_title(url, title, sizeof(title));
    do_file_head(fd_out, title, url);
    do_rnum_file_body(fd_in, fd_out, size, n, title);
}

static int http_get_head(struct connectioninfo *cip)
{
    char buf[256], *s_since;
    char path[1025];
    struct stat sb;
    int fd_in, i, status, result;
    int fd_out = cip->fd;
    struct httpinfo *hip = cip->hip;


    if (debug > 1)
        fprintf(stderr, "*** mbox/pm_get() called ***\n");

    if (url_expand(hip->url, path, sizeof(path), &sb, NULL, NULL) == NULL)
        return -1;

    if (hip->mip &&
            (s_since = mime_getheader(hip->mip, "IF-MODIFIED-SINCE", 1)))
    {
        struct tm tm_since;
        int t_since;


        t_since = atotm(s_since, &tm_since);
        if (t_since != -1)
        {
            if (sb.st_mtime <= (time_t) t_since)
                return http_not_modified(cip);
        }
        else
        {
            /* Could not parse the date format - do a string compare */
            http_time_r(&sb.st_mtime, buf, sizeof(buf));
            i = strlen(buf);
            buf[i-1] = '\0';

            if (strcmp(s_since, buf) == 0)
                return http_not_modified(cip);
        }
    }

    fd_in = s_open(path, O_RDONLY);
    if (fd_in < 0)
    {
        if (debug)
            perror("open(path)");

        return -1;
    }

    result = 200;

    if (hip->mip != NULL)
    {
        http_sendheaders(fd_out, cip, result, NULL);
        http_sendlastmodified(fd_out, sb.st_mtime);

        fd_puts("Content-Type: text/html\n\n", fd_out);
    }
#if 0
    hip->length = sb.st_size;
#endif

    if (hip->mip != NULL)
        fd_putc('\n', fd_out);

    if (strcasecmp(hip->method, "HEAD") == 0)
    {
        s_close(fd_in);
        return result;
    }

#ifdef SF_ARCHIVE
    if (strncmp(hip->url, "/sf_archive/", strlen("/sf_archive/")) == 0)
    {
        sf_do_all(fd_in, fd_out, hip->url, sb.st_size);
    }
    else
    {
        if (hip->request == NULL)
        {
#if 0
            do_all(fd_in, fd_out, hip->url, sb.st_size);
#else
            do_rindex(fd_in, fd_out, hip->url, sb.st_size);
#endif
        }
        else if (strcasecmp(hip->request, "all") == 0)
        {
            do_all(fd_in, fd_out, hip->url, sb.st_size);
        }
        else if (strcasecmp(hip->request, "index") == 0)
        {
            do_index(fd_in, fd_out, hip->url, sb.st_size);
        }
        else if (strcasecmp(hip->request, "rindex") == 0)
        {
            do_rindex(fd_in, fd_out, hip->url, sb.st_size);
        }
        else if (strcasecmp(hip->request, "id_index") == 0)
        {
            do_id_index(fd_in, fd_out, hip->url, sb.st_size);
        }
        else if (strcasecmp(hip->request, "id_rindex") == 0)
        {
            do_id_rindex(fd_in, fd_out, hip->url, sb.st_size);
        }
        else if (all_digits(hip->request))
        {
            do_num_search(fd_in, fd_out, hip->request, hip->url, sb.st_size);
        }
        else if ((hip->request[0] == 'r' || hip->request[0] == '-' ) &&
                 all_digits(hip->request + 1))
        {
            do_rnum_search(fd_in, fd_out, hip->request+1, hip->url, sb.st_size);
        }
        else
        {
#if 0
            do_all(fd_in, fd_out, hip->url, sb.st_size);
#else
            do_rindex(fd_in, fd_out, hip->url, sb.st_size);
#endif
        }
    }
#else
    do_all(fd_in, fd_out, hip->url, sb.st_size);
#endif

    status = s_close(fd_in);
    if (status < 0)
        if (debug)
        {
            fprintf(stderr, "ERROR on #%d: ", cip->request_no);
            perror("close");
        }

    if (debug > 2)
        fprintf(stderr, "modules/mbox/http_get: Returning\n");

    return result;
}


int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;

    if (strcasecmp(hip->method, "GET") == 0 ||
            strcasecmp(hip->method, "HEAD") == 0)
        return http_get_head(cip);
    else
        return -2;
}
