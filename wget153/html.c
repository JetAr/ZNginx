/* A simple HTML parser.
   Copyright (C) 1995, 1996, 1997 Free Software Foundation, Inc.

This file is part of Wget.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#include "config.h"

#include <ctype.h>
#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

#include "wget.h"
#include "url.h"
#include "utils.h"
#include "ftp.h"
#include "html.h"

#ifndef errno
extern int errno;
#endif

static state_t global_state;

struct tag_attr
{
    char *tag;
    char *attr;
};


/* Match a string against a null-terminated list of identifiers.  */
//z 将tag以及attr和一个tag_attr结构数组比较；看其中是否已包含。
static int
idmatch (struct tag_attr *tags, const char *tag, const char *attr)
{
    int i;

    //z 其中之一为NULL，那么返回0
    if (!tag || !attr)
        return 0;

    //z 比较
    for (i = 0; tags[i].tag; i++)
        if (!strcasecmp (tags[i].tag, tag) && !strcasecmp (tags[i].attr, attr))
            return 1;
    return 0;
}

/*
which one is useful ?
*/
//z 解析一个BUF，查看其中是否有URLS。如果遇到了对应的tag，那么解析出URL。
/* Parse BUF (a buffer of BUFSIZE characters) searching for HTML tags
   describing URLs to follow.  When a tag is encountered, extract its
   components (as described by html_allow[] array), and return the
   address and the length of the string.  Return NULL if no URL is
   found.  */
