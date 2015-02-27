/* Dealing with host names.
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

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#ifdef HAVE_STRING_H
# include <string.h>
#else
# include <strings.h>
#endif
#include <assert.h>
#include <sys/types.h>

#ifdef WINDOWS
# include <winsock.h>
#else
# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
#endif /* WINDOWS */

#ifdef HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>
#endif
#include <errno.h>

#include "wget.h"
#include "utils.h"
#include "host.h"
#include "url.h"

#ifndef errno
extern int errno;
#endif

//z 主机列表
/* Host list entry */
struct host
{
    /* Host's symbolical name, as encountered at the time of first
    	inclusion, e.g. "fly.cc.fer.hr".  */
    char *hostname;//z 域名
    /* Host's "real" name, i.e. its IP address, written out in ASCII
    form of N.N.N.N, e.g. "161.53.70.130".  */
    char *realname;//z ip地址
    //z 一个ip地址可以对应多个域名。
    /* More than one HOSTNAME can correspond to the same REALNAME.  For
    our purposes, the canonical name of the host is its HOSTNAME when
    it was first encountered.  This entry is said to have QUALITY.  */
    int quality;//z
    /* Next entry in the list.  */
    struct host *next;
};

static struct host *hlist;

static struct host *add_hlist PARAMS ((struct host *, const char *,
                                       const char *, int));

/* The same as gethostbyname, but supports internet addresses of the
   form `N.N.N.N'.  */
struct hostent *
ngethostbyname (const char *name)
{
    struct hostent *hp;
    unsigned long addr;

    addr = (unsigned long)inet_addr (name);
    if ((int)addr != -1)
        hp = gethostbyaddr ((char *)&addr, sizeof (addr), AF_INET);
    else
        hp = gethostbyname (name);

    return hp;
}

/* Search for HOST in the linked list L, by hostname.  Return the
   entry, if found, or NULL.  The search is case-insensitive.  */
//z 搜索 host 列表
static struct host *
search_host (struct host *l, const char *host)
{
    for (; l; l = l->next)
    {
        //z 单纯比较 host 名称
        if (strcasecmp (l->hostname, host) == 0)
            return l;
    }

    return NULL;
}

/* Like search_host, but searches by address.  */
//z 是否是否IP地址已经在host list中了。
static struct host *
search_address (struct host *l, const char *address)
{
    for (; l; l = l->next)
    {
        //z 比较的是IP地址
        int cmp = strcmp (l->realname, address);

        if (cmp == 0)
            return l;
        else if (cmp > 0)
            return NULL;
    }

    return NULL;
}

/* Store the address of HOSTNAME, internet-style, to WHERE.  First
   check for it in the host list, and (if not found), use
   ngethostbyname to get it.

   Return 1 on successful finding of the hostname, 0 otherwise.  */
int
store_hostaddress (unsigned char *where, const char *hostname)
{
    struct host *t;
    unsigned long addr;
    struct hostent *hptr;
    struct in_addr in;
    char *inet_s;

    /* If the address is of the form d.d.d.d, there will be no trouble
    with it.  */
    //z 在windows上出错时会返回 INADDR_NONE 或者 INADDR_ANY
    addr = (unsigned long)inet_addr (hostname);
    if ((int)addr == -1)
    {
        /* If it is not of that form, try to find it in the cache.  */
        //z 不在那种格式里，返回出错，搜索已经保存的host信息。
        t = search_host (hlist, hostname);
        //z 如果找到了
        if (t)
            //z 那么使用该ip地址
            addr = (unsigned long)inet_addr (t->realname);
    }
    /* If we have the numeric address, just store it.  */
    if ((int)addr != -1)
    {
        /* This works on both little and big endian architecture, as
        inet_addr returns the address in the proper order.  It
        	appears to work on 64-bit machines too.  */
        //z 将addr拷贝到where
        memcpy (where, &addr, 4);
        return 1;
    }
    /* Since all else has failed, let's try gethostbyname().  Note that
    we use gethostbyname() rather than ngethostbyname(), because we
    *know* the address is not numerical.  */
    //z 如果不是IP地址的形式且host list中没有该host
    hptr = gethostbyname (hostname);
    if (!hptr)
        return 0;
    /* Copy the address of the host to socket description.  */
    //z 拷贝host的地址到where
    memcpy (where, hptr->h_addr_list[0], hptr->h_length);
    //z 将hostname给cache起来；首先要查找该IP地址是否已经有所指向
    /* Now that we're here, we could as well cache the hostname for
    future use, as in realhost().  First, we have to look for it by
    address to know if it's already in the cache by another name.  */

    /* Originally, we copied to in.s_addr, but it appears to be missing
    on some systems.  */
    memcpy (&in, *hptr->h_addr_list, sizeof (in));
    //z The inet_ntoa function converts an (Ipv4) Internet network address into an ASCII string in Internet standard dotted-decimal format.
    STRDUP_ALLOCA (inet_s, inet_ntoa (in));
    //z 查看该ip地址是否已储存
    t = search_address (hlist, inet_s);
    //z 如果在 host list 中已发现
    if (t) /* Found in the list, as realname.  */
    {
        /* Set the default, 0 quality.  */
        //z 该ip已经存在过了，那么设置 quality 为0。
        hlist = add_hlist (hlist, hostname, inet_s, 0);
        return 1;
    }
    /* Since this is really the first time this host is encountered,
    set quality to 1.  */
    //z 否则，该ip地址在host list中并不存在，那么quality为1
    hlist = add_hlist (hlist, hostname, inet_s, 1);
    return 1;
}

