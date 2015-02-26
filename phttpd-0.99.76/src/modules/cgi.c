/*
** cgi.c
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
#include <alloca.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "phttpd.h"

#define MAXENV	256

static char *cgi_user = NULL;
static char *cgi_group = NULL;
static char *cgi_path = NULL;
static char *copy_environ = NULL;

static char *cgi_newroot = NULL;
static rlim_t cgi_niceval = 5;
static rlim_t cgi_maxcputime = 0;
static rlim_t cgi_vmem = 0;
static rlim_t cgi_fdmax = 0;

static uid_t cgi_uid = 60001; /* Nobody */
static gid_t cgi_gid = 60001; /* Nobody */
static int user_cgi = 1;
static int run_as_authuser = 0;
static int max_environ = MAXENV;
static int max_processes = 0;

static sema_t proc_sema;



static char *cgi_setenv(char **envp, const char *var, const char *val)
{
    char *buf;
    int i, len, blen;


    if (debug > 5)
        fprintf(stderr, "cgi_setenv: var=%s, val=%s\n",
                var, val ? val : "<null>");

    if (val == NULL)
        return NULL;

    blen = strlen(var)+strlen(val)+2;
    buf = s_malloc(blen);

    s_strcpy(buf, blen, var);
    len = s_strcat(buf, blen, "=");
    s_strcat(buf, blen, val);

    for (i = 0; i < max_environ && envp[i]; i++)
        if (strncmp(envp[i], buf, len) == 0)
            break;

    if (i >= max_environ)
        return NULL;

    if (envp[i])
        s_free(envp[i]);

    envp[i] = buf;
    return buf;
}


static char *cgi_setenv_i(char **envp, const char *var, int num)
{
    char buf[256];

    s_sprintf(buf, sizeof(buf), "%d", num);
    return cgi_setenv(envp, var, buf);
}


static int cse_header(hashentry_t *hep, void *envp)
{
    char *name, *cp;
    int namsize;


    if (strcmp(hep->key, "AUTHORIZATION") == 0)
        return 0;

    namsize = strlen(hep->key)+6;
    name = (char *) alloca(namsize);
    s_strcpy(name, namsize, "HTTP_");
    s_strcat(name, namsize, hep->key);
    cp = name+5;
    while (*cp)
    {
        if (*cp == '-')
            *cp = '_';
        ++cp;
    }

    cp = cgi_setenv(envp, name, hep->data);

    return 0;
}