/*History:
2015-01-29 17:28 初步看了下，没有例子，不知道实际解析时的情况，比较糊涂。最好是自己写例子测试一下的。另外看起来得了解一点html语法。

*/
const char *
htmlfindurl(const char *buf, int bufsize, int *size, int init)
{
    const char *p, *ph;
    state_t *s;

    /* NULL-terminated list of tags and modifiers someone would want to
       follow -- feel free to edit to suit your needs: */
    //z 允许的 html tags；声明为static ，只要一份即可。
    static struct tag_attr html_allow[] =
    {
        //z tag : attr 值对
        { "a", "href" },
        { "img", "src" },
        { "img", "href" },
        { "body", "background" },
        { "frame", "src" },
        { "iframe", "src" },
        { "fig", "src" },
        { "overlay", "src" },
        { "applet", "code" },
        { "script", "src" },
        { "embed", "src" },
        { "bgsound", "src" },
        { "area", "href" },
        { "img", "lowsrc" },
        { "input", "src" },
        { "layer", "src" },
        { "table", "background"},
        { "th", "background"},
        { "td", "background"},
        /* Tags below this line are treated specially.  */
        { "base", "href" },
        { "meta", "content" },
        { NULL, NULL }//z 最后以NULL作为结尾
    };

    s = &global_state;

    if (init)
    {
        DEBUGP (("Resetting a parser state.\n"));
        memset (s, 0, sizeof (*s));
    }

    while (1)
    {
        //z 如果 bufsize 为0，跳出循环
        if (!bufsize)
            break;

        /* Let's look for a tag, if we are not already in one.  */
        //z 首先寻找 tag
        if (!s->at_value)
        {
            /* Find '<'.  */
            //z 找到 <
            if (*buf != '<')
                for (; bufsize && *buf != '<'; ++buf, --bufsize);

            //z 如果 bufsize 为0 ，那么到达了结尾
            if (!bufsize)
                break;

            /* Skip spaces.  */
            //z 在处理的时候，跳过空格
            for (++buf, --bufsize; bufsize && ISSPACE (*buf) && *buf != '>';
                    ++buf, --bufsize);

            if (!bufsize)
                break;

            p = buf;

            /* Find the tag end.  */
            //z 直到找到空格或者找到>，或者找到=，或者到达结尾。
            for (; bufsize && !ISSPACE (*buf) && *buf != '>' && *buf != '=';
                    ++buf, --bufsize);

            if (!bufsize)
                break;

            //z 如果找到了 =
            if (*buf == '=')
            {
                /* <tag=something> is illegal.  Just skip it.  */
                ++buf, --bufsize;

                continue;
            }

            if (p == buf)
            {
                /* *buf == '>'.  */
                ++buf, --bufsize;

                continue;
            }

            s->tag = strdupdelim (p, buf);

            if (*buf == '>')
            {
                free (s->tag);
                s->tag = NULL;
                ++buf, --bufsize;
                continue;
            }
        }
        else                      /* s->at_value */
        {
            //z 这意思是在查找 value 。
            /* Reset AT_VALUE.  */
            s->at_value = 0;
            /* If in quotes, just skip out of them and continue living.  */
            if (s->in_quote)
            {
                s->in_quote = 0;
                for (; bufsize && *buf != s->quote_char; ++buf, --bufsize);

                if (!bufsize)
                    break;
                ++buf, --bufsize;
            }

            if (!bufsize)
                break;

            if (*buf == '>')
            {
                FREE_MAYBE (s->tag);
                FREE_MAYBE (s->attr);
                s->tag = s->attr = NULL;
                continue;
            }
        }

        /* Find the attributes.  */
        do
        {
            FREE_MAYBE (s->attr);
            s->attr = NULL;

            if (!bufsize)
                break;
            /* Skip the spaces if we have them.  We don't have them at
            places like <img alt="something"src="something-else">.
            ^ no spaces here */

            if (ISSPACE (*buf))
                for (++buf, --bufsize; bufsize && ISSPACE (*buf) && *buf != '>';
                        ++buf, --bufsize);

            if (!bufsize || *buf == '>')
                break;

            if (*buf == '=')
            {
                /* This is the case of <tag = something>, which is
                  illegal.  Just skip it.  */
                ++buf, --bufsize;
                continue;
            }

            p = buf;
            /* Find the attribute end.  */
            for (; bufsize && !ISSPACE (*buf) && *buf != '>' && *buf != '=';
                    ++buf, --bufsize);

            if (!bufsize || *buf == '>')
                break;

            //z 找到这其间的值为 attr 。
            /* Construct the attribute.  */
            s->attr = strdupdelim (p, buf);
            /* Now we must skip the spaces to find '='.  */
            if (*buf != '=')
            {
                for (; bufsize && ISSPACE (*buf) && *buf != '>'; ++buf, --bufsize);
                if (!bufsize || *buf == '>')
                    break;
            }

            /* If we still don't have '=', something is amiss.  */
            //z 是否找到了 = ，如果没有找到 = ，可能出现了错误。
            if (*buf != '=')
                continue;

            /* Find the beginning of attribute value by skipping the
            spaces.  */
            ++buf, --bufsize;
            //z 越过若干个空白字符。
            for (; bufsize && ISSPACE (*buf) && *buf != '>'; ++buf, --bufsize);
            //z 是否结束或者找到了‘>’
            if (!bufsize || *buf == '>')
                break;
            ph = NULL;
            /* The value of an attribute can, but does not have to be
            quoted.  */
            //z 如果当前字符为 ' 或者 " ， 进入引号状态
            if (*buf == '\"' || *buf == '\'')
            {
                //z 进入 quote 状态
                s->in_quote = 1;
                //z 记住当前的字符串，方便寻找到下一个做比对。
                s->quote_char = *buf;
                //z p 指向引号内第一个字符
                p = buf + 1;
                //z 步进，直到找到另一个引号，或者遇到了回车
                for (++buf, --bufsize;
                        bufsize && *buf != s->quote_char && *buf != '\n';
                        ++buf, --bufsize)

                    //z 如果当前字符串为 # ， 记录下其位置
                    if (*buf == '#')
                        ph = buf;
                if (!bufsize)
                {
                    //z 如果到达了字符结尾，结束 in_quote 状态
                    s->in_quote = 0;
                    break;
                }
                //z 如果遇到了 '\n' ，继续下一轮。
                if (*buf == '\n')
                {
                    /* #### Is the following logic good?

                     Obviously no longer in quote.  It might be well
                     to check whether '>' was encountered, but that
                     would be encouraging writers of invalid HTMLs,
                      and we don't want that, now do we?  */
                    s->in_quote = 0;
                    continue;
                }
            }
            else
            {
                p = buf;

                for (; bufsize && !ISSPACE (*buf) && *buf != '>'; ++buf, --bufsize)
                    if (*buf == '#')
                        ph = buf;
                if (!bufsize)
                    break;
            }

            //z URI 中的# （ found unprotected 是什么意思？ ） # 可能表示的意思是一个 html marker 或者 color spec 。
            /* If '#' was found unprotected in a URI, it is probably an
            HTML marker, or color spec.  */
            //z 如果有 # ，那么将 ph 视作结束？
            *size = (ph ? ph : buf) - p;
            /* The URI is liable to be returned if:
            1) *size != 0;
            2) its tag and attribute are found in html_allow.  */
            //z 实际可能表示的例子有 ： <a href="http://www.w3school.com.cn/">Visit W3School</a> 这个样子
            if (*size && idmatch (html_allow, s->tag, s->attr))
            {
                if (!strcasecmp (s->tag, "base") && !strcasecmp (s->attr, "href"))
                {
                    FREE_MAYBE (s->base);
                    s->base = strdupdelim (p, buf);
                }
                //z 比对 meta 和 content
                else if (!strcasecmp (s->tag, "meta") && !strcasecmp (s->attr, "content"))
                {
                    /* Some pages use a META tag to specify that the page
                    be refreshed by a new page after a given number of
                    seconds.  We need to attempt to extract an URL for
                    the new page from the other garbage present.  The
                    general format for this is:
                    <META HTTP-EQUIV=Refresh CONTENT="0; URL=index2.html">

                     So we just need to skip past the "0; URL="
                     garbage to get to the URL.  META tags are also
                     used for specifying random things like the page
                     author's name and what editor was used to create
                     it.  So we need to be careful to ignore them and
                      not assume that an URL will be present at all.  */
                    //z 只要是数字，那么持续向前
                    for (; *size && ISDIGIT (*p); p++, *size -= 1);

                    //z 查看是否会遇到 ;
                    if (*p == ';')
                    {
                        //z 跳过 space。
                        for (p++, *size -= 1; *size && ISSPACE (*p); p++, *size -= 1) ;
                        //z 比对，是否找到了 URL,
                        if (!strncasecmp (p, "URL=", 4))
                        {
                            //z 如果在 meta 中找到了 URL
                            p += 4, *size -= 4;
                            s->at_value = 1;
                            //z 这意思是直接返回 p？
                            return p;
                        }
                    }
                }
                else
                {
                    s->at_value = 1;
                    return p;
                }
            }

            /* Exit from quote.  */
            if (*buf == s->quote_char)
            {
                s->in_quote = 0;
                ++buf, --bufsize;
            }
        }
        while (*buf != '>');

        FREE_MAYBE (s->tag);
        FREE_MAYBE (s->attr);
        s->tag = s->attr = NULL;

        if (!bufsize)
            break;
    }

    FREE_MAYBE (s->tag);
    FREE_MAYBE (s->attr);
    FREE_MAYBE (s->base);

    memset (s, 0, sizeof (*s));	/* just to be sure */
    DEBUGP (("HTML parser ends here (state destroyed).\n"));

    return NULL;
}

