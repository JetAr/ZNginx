/*
** map.c
**
** Copyright (c) 1994-1997 Peter Eriksson <pen@signum.se>
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
#include <sys/types.h>
#include <sys/stat.h>

#include "phttpd.h"


static char *get_next_line(char *buf, char *last_char,
                           char *line, int line_size,
                           char *comment, int comment_size)
{
    char ch;
    char *last_line_char = line+line_size-1;
    char *last_cmt_char = comment+comment_size-1;

    if ( buf >= last_char )
        return NULL;

    /* Read up to comment or end of line */
    for ( ch = *buf;
            ch != '\n' && ch != '\r' && ch != '#'
            && line < last_line_char && buf < last_char ;
            ch = *++buf )
    {
        *line++ = ch;
    }

    /* Read up to end of line */
    for ( ;
            ch != '\n' && ch != '\r' && comment < last_cmt_char && buf < last_char ;
            ch = *++buf )
    {
        *comment++ = ch;
    }

    /* Read up to end of line */
    for ( ;
            ch != '\n' && ch != '\r' && buf < last_char ;
            ch = *++buf )
        ;

    *line = *comment = '\0';
    return(buf+1);
}

static void print_map_line(int fd, int have_xy, char *url, char *comment)
{
    int flag = 0;

    if ( have_xy )
        return;

    if ( url )
        if ( url[0] != '\0' )
        {
            fd_printf(fd, "<A HREF=\"%s\">%s</A> ", url, url);
            flag = 1;
        }

    if ( comment )
        if ( comment[0] != '\0' )
        {
            fd_puts(comment, fd);
            flag = 1;
        }

    if ( flag )
        fd_puts("<BR>\n", fd);
}

struct pair
{
    int x;
    int y;
};


static int in_cern_circle(int rq_x,
                          int rq_y,
                          struct pair *p,
                          int r)
{
    int dx, dy;


    dx = (rq_x - p->x);
    dy = (rq_y - p->y);

    return ( (dx*dx+dy*dy) <= r*r );
}

static int in_ncsa_circle(int rq_x,
                          int rq_y,
                          struct pair *p1,
                          struct pair *p2)
{
    int dx1, dy1;
    int dx2, dy2;


    dx1 = (p1->x - p2->x);
    dy1 = (p1->y - p2->y);
    dx2 = (rq_x - p1->x);
    dy2 = (rq_y - p1->y);

    return ( (dx1*dx1+dy1*dy1) >= (dx2*dx2+dy2*dy2) );
}


static int in_rectangle(int rq_x,
                        int rq_y,
                        struct pair *p1,
                        struct pair *p2)
{
    if (rq_x >= p1->x && rq_x <= p2->x &&
            rq_y >= p1->y && rq_y <= p2->y)
        return 1;
    else
        return 0;
}


static int max(int v1, int v2)
{
    return v1 > v2 ? v1 : v2;
}

static int min(int v1, int v2)
{
    return v1 < v2 ? v1 : v2;
}


static int in_polygon(int rq_x, int rq_y,
                      struct table *tp)
{
    int i, n;
    int dx, dy;


    n = 0;

    for (i = 0; i+1 < tp->length; i++)
    {
        struct pair *p0, *p1;

        p0 = tp->value[i];
        p1 = tp->value[i+1];

        dx = p1->x - p0->x;
        dy = p1->y - p0->y;
        if ( dy < 0 )
        {
            dx = -dx;
            dy = -dy;
        }

        if (max(p1->y, p0->y) > rq_y &&
                min(p1->y, p0->y) < rq_y &&
                (rq_y - p0->y) * dx > (rq_x - p0->x) * dy)
        {
            n++;
        }
    }

    return n & 1;
}


static struct options map_cfg_table[] =
{
    { NULL,                 -1,       NULL,		NULL }
};


int pm_init(const char **argv)
{
    char *cfg_path, *cp;
    const char *name = argv[0];
    int cfgsize;


    if (debug > 1)
        fprintf(stderr, "*** libmap/pm_init(\"%s\") called ***\n", name);

    cfgsize = strlen(name)+6;
    cfg_path = s_malloc(cfgsize);
    s_strcpy(cfg_path, cfgsize, name);

    cp = strrchr(cfg_path, '.');
    if (cp && strcmp(cp, ".so") == 0)
        *cp = '\0';

    s_strcat(cfg_path, cfgsize, ".conf");
    if (config_parse_file(cfg_path, map_cfg_table, 0) < 0)
        return -1;

    if (config_parse_argv(argv+1, map_cfg_table) < 0)
        return -1;

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** libmap/pm_exit() called ***\n");
}



static int get_int(char **cp, int *val)
{
    while (s_isspace(**cp))
        ++*cp;

    if (!s_isdigit(**cp))
        return 0;

    *val = 0;

    while (s_isdigit(**cp))
    {
        *val *= 10;
        *val += **cp - '0';

        ++*cp;
    }

    return 1;
}