static char **cgi_setup_env(const char *method,
                            char *url,
                            struct connectioninfo *cip,
                            struct httpinfo *hip,
                            struct mimeinfo *mip,
                            const char *path,
                            char *path_info,
                            char *path_translated)
{
    char **envp, *buf, *request;
    int blen;


    envp = s_malloc((max_environ+1) * sizeof(char *));
    if (envp == NULL)
        return NULL;

    cgi_setenv(envp, "DOCUMENT_ROOT", (cgi_newroot==NULL) ? server_home : "/");
    cgi_setenv(envp, "TZ", tzname[0]);
    cgi_setenv(envp, "PATH", path);

    blen = strlen(server_version) + 8;
    buf = (char *) alloca(blen);
    s_sprintf(buf, blen, "phttpd/%s", server_version);

    cgi_setenv(envp, "SERVER_SOFTWARE", buf);
    cgi_setenv(envp, "SERVER_NAME", server_host);
    cgi_setenv(envp, "GATEWAY_INTERFACE", "CGI/1.1");
    cgi_setenv(envp, "REQUEST_METHOD", method);
    cgi_setenv(envp, "PATH_INFO", path_info);

    cgi_setenv_i(envp, "SERVER_PORT", server_port);

    if (path_translated && path_translated[0])
    {
        if (path_translated[0] != '/')
        {
            int blen = strlen(server_home) +
                       strlen(path_translated) + 2;

            buf = (char *) alloca(blen);
            s_strcpy(buf, blen, server_home);
            if (strcmp(path_translated, ".") != 0)
            {
                s_strcat(buf, blen, "/");
                s_strcat(buf, blen, path_translated);
            }

            cgi_setenv(envp, "PATH_TRANSLATED", buf);
        }
        else
            cgi_setenv(envp, "PATH_TRANSLATED", path_translated);
    }

    if (cip)
    {
        cgi_setenv(envp, "LOCAL_ADDR",  cip->server.addr_s);
        cgi_setenv(envp, "LOCAL_HOST", get_hostname(&cip->server));

        cgi_setenv(envp, "REMOTE_ADDR", cip->client.addr_s);
        cgi_setenv(envp, "REMOTE_HOST", get_hostname(&cip->client));
        cgi_setenv(envp, "REMOTE_IDENT", get_ident(&cip->client));

        cgi_setenv(envp, "AUTH_TYPE", cip->auth.type);
        cgi_setenv(envp, "REMOTE_USER", cip->auth.user);
        if (cip->auth.xsetenv)
            (*cip->auth.xsetenv)(cip->auth.xinfo, cgi_setenv, envp);
    }

    if (hip)
    {
        cgi_setenv(envp, "SERVER_PROTOCOL", hip->version);
        cgi_setenv(envp, "SCRIPT_NAME", url+cip->vspl);
        cgi_setenv(envp, "QUERY_STRING", hip->request);
    }

    if (mip)
    {
        cgi_setenv(envp, "CONTENT_TYPE",
                   mime_getheader(mip, "CONTENT-TYPE", 1));

        cgi_setenv(envp, "CONTENT_LENGTH",
                   mime_getheader(mip, "CONTENT-LENGTH", 1));

        ht_foreach(&mip->table, cse_header, (void *) envp);
    }



    cgi_setenv(envp, "PHTTPD_ORIG_URL", hip->orig_url+cip->vspl);
    cgi_setenv(envp, "PHTTPD_ORIG_REQUEST", hip->orig_request);

    /* Non CGI/1.1 conformant extension, but very nice to have... */
    if (hip->request && hip->request[0])
    {
        char **argv;
        int rlen = strlen(hip->request)+1;

        request = (char *) alloca(rlen);
        s_strcpy(request, rlen, hip->request);

        if ((argv = strsplit(request, '&', 0)) != NULL)
        {
            int i;

            for (i = 0; argv[i]; i++)
            {
                char **varval, *var_name;


                if ((varval = strsplit(argv[i], '=', 2)) != NULL)
                {
                    if (debug > 4)
                        fprintf(stderr, "varval[0] = %s, varval[1] = %s\n",
                                varval[0] ? varval[0] : "[null]",
                                varval[1] ? varval[1] : "[null]");

                    if (varval[0] && varval[1])
                    {
                        int vlen = strlen(varval[0])+14;

                        var_name = (char *) alloca(vlen);
                        s_strcpy(var_name, vlen, "PHTTPD_QUERY_");
                        s_strcat(var_name, vlen, varval[0]);

                        cgi_setenv(envp, var_name, varval[1]);
                    }

                    s_free(varval);
                }
            }

            s_free(argv);
        }
    }

    if (copy_environ)
    {
        char *var, *cp, *cea, *val;
        int clen = strlen(copy_environ)+1;

        cea = alloca(clen);
        s_strcpy(cea, clen, copy_environ);

        var = strtok_r(cea, ":", &cp);
        while (var)
        {
            val = getenv(var);

            cgi_setenv(envp, var, val);

            var = strtok_r(NULL, ":", &cp);
        }

    }

    return envp;
}


static void cgi_free_env(char **envp)
{
    int i;


    if (envp == NULL)
        return;

    for (i = 0; i < max_environ && envp[i]; i++)
        s_free(envp[i]);

    s_free(envp);
}

static struct options cgi_cfg_table[] =
{
    { "default-user", 	T_STRING, &cgi_user,   NULL },
    { "default-group", 	T_STRING, &cgi_group,   NULL },
    { "path",		T_STRING, &cgi_path,  NULL },
    { "newroot",	T_STRING, &cgi_newroot,  NULL },
    { "nicevalue",	T_NUMBER, &cgi_niceval, NULL },
    { "maxcputime",	T_NUMBER, &cgi_maxcputime, NULL },
    { "max-vmem",	T_NUMBER, &cgi_vmem, NULL },
    { "max-fd",		T_NUMBER, &cgi_fdmax, NULL },
    { "copy-environ",   T_STRING, &copy_environ, NULL },