/* The function returns the base reference of HTML buffer id, or NULL
   if one wasn't defined for that buffer.  */
const char *
html_base (void)
{
    return global_state.base;
}

/* The function returns the pointer to the malloc-ed quoted version of
   string s.  It will recognize and quote numeric and special graphic
   entities, as per RFC1866:

   `&' -> `&amp;'
   `<' -> `&lt;'
   `>' -> `&gt;'
   `"' -> `&quot;'

   No other entities are recognized or replaced.  */
static char *
html_quote_string (const char *s)
{
    const char *b = s;
    char *p, *res;
    int i;

    /* Pass through the string, and count the new size.  */
    //z 扫描字符串，得到替换后的字符串的长度。
    for (i = 0; *s; s++, i++)
    {
        //z 不同字符串长度有不同长度
        if (*s == '&')
            i += 4;                /* `amp;' */
        else if (*s == '<' || *s == '>')
            i += 3;                /* `lt;' and `gt;' */
        else if (*s == '\"')
            i += 5;                /* `quot;' */
    }

    //z 分配新的空间
    res = (char *)xmalloc (i + 1);
    //z 回到开头
    s = b;

    for (p = res; *s; s++)
    {
        //z 转义替换字符串
        switch (*s)
        {
        case '&':
            *p++ = '&';
            *p++ = 'a';
            *p++ = 'm';
            *p++ = 'p';
            *p++ = ';';
            break;
        case '<':
        case '>':
            *p++ = '&';
            *p++ = (*s == '<' ? 'l' : 'g');
            *p++ = 't';
            *p++ = ';';
            break;
        case '\"':
            *p++ = '&';
            *p++ = 'q';
            *p++ = 'u';
            *p++ = 'o';
            *p++ = 't';
            *p++ = ';';
            break;
        default:
            *p++ = *s;
        }
    }

    //z 设置字符串结束符
    *p = '\0';
    return res;
}

