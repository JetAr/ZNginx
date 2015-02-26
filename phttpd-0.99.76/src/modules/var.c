/*
** var.c
**
** Copyright (c) 1995 Marcus E. Hennecke <marcush@leland.stanford.edu>
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
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <math.h>
#include <alloca.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "phttpd.h"

/***********************************************************************/
/* The code below should probably go in table.c.		       */

/* Deletes and at the same time frees entries pos1 to pos2 (inclusive) */
/* of a table. */
int tbl_remove(struct table *tp, int pos1, int pos2,
               void (*val_free)(void *val))
{
    int i;

    if (pos2 < pos1)
        return tp->length;

    mutex_lock(&tp->lock);

    for (i = pos1; i <= pos2; i++)
        (*val_free)(tp->value[i]);

    for (pos2++; pos2 < tp->length; pos1++, pos2++)
        tp->value[pos1] = tp->value[pos2];

    tp->length = pos1;
    for (; pos1 < pos2; pos1++)
        tp->value[pos1] = NULL;

    mutex_unlock(&tp->lock);

    return tp->length;
}
/* The code above should probably go in table.c.		       */
/***********************************************************************/


/***********************************************************************/
/* These type definitions and the following functions should probably */
/* go in mime.h and mime.c since they are general and they could be */
/* useful to other modules. */

/* mimetype holds MIME information */
struct mimetype
{
    char *content_type;
    float q;
    float mxb;
    float version;
};

/* This is to hold all necessary information for a variant */
struct variantinfo
{
    char *url;
    struct mimetype mt;
    char *content_charset;
    char *content_language;
    char *content_encoding;
    int   content_length;
};

void mime_free(void *p)
{
    struct mimetype *mtp = (struct mimetype *)p;

    if (!mtp) return;
    if (mtp->content_type)
        s_free(mtp->content_type);
    s_free(mtp);
}

/* This is just for debugging purposes. */
void print_variant(struct variantinfo *vip)
{
    fprintf(stderr, "Variant URL: %s\n", vip->url);
    if (vip->mt.content_type)
        fprintf(stderr, "\t%s; q=%3.1f; mxb=%3.1f; version=%3.1f\n",
                vip->mt.content_type, vip->mt.q, vip->mt.mxb,
                vip->mt.version);
    if (vip->content_charset)
        fprintf(stderr, "\t%s\n", vip->content_charset);
    if (vip->content_language)
        fprintf(stderr, "\t%s\n", vip->content_language);
    if (vip->content_encoding)
        fprintf(stderr, "\t%s\n", vip->content_encoding);
    if (vip->content_length)
        fprintf(stderr, "\t%d\n", vip->content_length);
}

void variant_free(void *p)
{
    struct variantinfo *vip = (struct variantinfo *)p;

    if (!vip) return;

    if (vip->url)
        s_free(vip->url);
    if (vip->mt.content_type)
        s_free(vip->mt.content_type);
    if (vip->content_charset)
        s_free(vip->content_charset);
    if (vip->content_language)
        s_free(vip->content_language);
    if (vip->content_encoding)
        s_free(vip->content_encoding);
    s_free(vip);
}

/* Compares two mime types. Takes care of * wild cards. */
int mime_typecmp(void *p1, void *p2)
{
    char *cp1 = (char*)p1;
    char *cp2 = (char*)p2;
    char *cp;
    int len;

    if (cp1[0] == '*' || cp2[0] == '*')
        return 1;

    cp = strchr(cp1, '/');
    if (!cp)
        return 0;

    len = cp+1 - cp1;
    if (strncasecmp(cp1, cp2, len))
        return 0;
    cp1 += len;
    cp2 += len;

    if (cp1[0] == '*' || cp2[0] == '*')
        return 1;

    return (strcasecmp(cp1, cp2) == 0);
}


/* The following are all just wrapper functions to be used for the */
/* tbl_foreach function. */

/* Compare the encoding. */
static int encodingcmp(void *cp1, void *cp2)
{
    return(cp1 && cp2 && strcasecmp((char*)cp1, (char*)cp2) == 0);
}

/* Compare the languages. */
static int languagecmp(void *lang1, void *lang2)
{
    return(lang1 && lang2 && strcasecmp((char*)lang1, (char*)lang2) == 0);
}

static int check_languages(void *vip, void *al)
{
    return(tbl_foreach((struct table *)al, languagecmp,
                       ((struct variantinfo *)vip)->content_language));
}