/* Add a host to the host list.  The list is sorted by addresses.  For
   equal addresses, the entries with quality should bubble towards the
   beginning of the list.  */
static struct host *
add_hlist (struct host *l, const char *nhost, const char *nreal, int quality)
{
    struct host *t, *old, *beg;

    /* The entry goes to the beginning of the list if the list is empty
    or the order requires it.  */
    //z 如果host list为NULL，或者小于list header的 ip 地址
    if (!l || (strcmp (nreal, l->realname) < 0))
    {
        t = (struct host *)xmalloc (sizeof (struct host));
        t->hostname = xstrdup (nhost);
        t->realname = xstrdup (nreal);
        //z 此时存储为0。
        t->quality = quality;
        t->next = l;
        return t;
    }

    beg = l;
    /* Second two one-before-the-last element.  */
    while (l->next)
    {
        int cmp;
        old = l;
        l = l->next;
        //z 比较的都是 ip 地址。可能多个 host name 对应一个IP地址
        cmp = strcmp (nreal, l->realname);
        if (cmp >= 0)
            continue;
        /* If the next list element is greater than s, put s between the
        current and the next list element.  */
        t = (struct host *)xmalloc (sizeof (struct host));
        old->next = t;
        t->next = l;
        t->hostname = xstrdup (nhost);
        t->realname = xstrdup (nreal);
        t->quality = quality;
        return beg;
    }

    //z 遍历完了 host list 也没有发现，那么将该host放置在host list 最后面
    t = (struct host *)xmalloc (sizeof (struct host));
    t->hostname = xstrdup (nhost);
    t->realname = xstrdup (nreal);
    t->quality = quality;
    /* Insert the new element after the last element.  */
    l->next = t;
    t->next = NULL;
    return beg;
}

/*
	判断由wget获取的HOST的 real name。如果HOST引用了多于一次，那么返回头一次遇到的值

	如果host在host list 中没有发现。
	如果失败了（没有发现？），那么使用ip地址，如果仍旧没有发现，将之加入到Host list 中去
	实际是实现了一个简单的缓存？
*/
/* Determine the "real" name of HOST, as perceived by Wget.  If HOST
   is referenced by more than one name, "real" name is considered to
   be the first one encountered in the past.

   If the host cannot be found in the list of already dealt-with
   hosts, try with its INET address.  If this fails too, add it to the
   list.  The routine does not call gethostbyname twice for the same
   host if it can possibly avoid it.  */
char *
realhost (const char *host)
{
    struct host *l;
    struct in_addr in;
    struct hostent *hptr;
    char *inet_s;

    DEBUGP (("Checking for %s.\n", host));
    /* Look for the host, looking by the host name.  */
	//z 在host list中查找对应的host
    l = search_host (hlist, host);

    //z 如果 quality 不为0
    if (l && l->quality)              /* Found it with quality */
    {
        //z
        DEBUGP (("%s was already used, by that name.\n", host));
        /* Here we return l->hostname, not host, because of the possible
           case differences (e.g. jaGOR.srce.hr and jagor.srce.hr are
           the same, but we want the one that was first.  */
        return xstrdup (l->hostname);
    }
    else if (!l)                      /* Not found, with or without quality */
    {
        /* The fact that gethostbyname will get called makes it
        necessary to store it to the list, to ensure that
         gethostbyname will not be called twice for the same string.
         However, the quality argument must be set appropriately.

         Note that add_hlist must be called *after* the realname
         search, or the quality would be always set to 0 */
        DEBUGP (("This is the first time I hear about host %s by that name.\n",
                 host));
        hptr = ngethostbyname (host);
        if (!hptr)
            return xstrdup (host);
        /* Originally, we copied to in.s_addr, but it appears to be
           missing on some systems.  */
        memcpy (&in, *hptr->h_addr_list, sizeof (in));
        STRDUP_ALLOCA (inet_s, inet_ntoa (in));
    }
    else /* Found, without quality */
    {
        /* This case happens when host is on the list,
        but not as first entry (the one with quality).
         Then we just get its INET address and pick
         up the first entry with quality.  */
        DEBUGP (("We've dealt with host %s, but under the name %s.\n",
                 host, l->realname));
        STRDUP_ALLOCA (inet_s, l->realname);
    }

    /* Now we certainly have the INET address.  The following loop is
       guaranteed to pick either an entry with quality (because it is
       the first one), or none at all.  */
    //z 通过host name没有发现，那么通过ip地址再次查找
    l = search_address (hlist, inet_s);
    if (l) /* Found in the list, as realname.  */
    {
        /* Set the default, 0 quality.  */
        //z 如果找到，那么设置quality为0，加入host list，设置quality为0
        hlist = add_hlist (hlist, host, inet_s, 0);
        return xstrdup (l->hostname);
    }
    /* Since this is really the first time this host is encountered,
       set quality to 1.  */
    //z 通过host以及ip均没有找到，确定是第一次遇上，加入list host，设置quality为1
    hlist = add_hlist (hlist, host, inet_s, 1);
    return xstrdup (host);
}