/* The function creates an HTML index containing references to given
   directories and files on the appropriate host.  The references are
   FTP.  */
uerr_t
ftp_index (const char *file, struct urlinfo *u, struct fileinfo *f)
{
    FILE *fp;
    char *upwd;
    char *htclfile;		/* HTML-clean file name */

    if (!opt.dfp)
    {
        fp = fopen (file, "wb");

        if (!fp)
        {
            logprintf (LOG_NOTQUIET, "%s: %s\n", file, strerror (errno));
            return FOPENERR;
        }
    }
    else
        fp = opt.dfp;

    if (u->user)
    {
        char *tmpu, *tmpp;        /* temporary, clean user and passwd */

        tmpu = CLEANDUP (u->user);
        tmpp = u->passwd ? CLEANDUP (u->passwd) : NULL;
        upwd = (char *)xmalloc (strlen (tmpu)
                                + (tmpp ? (1 + strlen (tmpp)) : 0) + 2);

        sprintf (upwd, "%s%s%s@", tmpu, tmpp ? ":" : "", tmpp ? tmpp : "");

        free (tmpu);
        FREE_MAYBE (tmpp);
    }
    else
        upwd = xstrdup ("");

    fprintf (fp, "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\n");
    fprintf (fp, "<html>\n<head>\n<title>");
    fprintf (fp, _("Index of /%s on %s:%d"), u->dir, u->host, u->port);
    fprintf (fp, "</title>\n</head>\n<body>\n<h1>");
    fprintf (fp, _("Index of /%s on %s:%d"), u->dir, u->host, u->port);
    fprintf (fp, "</h1>\n<hr>\n<pre>\n");

    while (f)
    {
        fprintf (fp, "  ");
        if (f->tstamp != -1)
        {
            /* #### Should we translate the months? */
            static char *months[] =
            {
                "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
            };
            struct tm *ptm = localtime ((time_t *)&f->tstamp);

            fprintf (fp, "%d %s %02d ", ptm->tm_year + 1900, months[ptm->tm_mon],
                     ptm->tm_mday);
            if (ptm->tm_hour)
                fprintf (fp, "%02d:%02d  ", ptm->tm_hour, ptm->tm_min);
            else
                fprintf (fp, "       ");
        }
        else
            fprintf (fp, _("time unknown       "));

        switch (f->type)
        {
        case FT_PLAINFILE:
            fprintf (fp, _("File        "));
            break;
        case FT_DIRECTORY:
            fprintf (fp, _("Directory   "));
            break;
        case FT_SYMLINK:
            fprintf (fp, _("Link        "));
            break;
        default:
            fprintf (fp, _("Not sure    "));
            break;
        }

        htclfile = html_quote_string (f->name);
        fprintf (fp, "<a href=\"ftp://%s%s:%hu", upwd, u->host, u->port);
        if (*u->dir != '/')
            putc ('/', fp);
        fprintf (fp, "%s", u->dir);
        if (*u->dir)
            putc ('/', fp);
        fprintf (fp, "%s", htclfile);
        if (f->type == FT_DIRECTORY)
            putc ('/', fp);
        fprintf (fp, "\">%s", htclfile);
        if (f->type == FT_DIRECTORY)
            putc ('/', fp);
        fprintf (fp, "</a> ");
        if (f->type == FT_PLAINFILE)
            fprintf (fp, _(" (%s bytes)"), legible (f->size));
        else if (f->type == FT_SYMLINK)
            fprintf (fp, "-> %s", f->linkto ? f->linkto : "(nil)");

        putc ('\n', fp);
        free (htclfile);
        f = f->next;
    }

    fprintf (fp, "</pre>\n</body>\n</html>\n");
    free (upwd);

    if (!opt.dfp)
        fclose (fp);
    else
        fflush (fp);

    return FTPOK;
}