/* Reads mime type and evaluates parameters. Fills a mimtype structure. */
void mime_gettype(char *buf, struct mimetype *mtp)
{
    char ch, *cp, *val;
    int equals;

    mtp->q = 1.0;
    mtp->mxb = 0.0;
    mtp->version = 0.0;

    for (ch = *(cp = buf);
            ch && !s_isspace(ch) && ch != ';';
            ch = *++cp )
        ;
    *cp = '\0';
    mtp->content_type = s_strdup(buf);
    if (debug > 5)
        fprintf(stderr, "\tmime_gettype: content_type %s\n", buf);

    while (ch)
    {
        for ( ; ch && (s_isspace(ch) || ch == ';'); ch = *++cp )
            ;
        if (!ch)
            break;

        buf = cp;

        for ( ; ch && !s_isspace(ch) && ch != '='; ch = *++cp )
            ;
        if (!ch)
            break;
        *cp = '\0';

        for ( equals = 0; ch && (s_isspace(ch) || ch == '='); ch = *++cp )
            if (ch == '=')
                equals = 1;
        if (!equals)
            break;

        val = cp;

        for ( ; ch && !s_isspace(ch) && ch != '='; ch = *++cp )
            ;
        *cp = '\0';

        if (s_tolower(buf[0]) == 'q' &&
                ( buf[1] == '\0' ||
                  (s_tolower(buf[1]) == 's' && buf[2] == '\0') ) )
        {
            mtp->q = atof(val);
            if (mtp->q < 0.0)
                mtp->q = 0.0;
            if (mtp->q > 1.0)
                mtp->q = 1.0;
            if (debug > 5)
                fprintf(stderr, "\tmime_gettype: q=%3.1f\n", mtp->q);
        }
        else if (s_tolower(buf[0]) == 'm' && buf[1] == 'x' &&
                 s_tolower(buf[2]) == 'b' && buf[3] == '\0')
        {
            mtp->mxb = atof(val);
            sscanf(val,"%f",&(mtp->mxb));
            if (mtp->mxb < 0.0)
                mtp->mxb = 0.0;
            if (debug > 5)
                fprintf(stderr, "\tmime_gettype: mxb=%3.1f\n", mtp->mxb);
        }
        else if (!strcasecmp(buf, "version") || !strcasecmp(buf, "level"))
        {
            mtp->version = atof(val);
            if (mtp->version < 0.0)
                mtp->version = 0.0;
            if (debug > 5)
                fprintf(stderr, "\tmime_gettype: version=%3.1f\n", mtp->version);
        }
    }

    /* Assumes that every browser understand at least 2.0 */
    if (mtp->version == 0.0 &&
            (!strcmp("text/html", mtp->content_type) ||
             !strcmp("*/*", mtp->content_type)))
        mtp->version = 2.0;
}

/* Fills in the missing file sizes in the variants table. */
static void fill_mxb(struct table *vtp, const char *base)
{
    struct variantinfo *vip;
    int i;
    struct stat sb;
    char *url, *baseend, buf[1025], path[1025];

    baseend = NULL;

    for (i = 0; i < vtp->length; i++)
    {
        vip = vtp->value[i];

        if (vip->mt.mxb <= 0.0)
        {
            url = vip->url;
            if (url[0] != '/')
            {
                if (!baseend)
                {
                    s_strcpy(buf, sizeof(buf), base);
                    baseend = strrchr(buf, '/') + 1;
                }
                s_strcpy(baseend, sizeof(buf)-(baseend-buf), url);
                url = buf;
            }

            if (url_expand(url, path, sizeof(path), &sb, NULL,
                           NULL))
                vip->mt.mxb = (float)sb.st_size;
        }
    }
}

/* Try to find the best match between a mime type and the variant */
/* entries in the variants table. Return the quality of the best */
/* match. This is used to eliminate Accept headers. */
static float mime_variant_match(struct mimetype *mip, struct table *vtp)
{
    struct variantinfo *vip;
    float q, maxq = 0.0;
    int i;

    if (debug > 7)
        fprintf(stderr,"mime_variant_match\n");
    for ( i = 0; i < vtp->length; i++ )
    {
        vip = vtp->value[i];

        if (mime_typecmp(mip->content_type, vip->mt.content_type))
        {
            q = mip->q * vip->mt.q;
            if ( q > maxq)
                maxq = q;
        }
    }

    if (debug > 7)
        fprintf(stderr,"mime_variant_match returning %3.1f\n", maxq);
    return maxq;
}