/*
比较两个 host 是否是同样的host，从url中解析出hostname，查看是否相同；根据option的选项设置可能会进一步比较realhost
*/
/* Compare two hostnames (out of URL-s if the arguments are URL-s),
   taking care of aliases.  It uses realhost() to determine a unique
   hostname for each of two hosts.  If simple_check is non-zero, only
   strcmp() is used for comparison.  */
int
same_host (const char *u1, const char *u2)
{
    const char *s;
    char *p1, *p2;
    char *real1, *real2;

    /* Skip protocol, if present.  */
    u1 += skip_url (u1);//z 跳过 URL: 以及紧随气候的空白符
    u2 += skip_url (u2);
	//z 越过协议名称
    u1 += skip_proto (u1);
    u2 += skip_proto (u2);

    /* Skip username ans password, if present.  */
	//z http://user:pass@www.moo.ui
    u1 += skip_uname (u1);
    u2 += skip_uname (u2);

	//z 步进；直到找到 '/' 或 ':'
    for (s = u1; *u1 && *u1 != '/' && *u1 != ':'; u1++);
    p1 = strdupdelim (s, u1);// [ )，在heap上复制一份
    for (s = u2; *u2 && *u2 != '/' && *u2 != ':'; u2++);
    p2 = strdupdelim (s, u2);// [ )，在heap上复制一份
    DEBUGP (("Comparing hosts %s and %s...\n", p1, p2));
    //z 比较host
	if (strcasecmp (p1, p2) == 0)//z 如果相等
    {
		//z p1及p2是在heap上分配的。比较完毕释放
        free (p1);
        free (p2);
        DEBUGP (("They are quite alike.\n"));
        return 1;
    }
    else if (opt.simple_check)//z 是否进行简单比较
    {
        free (p1);
        free (p2);
        DEBUGP (("Since checking is simple, I'd say they are not the same.\n"));
        return 0;//z 采用了 simple 比较的形式，那么这就看作是不一样的
    }

	//z 如果不等，且不采用 simple_check 的形式
	//z 分别根据p1和p2得到其对应的 realhost
    real1 = realhost (p1);
    real2 = realhost (p2);
    free (p1);
    free (p2);
    //z 对realhost进行对应的比较
	if (strcasecmp (real1, real2) == 0)
    {
        DEBUGP (("They are alike, after realhost()->%s.\n", real1));
        free (real1);
        free (real2);
        return 1;
    }
    else
    {
        DEBUGP (("They are not the same (%s, %s).\n", real1, real2));
        free (real1);
        free (real2);
        return 0;
    }
}

/* Determine whether a URL is acceptable to be followed, according to
   a list of domains to accept.  */
int
accept_domain (struct urlinfo *u)
{
    assert (u->host != NULL);
    if (opt.domains)
    {
        if (!sufmatch ((const char **)opt.domains, u->host))
            return 0;
    }
    if (opt.exclude_domains)
    {
        if (sufmatch ((const char **)opt.exclude_domains, u->host))
            return 0;
    }
    return 1;
}

/* Check whether WHAT is matched in LIST, each element of LIST being a
   pattern to match WHAT against, using backward matching (see
   match_backwards() in utils.c).

   If an element of LIST matched, 1 is returned, 0 otherwise.  */
int
sufmatch (const char **list, const char *what)
{
    int i, j, k, lw;

    lw = strlen (what);
    for (i = 0; list[i]; i++)
    {
        for (j = strlen (list[i]), k = lw; j >= 0 && k >= 0; j--, k--)
            if (tolower (list[i][j]) != tolower (what[k]))
                break;
        /* The domain must be first to reach to beginning.  */
        if (j == -1)
            return 1;
    }
    return 0;
}