    { "allow-user-cgi", T_BOOL,   &user_cgi,  NULL },
    { "run-as-authuser", T_BOOL,   &run_as_authuser,  NULL },

    { "max-processes",  T_NUMBER, &max_processes, NULL },
    { "max-environ",    T_NUMBER, &max_environ, NULL  },

    { NULL, -1, NULL, NULL }
};


int pm_init(const char **argv)
{
    struct passwd *pwp, pwb;
    char pbuf[1024];
    char *cfg_path, *cp;
    const char *name;
    int nlen;


    name = argv[0];

    if (debug > 1)
        fprintf(stderr, "*** cgi/pm_init(\"%s\") called ***\n", name);

    nlen = strlen(name)+6;
    cfg_path = s_malloc(nlen);
    s_strcpy(cfg_path, nlen, name);

    cp = strrchr(cfg_path, '.');
    if (cp && strcmp(cp, ".so") == 0)
        *cp = '\0';

    s_strcat(cfg_path, nlen, ".conf");
    if (config_parse_file(cfg_path, cgi_cfg_table, 0) < 0)
    {
        if (debug > 1)
            fprintf(stderr, "config_parse_file() failed\n");

        return -1;
    }

    if (config_parse_argv(argv+1, cgi_cfg_table) < 0)
    {
        if (debug > 1)
            fprintf(stderr, "config_parse_file() failed\n");
        return -1;
    }

    if (uidgid_get(cgi_user, cgi_group,
                   &cgi_uid, &cgi_gid,
                   &pwp, &pwb, pbuf, sizeof(pbuf)) < 0)
    {
        syslog(LOG_ERR,
               "cgi: uidgid_get(\"%s\", \"%s\") failed, using default",
               cgi_user, cgi_group);
    }

    if (cgi_uid == -1)
        cgi_uid = server_uid;

    if (cgi_gid == -1)
        cgi_gid = server_gid;

    if (cgi_path == NULL)
        cgi_path = s_strdup("/bin:/usr/bin");

    if (max_processes > 0)
        sema_init(&proc_sema, max_processes, USYNC_THREAD, NULL);

    return 0;
}


void pm_exit(void)
{
    if (debug > 1)
        fprintf(stderr, "*** cgi/pm_exit() called ***\n");

    if (cgi_user)
        s_free(cgi_user);

    if (cgi_group)
        s_free(cgi_group);

    if (cgi_path)
        s_free(cgi_path);
}


struct relayinfo
{
    int read_fd;
    int write_fd;
    pid_t pid;
    int content_length;
};


static void *cgi_relay_data(void *misc)
{
    struct relayinfo *rip = (struct relayinfo *) misc;

    if (debug > 4)
        fprintf(stderr, "CGI: cgi_relay_data(%d -> %d, %d bytes)\n",
                rip->read_fd, rip->write_fd, rip->content_length);

    fd_nrelay(rip->read_fd, rip->write_fd, 0, rip->content_length);

    if (debug > 4)
        fprintf(stderr, "CGI: cgi_relay_data(), closing %d\n",
                rip->write_fd);

    fd_close(rip->write_fd);

    if (debug > 4)
        fprintf(stderr, "CGI: cgi_relay_data(): Stop\n");

    return NULL;
}