/* Convert the accept table into a table with mime type entries. This */
/* makes it easier to handle mime parameters such as quality factor, */
/* version, and mxb. */
static struct table *convert_accept(struct table *tp, struct table *vtp,
                                    int *sflag)
{
    struct table *atp;
    int i, sizeflag = 0;
    struct mimetype *mtp;

    if (debug > 6)
        fprintf(stderr, "Converting %d accept headers\n", tp->length);

    atp = tbl_create(tp->length);
    for ( i = 0; i < tp->length; i++ )
    {
        mtp = s_malloc(sizeof(struct mimetype));
        if (!mtp)
        {
            tbl_free(atp, mime_free);
            return NULL;
        }

        mime_gettype((char*)(tp->value[i]), mtp);

        if ( mime_variant_match(mtp, vtp) > 0.0 )
        {
            tbl_append(atp, mtp);
            if ( mtp->mxb > 0.0 )
                sizeflag = 1;
        }
        else
            mime_free(mtp);
    }

    if (debug > 6)
        fprintf(stderr, "Returning %d accept headers\n", atp->length);

    *sflag = sizeflag;
    return atp;
}


/* The following functions evaluate matches between the various Accept */
/* headers and the variants. */

/* Try to find the best match between a mime type and the mime type */
/* entries in the accept table. Return the quality of the best */
/* match. This is used to eliminate variants. */
static float mime_match(struct mimetype *mip1, struct table *atp)
{
    struct mimetype *mip2;
    float q, maxq = 0.0;
    int i;

    if (debug > 7)
        fprintf(stderr,"mime_match\n");

    for ( i = 0; i < atp->length; i++ )
    {
        mip2 = atp->value[i];

        if (debug > 8)
        {
            fprintf(stderr,"Comparing %s; q=%3.1f; mxb=%3.1f; version=%3.1f\n",
                    mip1->content_type, mip1->q, mip1->mxb, mip1->version);
            fprintf(stderr,"     with %s; q=%3.1f; mxb=%3.1f; version=%3.1f\n",
                    mip2->content_type, mip2->q, mip2->mxb, mip2->version);
        }
        if (mime_typecmp(mip1->content_type, mip2->content_type))
        {
            q = mip1->q * mip2->q;
            if (mip2->mxb > 0.0 && mip2->mxb < mip1->mxb)
                q = 0.0;
            if (mip2->version < mip1->version)
                q = 0.0;
            if ( q > maxq)
                maxq = q;
        }
    }

    if (debug > 7)
        fprintf(stderr,"mime_match returning %3.1f\n", maxq);
    return maxq;
}

/* Returns the index of the language (languages are listed in the */
/* order they are prefered; the first language is the favorite). */
static int find_language(char *lang, struct table *atp)
{
    int i;

    for (i = 0; i < atp->length; i++)
        if (languagecmp(lang, atp->value[i]))
            return i;

    return atp->length;
}

/* Returns the size of the variant. If necessary, fills in the */
/* information by looking up the file itself. */
static float get_mxb(struct variantinfo *vip, const char *base)
{
    struct stat sb;
    char *url, *baseend, buf[1025], path[1025];

    if (vip->mt.mxb <= 0.0)
    {
        url = vip->url;
        if (url[0] != '/')
        {
            s_strcpy(buf, sizeof(buf), base);
            baseend = strrchr(buf, '/') + 1;
            s_strcpy(baseend, sizeof(buf)-(baseend-buf), url);
            url = buf;
        }

        if (url_expand(url, path, sizeof(path), &sb, NULL,
                       NULL))
            vip->mt.mxb = (float)sb.st_size;
    }

    return vip->mt.mxb;
}