/* Return email address of the form username@FQDN suitable for
   anonymous FTP passwords.  This process is error-prone, and the
   escape hatch is the MY_HOST preprocessor constant, which can be
   used to hard-code either your hostname or FQDN at compile-time.

   If the FQDN cannot be determined, a warning is printed, and the
   function returns a short `username@' form, accepted by most
   anonymous servers.

   If not even the username cannot be divined, it means things are
   seriously fucked up, and Wget exits.  */
char *
ftp_getaddress (void)
{
    static char *address;

    /* Do the drill only the first time, as it won't change.  */
    if (!address)
    {
        char userid[32];		/* 9 should be enough for Unix, but
				   I'd rather be on the safe side.  */
        char *host, *fqdn;

        if (!pwd_cuserid (userid))
        {
            logprintf (LOG_ALWAYS, _("%s: Cannot determine user-id.\n"),
                       exec_name);
            exit (1);
        }
#ifdef MY_HOST
        STRDUP_ALLOCA (host, MY_HOST);
#else /* not MY_HOST */
#ifdef HAVE_UNAME
        {
            struct utsname ubuf;
            if (uname (&ubuf) < 0)
            {
                logprintf (LOG_ALWAYS, _("%s: Warning: uname failed: %s\n"),
                           exec_name, strerror (errno));
                fqdn = "";
                goto giveup;
            }
            STRDUP_ALLOCA (host, ubuf.nodename);
        }
#else /* not HAVE_UNAME */
#ifdef HAVE_GETHOSTNAME
        host = alloca (256);
        if (gethostname (host, 256) < 0)
        {
            logprintf (LOG_ALWAYS, _("%s: Warning: gethostname failed\n"),
                       exec_name);
            fqdn = "";
            goto giveup;
        }
#else /* not HAVE_GETHOSTNAME */
#error Cannot determine host name.
#endif /* not HAVE_GETHOSTNAME */
#endif /* not HAVE_UNAME */
#endif /* not MY_HOST */
        /* If the address we got so far contains a period, don't bother
           anymore.  */
        if (strchr (host, '.'))
            fqdn = host;
        else
        {
            /* #### I've seen the following scheme fail on at least one
               system!  Do we care?  */
            char *tmpstore;
            /* According to Richard Stevens, the correct way to find the
               FQDN is to (1) find the host name, (2) find its IP
               address using gethostbyname(), and (3) get the FQDN using
               gethostbyaddr().  So that's what we'll do.  Step one has
               been done above.  */
            /* (2) */
            struct hostent *hp = gethostbyname (host);
            if (!hp || !hp->h_addr_list)
            {
                logprintf (LOG_ALWAYS, _("\
%s: Warning: cannot determine local IP address.\n"),
                           exec_name);
                fqdn = "";
                goto giveup;
            }
            /* Copy the argument, so the call to gethostbyaddr doesn't
               clobber it -- just in case.  */
            tmpstore = (char *)alloca (hp->h_length);
            memcpy (tmpstore, *hp->h_addr_list, hp->h_length);
            /* (3) */
            hp = gethostbyaddr (tmpstore, hp->h_length, hp->h_addrtype);
            if (!hp || !hp->h_name)
            {
                logprintf (LOG_ALWAYS, _("\
%s: Warning: cannot reverse-lookup local IP address.\n"),
                           exec_name);
                fqdn = "";
                goto giveup;
            }
            if (!strchr (hp->h_name, '.'))
            {
#if 0
                /* This gets ticked pretty often.  Karl Berry reports
                       that there can be valid reasons for the local host
                       name not to be an FQDN, so I've decided to remove the
                       annoying warning.  */
                logprintf (LOG_ALWAYS, _("\
%s: Warning: reverse-lookup of local address did not yield FQDN!\n"),
                           exec_name);
#endif
                fqdn = "";
                goto giveup;
            }
            /* Once we're here, hp->h_name contains the correct FQDN.  */
            STRDUP_ALLOCA (fqdn, hp->h_name);
        }
giveup:
        address = (char *)xmalloc (strlen (userid) + 1 + strlen (fqdn) + 1);
        sprintf (address, "%s@%s", userid, fqdn);
    }
    return address;
}

/* Print error messages for host errors.  */
char *
herrmsg (int error)
{
    /* Can't use switch since some constants are equal (at least on my
       system), and the compiler signals "duplicate case value".  */
    if (error == HOST_NOT_FOUND
            || error == NO_RECOVERY
            || error == NO_DATA
            || error == NO_ADDRESS
            || error == TRY_AGAIN)
        return _("Host not found");
    else
        return _("Unknown error");
}

/* Clean the host list.  This is a separate function, so we needn't
   export HLIST and its implementation.  Ha!  */
void
clean_hosts (void)
{
    struct host *l = hlist;

    while (l)
    {
        struct host *p = l->next;
        free (l->hostname);
        free (l->realname);
        free (l);
        l = p;
    }
    hlist = NULL;
}
