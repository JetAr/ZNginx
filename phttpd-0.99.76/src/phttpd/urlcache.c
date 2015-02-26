/*
** urlcache.c
**
** Copyright (c) 1995 Peter Eriksson <pen@signum.se>
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

#include "phttpd.h"

int urlcache_refresh = 120;
int urlcache_ttl = 120;
int urlcache_gc_interval = 600;
int urlcache_size = 101;

static cache_t urlcache;


#define T(F)   ((flags & (F) ) && (uip->flags & (F)) == 0)


static char *get_handler(char *url)
{
    const char *handler = url_gethandler(url);

    if (handler)
        return s_strdup(handler);

    return NULL;
}

static char *get_rewrite(char *url)
{
    char buf[2048];

    if (url_getrewrite(url, buf, sizeof(buf)))
        return s_strdup(buf);

    return NULL;
}

static char *get_redirect(char *url)
{
    char buf[2048];

    if (url_getredirect(url, buf, sizeof(buf)))
    {
        return s_strdup(buf);
    }

    return NULL;
}


static char *get_predirect(char *url)
{
    char buf[2048];

    if (url_getpredirect(url, buf, sizeof(buf)))
        return s_strdup(buf);

    return NULL;
}

static char *get_access(char *url)
{
    char buf[2048];


    if (host_getaccess(url, buf, sizeof(buf)))
        return s_strdup(buf);

    return NULL;
}

static char *get_auth(char *url)
{
    char buf[2048];


    if (read_getauthenticate(url, buf, sizeof(buf)))
        return s_strdup(buf);

    return NULL;
}


static unsigned  urlinfo_update(urlinfo_t *uip,
                                unsigned int flags)
{
    if (debug > 2)
        fprintf(stderr, "urlinfo_update, url=%s\n", uip->rewrite.url);

    if (T(UCF_ACCESS))
    {
        if (debug > 2)
            fprintf(stderr, "urlinfo_update: access\n");

        uip->access.path = get_access(uip->rewrite.url);
        uip->flags |= UCF_ACCESS;
    }

    if (T(UCF_AUTH))
    {
        char *realm;

        if (debug > 2)
            fprintf(stderr, "urlinfo_update: auth\n");

        uip->auth.source = get_auth(uip->rewrite.url);

        realm = uip->auth.source;
        if (realm)
        {
            while (*realm && !s_isspace(*realm))
                ++realm;
            if (s_isspace(*realm))
                *realm++ = '\0';
            while (*realm && s_isspace(*realm))
                ++realm;
        }

        uip->auth.realm = realm;

        uip->flags |= UCF_AUTH;
    }

    if (T(UCF_REDIRECT))
    {
        if (debug > 2)
            fprintf(stderr, "urlinfo_update: redirect\n");

        uip->redirect.url = get_redirect(uip->rewrite.url);
        http_extract_request(uip->redirect.url, &uip->redirect.request);

        uip->flags |= UCF_REDIRECT;
    }

    if (T(UCF_PREDIRECT))
    {
        if (debug > 2)
            fprintf(stderr, "urlinfo_update: predirect\n");

        uip->predirect.url = get_predirect(uip->rewrite.url);
        http_extract_request(uip->predirect.url, &uip->predirect.request);

        uip->flags |= UCF_PREDIRECT;
    }

    if (T(UCF_HANDLER))
    {
        if (debug > 2)
            fprintf(stderr, "urlinfo_update: handler\n");

        uip->handler = get_handler(uip->rewrite.url);
        uip->flags |= UCF_HANDLER;
    }

    return uip->flags;
}


static urlinfo_t *urlinfo_alloc(char *url)
{
    urlinfo_t *uip;
    char *new_url;

    if (debug > 2)
        fprintf(stderr, "urlinfo_alloc(), url=%s\n", url);

    uip = s_malloc(sizeof(urlinfo_t));

    new_url = get_rewrite(url);
    if (debug > 3)
        fprintf(stderr, "urlinfo_alloc(): new_url=%s\n",
                new_url ? new_url : "<null>");

    if (new_url)
        uip->rewrite.url = new_url;
    else
        uip->rewrite.url = s_strdup(url);

    uip->rewrite.request = NULL;

#if 0 /* This code breaks URLs like: /cgi-bin/test%3Fcd?ef */
    http_extract_request(uip->rewrite.url, &uip->rewrite.request);
#endif
    return uip;
}


static int cache_update(void *key,
                        unsigned int keylen,
                        void *data,
                        void **new_data,
                        void *misc)
{
    unsigned int flags;
    char *url;
    urlinfo_t *uip;


    flags = (misc ? *(unsigned int *)misc : 0) & UCF_ALL;
    url = key;


    uip = urlinfo_alloc(url);

    urlinfo_update(uip, flags);

    *new_data = uip;
    return 1;
}


static void urlinfo_free(void *data)
{
    urlinfo_t *uip = data;


    if (uip->handler)
        s_free(uip->handler);


    if (uip->rewrite.url)
        s_free(uip->rewrite.url);

    if (uip->rewrite.request)
        s_free(uip->rewrite.request);


    if (uip->access.path)
        s_free(uip->access.path);


    if (uip->auth.source)
        s_free(uip->auth.source);

    if (uip->redirect.url)
        s_free(uip->redirect.url);

    if (uip->redirect.request)
        s_free(uip->redirect.request);

    if (uip->predirect.url)
        s_free(uip->predirect.url);

    if (uip->predirect.request)
        s_free(uip->predirect.request);

    s_free(uip);
}


void urlcache_init(void)
{
    cache_init(&urlcache,
               urlcache_refresh, urlcache_ttl, urlcache_gc_interval,
               urlcache_size, NULL,
               urlinfo_free, cache_update);
}


ucentry_t *urlcache_lookup(char *url,
                           unsigned int flags)
{
    cacheentry_t *cep;
    ucentry_t *ucp;


    if (debug > 2)
        fprintf(stderr, "urlcache_lookup(\"%s\")\n", url);

    cep = cache_lookup(&urlcache, url, 0, &flags,
                       ((flags&UCF_RELOAD) ? CF_RELOAD : 0));
    if (cep == NULL)
        return NULL;

    ucp = s_malloc(sizeof(ucentry_t));
    ucp->cep = cep;
    ucp->uip = cep->data;

    return ucp;
}


void urlcache_release(ucentry_t *ucp)
{
    if (ucp == NULL)
        return;

    if (ucp->cep)
        cache_release(ucp->cep);

    s_free(ucp);
}


int urlcache_getdata(ucentry_t *ucp,
                     unsigned int flags)
{
    int status;
    urlinfo_t *uip;


    if (debug > 1)
        fprintf(stderr, "urlcache_getdata()\n");

    uip = ucp->uip;

    mutex_lock(&uip->lock);

    status = urlinfo_update(uip, flags);

    mutex_unlock(&uip->lock);

    return status;
}

int urlcache_getstats(cachestat_t *csp)
{
    return cache_getstats(&urlcache, csp);
}