/* This is the core function for content negotiation. */
char *mime_negotiate(struct table *variants, struct mimeinfo *mip,
                     const char *base)
{
    struct variantinfo *vip;
    struct mimetype *mtp;
    int i;
    float q, maxq = 0.0;
    float v, maxv = 0.0;
    int l, minl = 10000000;
    float s, mins = 1e23;
    struct table *atp;

    /* Create empty mime info if none given. */
    if (!mip)
    {
        mip = (struct mimeinfo *) alloca(sizeof(struct mimeinfo));
        mip->version = 1;
    }

    /* Get rid of invalid encodings */
    for ( i = 0; i < variants->length; i++ )
    {
        vip = (struct variantinfo *)(variants->value[i]);
        if (vip->content_encoding)
        {
            if (!mip->accept_encoding)
            {
                variant_free(vip);
                tbl_delete(variants,i--);
            }
            else
            {
                if (!tbl_foreach(mip->accept_encoding,
                                 encodingcmp, vip->content_encoding))
                {
                    variant_free(vip);
                    tbl_delete(variants,i--);
                }
            }
        }
    }

    /* Get rid of invalid languages */
    if (mip->accept_language)
    {
        /* Before we eliminate anything, check if we can even offer */
        /* one of those languages. Otherwise, just skip this step. */
        if (tbl_foreach(variants, check_languages, mip->accept_language))
        {
            for ( i = 0; i < variants->length; i++ )
            {
                vip = (struct variantinfo *)(variants->value[i]);
                if (!check_languages(vip, mip->accept_language))
                {
                    variant_free(vip);
                    tbl_delete(variants,i--);
                }
            }
        }
    }

    /* Anything left? */
    if (!variants->length)
        return NULL;

    /* Ok, it's time to convert the accept headers into something more */
    /* useful. At the same time, eliminate those headers that we don't */
    /* need. */
    if (mip->accept)
    {
        atp = convert_accept(mip->accept, variants, &i);
        if (!atp)
            return NULL;

        if (i)
            fill_mxb(variants, base);
    }
    else
    {
        /* Come up with some defaults if no accept headers given. */
        atp = tbl_create(1);
        mtp = s_malloc(sizeof(struct mimetype));
        mtp->content_type = s_strdup("*/*");
        mtp->q = 1.0;
        mtp->mxb = 0.0;
        mtp->version = 2.0;
        tbl_append(atp, mtp);
    }

    if (!atp || atp->length == 0)
        return NULL;

    if (debug > 7)
        fprintf(stderr, "Variants: %d\n", variants->length);

    /* Now go through the variants and eliminate all except for the */
    /* best matches. */
    maxq = 0.0;
    for ( i = 0; i < variants->length; i++ )
    {
        vip = (struct variantinfo *)(variants->value[i]);
        q = mime_match(&(vip->mt), atp);

        if (q < maxq || q == 0.0)	/* Not good enough */
        {
            variant_free(vip);
            tbl_delete(variants, i--);
        }
        else if (q > maxq )			/* Best so far */
        {
            maxq = q;
            tbl_remove(variants, 0, i-1, variant_free);
            i = 0;
        }
    }

    if (debug > 7)
        fprintf(stderr, "Variants: %d\n", variants->length);

    /* More than one variant left? Ok, pick highest version. */
    if (variants->length > 1)
    {
        vip = (struct variantinfo *)(variants->value[0]);
        maxv = vip->mt.version;
        for ( i = 1; i < variants->length; i++ )
        {
            vip = (struct variantinfo *)(variants->value[i]);
            v = vip->mt.version;

            if (v < maxv)
            {
                variant_free(vip);
                tbl_delete(variants, i--);
            }
            else if (v > maxv )
            {
                maxv = v;
                tbl_remove(variants, 0, i-1, variant_free);
                i = 0;
            }
        }
    }

    if (debug > 7)
        fprintf(stderr, "Variants: %d\n", variants->length);

    /* Still more than one variant? Ok, pick favorite language. */
    if (variants->length > 1 && mip->accept_language)
    {
        vip = (struct variantinfo *)(variants->value[0]);
        minl = find_language(vip->content_language, mip->accept_language);
        for ( i = 1; i < variants->length; i++ )
        {
            vip = (struct variantinfo *)(variants->value[i]);
            l = find_language(vip->content_language, mip->accept_language);

            if (l > minl)
            {
                variant_free(vip);
                tbl_delete(variants, i--);
            }
            else if (l < minl )
            {
                minl = l;
                tbl_remove(variants, 0, i-1, variant_free);
                i = 0;
            }
        }
    }

    if (debug > 7)
        fprintf(stderr, "Variants: %d\n", variants->length);

    /* Still more than one variant? Use smallest file. */
    if (variants->length > 1)
    {
        vip = (struct variantinfo *)(variants->value[0]);
        mins = get_mxb(vip, base);
        for ( i = 1; i < variants->length; i++ )
        {
            vip = (struct variantinfo *)(variants->value[i]);
            s = get_mxb(vip, base);

            if (s > mins)
            {
                variant_free(vip);
                tbl_delete(variants, i--);
            }
            else if (s < mins )
            {
                mins = s;
                tbl_remove(variants, 0, i-1, variant_free);
                i = 0;
            }
        }
    }

    tbl_free(atp, mime_free);

    /* Anything left? */
    if (!variants->length)
        return NULL;

    vip = (struct variantinfo *)(variants->value[0]);
    return(vip->url);
}

/* The code above should probably go in mime.c			       */
/***********************************************************************/



int pm_init(const char **argv)
{
    char *name = argv[0];

    if (debug > 1)
        fprintf(stderr, "*** libvar/pm_init(\"%s\") called ***\n", name);

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** libvar/pm_exit() called ***\n");
}