int pm_request(struct connectioninfo *cip)
{
    pid_t pid;
    int status, len, why, code;
    int content_length = -1;
    int headers_sent = 0;
    int lineno = 0;
    int p2c_pipe[2];
    int c2p_pipe[2];
    int flags;
    int result;
    char **nargv = NULL;
    char buf[1024];
    char *cp, *rp;
    char **envp;
    char *wdir = NULL;
    uid_t uid = -1;
    gid_t gid = -1;
    struct passwd *pwp, pwb;
    char pbuf[1024];
    struct stat sb;
    char path[1025];
    char path_translated[1025];
    struct relayinfo rb;
    thread_t tid;
    char *url;
    char *path_info;
    int fd = cip->fd;
    struct httpinfo *hip = cip->hip;


    c2p_pipe[0] = c2p_pipe[1] = -1;

    if (debug > 1)
    {
        fprintf(stderr, "*** cgi/pm_request(\"%s\") called ***\n",
                hip->url);
        if (hip->request)
            fprintf(stderr, "\thip->request = \"%s\"\n", hip->request);
        if (hip->orig_url)
            fprintf(stderr, "\thip->orig_url = \"%s\"\n", hip->orig_url);
        if (hip->orig_request)
            fprintf(stderr, "\thip->orig_request = \"%s\"\n", hip->orig_request);
    }

    result = 0;

    len = strlen(hip->url);
    url = (char *) alloca(len + 1);
    s_strcpy(url, len+1, hip->url);

    /* Allow to RUN as userid of authuser ? */
    if ( run_as_authuser == 1 )
    {
        if (uidgid_get(cip->auth.user, NULL,
                       &uid, &gid,
                       &pwp, &pwb, pbuf, sizeof(pbuf)) < 0)
        {
            syslog(LOG_ERR,
                   "cgi: uidgid_get(\"%s\") failed for run_as_authuser, using default",
                   cip->auth.user);
        }
    }

    cp = NULL;


    if (debug > 3)
        fprintf(stderr, "CGI: before url_expand(), url=%s\n", url);

    while ((rp = url_expand(url, path, sizeof(path), &sb, &uid, &gid)) == NULL)
    {
        if (debug > 3)
            fprintf(stderr, "CGI: in url_expand(), url=%s\n", url);

        /* Do some CGI special handling to support the use of things
         * like:
             *
         *  	/cgi-bin/imagemap/foo/bar/fubar.gif
             *
         *  where /cgi-bin/imagemap is the CGI program...
         *
         * Should perhaps be modified to try things from the beginning
         * first instead of backwards. Later.
         */
        cp = strrchr(url, '/');
        if (cp == NULL)
            break;

        *cp = '\0';
    }

    if (cp == NULL && len > 0)
        if (url[len-1] == '/')
        {
            cp = url+len-1;
            *cp++ = 0;
        }

    if (debug > 3)
        fprintf(stderr, "CGI: after url_expand(), url=%s\n", url);

    /* We don't allow users to run CGI scripts? */
    if (user_cgi == 0 && uid != -1)
        return -1;

    if (rp == NULL || !S_ISREG(sb.st_mode))
        return -1;

    if (cp)
    {
        path_info = hip->url + (cp - url);
        if (*path_info == 0)
            path_info = "/";

        path_translated[0] = '\0';
        url_expand(path_info,
                   path_translated, sizeof(path_translated),
                   NULL, NULL, NULL);
    }
    else
    {
        path_info = NULL;
        path_translated[0] = '\0';
    }

    status = 0;
    envp = cgi_setup_env(hip->method,
                         url,
                         cip, hip, hip->mip,
                         cgi_path,
                         path_info,
                         path_translated);

    if (hip->mip)
    {
        cp = mime_getheader(hip->mip, "CONTENT-LENGTH", 1);
        if (cp)
            content_length = atoi(cp);
    }

    len = strlen(hip->url);

    if (pipe(p2c_pipe) < 0)
    {
        cgi_free_env(envp);
        return error_system(cip, "pipe()");
    }

    if (debug > 6)
        fprintf(stderr, "CGI: pipe(p2c_pipe), [0]=%d, [1]=%d\n",
                p2c_pipe[0], p2c_pipe[1]);

    flags = fcntl(p2c_pipe[0], F_GETFL, 0);
    flags &= ~O_ACCMODE;
    flags |= O_RDONLY;
    fcntl(p2c_pipe[0], F_SETFL, flags);

    flags = fcntl(p2c_pipe[1], F_GETFL, 0);
    flags &= ~O_ACCMODE;
    flags |= O_WRONLY;
    fcntl(p2c_pipe[1], F_SETFL, flags);

    fd_reopen(p2c_pipe[1], O_WRONLY, FDT_PIPE);

    if (pipe(c2p_pipe) < 0)
    {
        cgi_free_env(envp);
        return error_system(cip, "pipe()");
    }

    if (debug > 6)
        fprintf(stderr, "CGI: pipe(c2p_pipe), [0]=%d, [1]=%d\n",
                c2p_pipe[0], c2p_pipe[1]);

    flags = fcntl(c2p_pipe[0], F_GETFL, 0);
    flags &= ~O_ACCMODE;
    flags |= O_RDONLY;
    fcntl(c2p_pipe[0], F_SETFL, flags);

    flags = fcntl(c2p_pipe[1], F_GETFL, 0);
    flags &= ~O_ACCMODE;
    flags |= O_WRONLY;
    fcntl(c2p_pipe[1], F_SETFL, flags);


    /* Set up the process argv[] array */
    if (hip->request == NULL || strchr(hip->request, '='))
    {
        nargv = (char **) alloca(2 * sizeof(char *));
        if (nargv == NULL)
            s_abort();

        nargv[0] = hip->url;
        nargv[1] = NULL;
    }
    else
    {
        char *cp, *start;
        int i;


        i = 3; /* argv[0] + first arg + NULL */
        for (cp = hip->request; *cp; cp++)
            if (*cp == '+')
                ++i;

        nargv = (char **) alloca(i * sizeof(char *));
        if (nargv == NULL)
            s_abort();

        nargv[0] = hip->url;
        i = 1;
        start = hip->request;
        do
        {
            nargv[i++] = start;

            cp = strchr(start, '+');
            if (cp)
            {
                *cp++ = '\0';
                start = cp;
            }
        }
        while (cp);
        nargv[i] = NULL;
    }

    if (uid == -1)
        uid = cgi_uid;
    if (gid == -1)
        gid = cgi_gid;

    if (max_processes > 0)
        sema_wait(&proc_sema);

    pid = proc_run(path,
                   uid, gid,
                   cgi_newroot,
                   cgi_niceval, cgi_vmem, cgi_fdmax, cgi_maxcputime,
                   p2c_pipe[0], c2p_pipe[1], 2,
                   nargv, envp,
                   wdir);

    if (pid < 0)
    {
        if (debug > 4)
            fprintf(stderr, "cgi.c:proc_run() failed\n");

        s_close(p2c_pipe[0]);
        fd_close(p2c_pipe[1]);
        if (c2p_pipe[0] != -1)
            s_close(c2p_pipe[0]);
        if (c2p_pipe[1] != -1)
            s_close(c2p_pipe[1]);

        if (max_processes > 0)
            sema_post(&proc_sema);

        return error_system(cip, "CGI: proc_run()");
    }

    if (s_close(p2c_pipe[0]) < 0)
    {
        if (debug > 4)
            fprintf(stderr, "cgi.c:s_close(p2c_pipe[0]=%d) failed",
                    p2c_pipe[0]);
    }

    rb.read_fd = fd;
    rb.write_fd = p2c_pipe[1];
    rb.pid = pid;
    rb.content_length = content_length;

    if (thr_create(NULL,
                   0,
                   (void *(*)(void *)) cgi_relay_data,
                   (void *) &rb,
                   bound_threads ? THR_BOUND : 0,
                   &tid))
    {
        if (debug > 4)
            fprintf(stderr, "cgi.c:thr_create(cgi_relay_data) failed\n");

        fd_close(p2c_pipe[1]);

        if (c2p_pipe[0] != -1)
            s_close(c2p_pipe[0]);
        if (c2p_pipe[1] != -1)
            s_close(c2p_pipe[1]);

        if (max_processes > 0)
            sema_post(&proc_sema);

        return -1;
    }

    if (debug > 4)
        fprintf(stderr, "cgi.c: Before s_close(c2p_pipe[1]=%d)\n",
                c2p_pipe[1]);

    if (s_close(c2p_pipe[1]) < 0)
        syslog(LOG_ERR, "cgi.c: s_close(c2p_pipe[1]=%d) failed: %m",
               c2p_pipe[1]);

    fd_reopen(c2p_pipe[0], O_RDONLY, FDT_PIPE);

    if (debug > 4)
        fprintf(stderr, "CGI: After fd_reopen()\n");

    while (fd_gets(buf, sizeof(buf), c2p_pipe[0]))
    {
        lineno++;

        if (*buf == '\r' || *buf == '\n')
            break;

        /* This check is not according to the CGI-standard! */
        if (lineno == 1 && strncmp(buf, "HTTP/", 5) == 0)
        {
            fd_puts(buf, fd);
            fd_relay(c2p_pipe[0], fd, 0);
            buf[0] = '\0';
            headers_sent = 1;
            lineno = 0;
            break;
        }

        cp = strchr(buf, ':');
        if (cp == NULL)
            break;

        *cp = '\0';

        if (strcasecmp(buf, "LOCATION") == 0)
        {
            result = 302;
            http_sendheaders(fd, cip, result, NULL);
            headers_sent = 1;

            fd_printf(fd, "Location: %s\n", cp+1);
        }
        else if (strcasecmp(buf, "STATUS") == 0)
        {
            char buf2[1024], *emsg;
            int rval;


            emsg = NULL;

            rval = sscanf(cp + 1, " %d %[^\n\r]", &result, buf2);
            if (rval < 1)
            {
                result = http_error(cip, 500,
                                    "Invalid response from CGI script: Too few arguments to 'STATUS:'");
                headers_sent = 1;
            }
            else
            {
                if (rval == 2)
                    emsg = buf2;

                http_sendheaders(fd, cip, result, emsg);
                headers_sent = 1;
            }
        }
        else
        {
            if (headers_sent == 0)
            {
                result = 200;
                http_sendheaders(fd, cip, result, NULL);
                headers_sent = 1;
            }

            *cp = ':';
            fd_puts(buf, fd);
        }

        buf[0] = '\0';
    }

    if (debug > 4)
        fprintf(stderr, "CGI: before if (lineno != 0), lineno=%d\n", lineno);

    if (lineno != 0)
    {
        int len;


        if (debug > 4)
            fprintf(stderr, "CGI: Got data from subprocess\n");

        if (!headers_sent)
        {
            result = 200;
            http_sendheaders(fd, cip, result, NULL);
            fd_puts("Content-Type: text/plain\n", fd);
        }

        if (cip->hip->mip)
            fd_putc('\n', fd);

        len = fd_written(fd);

        if (buf[0] && buf[0] != '\n' && buf[0] != '\r')
            fd_puts(buf, fd);

        fd_relay(c2p_pipe[0], fd, 0);
    }
#if 0
    else
        fd_shutdown();
#endif

    if (fd_close(c2p_pipe[0]) < 0)
    {
        if (debug > 4)
            fprintf(stderr, "CGI: fd_close(c2p_pipe[0]=%d) failed\n",
                    c2p_pipe[0]);
    }

    c2p_pipe[0] = -1;

    errno = 0;

    if (debug > 4)
        fprintf(stderr, "CGI: before proc_wait()\n");

    why = proc_wait(pid, &code);
    switch (why)
    {
    case PROC_EXIT:
        if (debug > 4)
            fprintf(stderr, "CGI: proc_wait() returned PROC_EXIT, code=%d\n",
                    code);

        if (code < 0)
            result = -code;
        else if (code > 0)
            result = http_error(cip,
                                500,
                                "CGI process exited with exit code %d",
                                code);
        break;

    case PROC_SIGNAL:
        if (debug > 4)
            fprintf(stderr, "CGI: proc_wait() returned PROC_SIGNAL, code=%d\n",
                    code);

        result = http_error(cip, 500,
                            "CGI process exited due to signal %d",
                            code);
        break;

    default:
        if (debug > 4)
            fprintf(stderr, "CGI: proc_wait() returned %d, code=%d\n",
                    why, code);

        result = http_error(cip,
                            500,
                            "CGI process terminated unexpectedly");
    }

    cgi_free_env(envp);

    if (debug > 5)
        fprintf(stderr, "CGI: before thr_join (cgi_relay_data)\n");

    thr_join(tid, NULL, NULL);
    if (debug > 5)
        fprintf(stderr, "CGI: before fd_close\n");

    if (c2p_pipe[0] != -1)
        fd_close(c2p_pipe[0]);

    if (debug > 5)
        fprintf(stderr, "CGI: before sema_post\n");

    if (max_processes > 0)
        sema_post(&proc_sema);

    return result;
}