static int get_string(char **cp, char *buf, int size)
{
    int i;

    buf[0] = '\0';
    if (cp == NULL || *cp == NULL)
        return 0;

    while (s_isspace(**cp))
        ++*cp;

    i = 0;
    while (**cp && !s_isspace(**cp) && i < size-1)
    {
        buf[i++] = **cp;
        ++*cp;
    }

    buf[i] = '\0';
    return i;
}


static int get_pair(char **cp, struct pair *pp)
{
    while (s_isspace(**cp))
        ++*cp;

    if (**cp == '(')
        ++*cp;

    if (!get_int(cp, &pp->x))
        return 0;

    while (s_isspace(**cp))
        ++*cp;

    if (**cp != ',')
        return 0;
    ++*cp;

    if (!get_int(cp, &pp->y))
        return 0;

    while (s_isspace(**cp))
        ++*cp;

    if (**cp == ')' || **cp ==',')
        ++*cp;

    return 1;
}


static int http_get_head(struct connectioninfo *cip)
{
    char default_url[2048], url_buf[2048];
    int dist, mindist, sawpoint;
    char *map_p, *map_last, comment[2048], line[2048];
    int have_label;
    int have_xy;
    int rq_x, rq_y;
    int result = 0;
    fscentry_t *fep;
    fsinfo_t *fip;

    char *statement;
    char *rest;
    char *buf_p;
    char *url;

    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;
    unsigned int fsc_flags = 0;

    url = NULL;

    if (debug > 1)
        fprintf(stderr, "*** libmap/http_get_head() called ***\n");

    if (hip && hip->mip && (hip->mip->pragma_flags & MPF_NOCACHE))
        fsc_flags = FSCF_RELOAD;

    fep = fscache_lookup(hip->url, FSCF_GETDATA+fsc_flags);
    if (fep == NULL)
        return -1;

    fip = fep->fip;
    if (fip == NULL)
    {
        fscache_release(fep);
        return -1;
    }
    map_p = fip->data.file.content;
    map_last = map_p + fip->sb.st_size;

    default_url[0] = '\0';
    mindist = 999999;
    sawpoint = 0;
    have_label = 0;
    have_xy = 0;

    if (hip->request)
    {
        if (strcmp(hip->request, "xyzzy") == 0)
        {
            rq_x = (unsigned) rand() % 500;
            rq_y = (unsigned) rand() % 500;
            have_xy = 1;
        }
        else if (sscanf(hip->request, " %d , %d", &rq_x, &rq_y) == 2)
        {
            if (debug > 4)
                fprintf(stderr, "libmap: Got request: %d , %d\n", rq_x, rq_y);

            have_xy = 1;
        }
    }
    else
    {
        if (debug > 4)
            fprintf(stderr, "libmap: Got NULL default request\n");

        have_label = 1;
    }

    if ( !have_xy )
    {
        if (debug > 4)
            fprintf(stderr, "libmap: Got improper coordinates\n");

        result = 200;

        if (hip->mip != NULL)
        {
            http_sendheaders(fd, cip, result, NULL);
            http_sendlastmodified(fd, cip->cn_time);

            fd_puts("Content-Type: text/html\n", fd);
        }

        if (hip->mip != NULL)
            fd_putc('\n', fd);

        if (strcasecmp(hip->method, "HEAD") == 0)
        {
            fscache_release(fep);
            return result;
        }

        html_sysheader(fd, "H1", "Improper map coordinates");
        fd_puts("Your browser does not appear to support image maps.\n", fd);
        fd_puts("Attempting textual display.\n<hr>\n", fd);
    }

    while ((map_p = get_next_line(map_p, map_last,
                                  line, sizeof(line),
                                  comment, sizeof(comment))))
    {
        char *cp;


        if (debug > 4)
            fprintf(stderr, "libmap: Got MAPFILE line: %s", line);

        *url_buf = '\0';

        cp = line;
        while (s_isspace(*cp))
            ++cp;

        if (*cp == '\0')
        {
            print_map_line(fd, have_xy, NULL, comment);
            continue;
        }

        if (debug > 4)
            fprintf(stderr, "Got line: %s\n", line);

        buf_p = cp;
        statement = strtok_r(cp, " \t", &buf_p);
        if (statement == NULL)
        {
            print_map_line(fd, have_xy, NULL, comment);
            continue;
        }

        rest = strtok_r(NULL, "\n\r", &buf_p);

        if (strcmp(statement, "circle") == 0 ||
                strcmp(statement, "circ") == 0)
        {
            struct pair p1, p2;
            int r;


            if (get_pair(&rest, &p1) &&
                    get_int(&rest, &r) &&
                    get_string(&rest, url_buf, sizeof(url_buf)))
            {
                if (have_xy && in_cern_circle(rq_x, rq_y, &p1, r))
                {
                    url = url_buf;
                    goto Found;
                }
            }
            else if (get_string(&rest, url_buf, sizeof(url_buf)) &&
                     get_pair(&rest, &p1) &&
                     get_pair(&rest, &p2))
            {
                if (have_xy && in_ncsa_circle(rq_x, rq_y, &p1, &p2))
                {
                    url = url_buf;
                    goto Found;
                }
            }
        }

        else if (strcmp(statement, "rectangle") == 0 ||
                 strcmp(statement, "rect") == 0)
        {
            struct pair p1, p2;

            if ((get_pair(&rest, &p1) &&
                    get_pair(&rest, &p2) &&
                    get_string(&rest, url_buf, sizeof(url_buf))) ||
                    (get_string(&rest, url_buf, sizeof(url_buf)) &&
                     get_pair(&rest, &p1) &&
                     get_pair(&rest, &p2)))
            {
                if (have_xy && in_rectangle(rq_x, rq_y, &p1, &p2))
                {
                    url = url_buf;
                    goto Found;
                }
            }
        }

        else if (strcmp(statement, "polygon") == 0 ||
                 strcmp(statement, "poly") == 0)
        {
            struct table *tp;
            int found;
            struct pair *first_p;
            struct pair *pp;


            first_p = NULL;
            pp = NULL;
            found = 0;
            url = NULL;

            tp = tbl_create(0);

            pp = s_malloc(sizeof(struct pair));

            if (get_pair(&rest, pp))	/* CERN style */
                first_p = pp;
            else			/* NCSA style */
                if (get_string(&rest, url_buf, sizeof(url_buf)) &&
                        get_pair(&rest, pp))
                {
                    first_p = pp;
                    url = url_buf;
                }

            if (first_p)
            {
                tbl_append(tp, pp);
                pp = s_malloc(sizeof(struct pair));

                while (get_pair(&rest, pp))
                {
                    tbl_append(tp, pp);
                    pp = s_malloc(sizeof(struct pair));
                }

                if (pp->x != first_p->x || pp->y != first_p->y)
                {
                    pp->x = first_p->x;
                    pp->y = first_p->y;

                    tbl_append(tp, pp);
                    pp = NULL;
                }

                if (url == NULL)
                    if (get_string(&rest, url_buf, sizeof(url_buf)))
                        url = url_buf;

                found = (have_xy && in_polygon(rq_x, rq_y, tp));
            }

            if (pp)
                s_free(pp);

            tbl_free(tp, s_free);

            if (found)
                goto Found;
        }

        else if (strcmp(statement, "point") == 0)
        {
            struct pair p;

            if ((get_pair(&rest, &p) &&
                    get_string(&rest, url_buf, sizeof(url_buf))) ||
                    (get_string(&rest, url_buf, sizeof(url_buf)) &&
                     get_pair(&rest, &p)))
            {
                if ( have_xy )
                {
                    dist = (rq_x-p.x)*(rq_x-p.x) + (rq_y-p.y)*(rq_y-p.y);
                    if ( !sawpoint || dist < mindist )
                    {
                        mindist = dist;
                        s_strcpy(default_url, sizeof(default_url), url_buf);
                    }
                    sawpoint++;
                }
            }
        }

        else if (strcmp(statement, "default") == 0 ||
                 strcmp(statement, "def") == 0)
        {
            if (!sawpoint && get_string(&rest, url_buf, sizeof(url_buf)))
                if ( have_xy )
                    s_strcpy(default_url, sizeof(default_url), url_buf);
        }

        else if (debug)
            fprintf(stderr, "Illegal statement in MAP file: %s\n",
                    statement);

        print_map_line(fd, have_xy, url_buf, comment);
    }

    if ( have_xy )
    {
        if (default_url[0] == '\0')
        {
            result = 204;

            if (hip->mip != NULL)
            {
                http_sendheaders(fd, cip, result, NULL);
                http_sendlastmodified(fd, cip->cn_time);

                fd_puts("Content-Type: text/html\n", fd);
            }

            if (strcasecmp(hip->method, "HEAD") == 0)
            {
                fscache_release(fep);
                return result;
            }

            if (hip->mip != NULL)
                fd_putc('\n', fd);

            html_sysheader(fd, "H2", "No default");
            fd_puts("No default URL was specified in the map file.\n", fd);
            html_sysfooter(fd);

            goto End;
        }
    }
    else
    {
        html_sysfooter(fd);
        goto End;
    }

    url = default_url;

Found:
    if (debug > 1)
        fprintf(stderr, "Sending Redirect: %s\n", url);

    result = http_redirect(cip, url, NULL, NULL, 302);

End:
    fscache_release(fep);

    if (debug > 2)
        fprintf(stderr, "libmap/http_get_head: Returning\n");

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