static int http_get_head(struct connectioninfo *cip)
{
    struct stat sb;
    int fd_var;
    char buf[2048];
    int status, i;

    char *header;
    char *rest;
    char *cp, *url, *orig_url;
    struct table *tp = NULL;
    int redirect;

    struct httpinfo *hip = cip->hip;


    url = NULL;

    if (debug > 1)
        fprintf(stderr, "*** libvar/http_get_head() called ***\n");

    if (url_expand(hip->url, buf, sizeof(buf), &sb, NULL, NULL) == NULL)
        return -1;


    fd_var = fd_open(buf, O_RDONLY);
    if (fd_var < 0)
    {
        if (debug)
            perror("open()");
        return -1;
    }

    do
    {
        struct variantinfo *vip;

        vip = s_malloc(sizeof(struct variantinfo));
        if (vip == NULL)
            break;
        vip->mt.q = 1.0;

        do
        {
            status = mime_getline(buf, sizeof(buf), fd_var);

            if (debug > 4)
                fprintf(stderr, "libvar: Got VARFILE line: %s\n", buf);

            i = strlen(buf) - 1;
            while (i >= 0 && s_isspace(buf[i]))
                i--;
            buf[i+1] = '\0';
            for (cp = buf; *cp && s_isspace(*cp); cp++)
                ;

            if (*cp == '\0')
            {
                if (status > 0)
                    status = 0;
                break;
            }

            header = cp;
            cp = strchr(cp, ':');
            if (cp == NULL)
                continue;
            *cp = '\0';

            while ( *++cp && isspace(*cp) )
                ;
            rest = cp;

            if (!strcasecmp(header, "URI"))
            {
                if (vip->url)
                    continue;
                vip->url = s_strdup(rest);
            }

            else if (!strcasecmp(header, "CONTENT-TYPE"))
            {
                if (vip->mt.content_type)
                    continue;

                mime_gettype(rest, &(vip->mt));
            }

            else if (!strcasecmp(header, "CONTENT-CHARSET"))
            {
                if (vip->content_charset)
                    continue;
                vip->content_charset = s_strdup(rest);
            }

            else if (!strcasecmp(header, "CONTENT-LANGUAGE"))
            {
                if (vip->content_language)
                    continue;
                vip->content_language = s_strdup(rest);
            }

            else if (!strcasecmp(header, "CONTENT-ENCODING"))
            {
                if (vip->content_encoding)
                    continue;
                vip->content_encoding = s_strdup(rest);
            }

            else if (!strcasecmp(header, "CONTENT-LENGTH"))
            {
                if (vip->content_length)
                    continue;
                vip->content_length = atoi(rest);
            }

            else if (debug)
                fprintf(stderr, "Illegal header in VAR file: %s\n",
                        header);

        }
        while (status > 0);

        if (!vip->url ||
                (!vip->mt.content_type && !vip->content_language &&
                 !vip->content_encoding && !vip->content_length))
        {
            variant_free(vip);
            continue;
        }

        if (!vip->mt.content_type)
            vip->mt.content_type = s_strdup("*/*");

        if (!tp)
            tp = tbl_create(0);

        tbl_append(tp, vip);

        if (debug > 4)
        {
            print_variant(vip);
        }
    }
    while (status >= 0);

    fd_close(fd_var);

    if (!tp)
        return -1;

    url = mime_negotiate(tp, hip->mip, hip->url);

    if (!url)
    {
        status = 406;
        http_sendheaders(cip->fd, cip, status, NULL);
        fd_putc('\n', cip->fd);
        tbl_free(tp, variant_free);
        return status;
    }

    cp = strchr(url, '/');
    redirect = (cp != NULL);

    if (url[0] != '/')
    {
        s_strcpy(buf, sizeof(buf), hip->url);
        cp = strrchr(buf, '/');
        s_strcpy(cp+1, sizeof(buf)-((cp+1)-buf), buurl);
        url = buf;
    }

    if (redirect)
    {
        status = http_redirect(cip, url, hip->request, NULL, 302);
    }
    else
    {
        orig_url = hip->url;
        hip->url = url;
        status = phttpd_request(cip);
        hip->url = orig_url;
    }

    tbl_free(tp, variant_free);

    if (debug > 2)
        fprintf(stderr, "libvar/http_get_head: Returning\n");

    return status;
}

int pm_request(struct connectioninfo *cip)
{
    struct httpinfo *hip = cip->hip;


    if (strcasecmp(hip->method, "GET") == 0 ||
            strcasecmp(hip->method, "HEAD") == 0)
        return http_get_head(cip);
    else
        return -1;
}

