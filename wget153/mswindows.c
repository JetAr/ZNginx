/* mswindows.c -- Windows-specific support
   Copyright (C) 1995, 1996, 1997, 1998  Free Software Foundation, Inc.

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

/* #### Someone document these functions!  */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>
#include <string.h>
#include <assert.h>

#include "wget.h"
#include "url.h"

char *argv0;

/* Defined in log.c.  */
void redirect_output (const char *);

static int windows_nt_p;


/* Emulation of Unix sleep.  */
unsigned int
sleep (unsigned seconds)
{
    Sleep (1000 * seconds);
    /* Unix sleep() is interruptible.  To make it semi-usable, it
       returns a value that says how much it "really" slept, or some
       junk like that.  Ignore it.  */
    return 0U;
}

//z 读取注册表
static char *
read_registry (HKEY hkey, char *subkey, char *valuename, char *buf, int *len)
{
    HKEY result;
    DWORD size = *len;
    DWORD type = REG_SZ;
	//z 首先打开 key
    if (RegOpenKeyEx (hkey, subkey, NULL, KEY_READ, &result) != ERROR_SUCCESS)
        return NULL;
	//z 然后读取值
    if (RegQueryValueEx (result, valuename, NULL, &type, buf, &size) != ERROR_SUCCESS)
        buf = NULL;
    *len = size;
    RegCloseKey (result);
    return buf;
}

//z 获取当前登录用户名
char *
pwd_cuserid (char *where)
{
    char buf[32], *ptr;
    int len = sizeof (buf);
	//z 使用了多种方式来获取 user name 。
    if (GetUserName (buf, (LPDWORD) &len) == TRUE)
    {
        ;
    }
    else if (!!(ptr = getenv ("USERNAME")))
    {
        strcpy (buf, ptr);
    }
    else if (!read_registry (HKEY_LOCAL_MACHINE, "Network\\Logon",
                             "username", buf, &len))
    {
        return NULL;
    }
    if (where)
    {
        strncpy (where, buf, len);
        return where;
    }
    return xstrdup (buf);
}

// ×÷ÓÃÀàËÆÓÚ wget.exe -> wget
void
windows_main_junk (int *argc, char **argv, char **exec_name)
{
    char *p;

    argv0 = argv[0];

    /* Remove .EXE from filename if it has one.  */
    *exec_name = xstrdup (*exec_name);

    p = strrchr (*exec_name, '.');
    if (p)
        *p = '\0';
}

/* Winsock stuff. */
//z winsock 清理工作
static void
ws_cleanup (void)
{
    WSACleanup ();
}

static void
ws_hangup (void)
{
    redirect_output (_("\n\
CTRL+Break received, redirecting output to `%s'.\n\
Execution continued in background.\n\
You may stop Wget by pressing CTRL+ALT+DELETE.\n"));
}

void
fork_to_background (void)
{
    /* Whether we arrange our own version of opt.lfilename here.  */
    int changedp = 0;

	//z l 表示日志文件，filename 表示文件名称
    if (!opt.lfilename)
    {
		//z 根据文件前缀生成一个文件名，前缀+数字（数字增大，直到找到一个不存在的文件为止）
        opt.lfilename = unique_name (DEFAULT_LOGFILE);
        changedp = 1;
    }
    printf (_("Continuing in background.\n"));
    //z 成功生成日志记录文件，输出信息
	if (changedp)
        printf (_("Output will be written to `%s'.\n"), opt.lfilename);

    ws_hangup ();
    if (!windows_nt_p)
        FreeConsole ();
}

static BOOL WINAPI
ws_handler (DWORD dwEvent)
{
    switch (dwEvent)
    {
#ifdef CTRLC_BACKGND
    case CTRL_C_EVENT:
#endif
#ifdef CTRLBREAK_BACKGND
    case CTRL_BREAK_EVENT:
#endif
        fork_to_background ();
        break;
    case CTRL_SHUTDOWN_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    default:
        WSACleanup ();
        return FALSE;
    }
    return TRUE;
}

//z 设置 console 的标题
void
ws_changetitle (char *url, int nurl)
{
    char *title_buf;
    if (!nurl)
        return;

    title_buf = (char *)xmalloc (strlen (url) + 20);
    sprintf (title_buf, "Wget %s%s", url, nurl == 1 ? "" : " ...");
    /* #### What are the semantics of SetConsoleTitle?  Will it free the
       given memory later?  */
    SetConsoleTitle (title_buf);
}

char *
ws_mypath (void)
{
    static char *wspathsave;
    char *buffer;
    int rrr;
    char *ptr;

	//z 如果 wspathsave 不为NULL，那么直接返回，可以避免不必要的再次计算
    if (wspathsave)
    {
        return wspathsave;
    }
	//z 从后向前查找第一个 '\' 的位置
    ptr = strrchr (argv0, '\\');
    if (ptr)
    {
		//z 这样更改了 argv0 的内容了？
        *(ptr + 1) = '\0';
		//z 分配适当的空间
        wspathsave = (char*) xmalloc (strlen(argv0)+1);
		//z 然后拷贝；应该可以直接使用 strdup 之类的吧？
        strcpy (wspathsave, argv0);
        return wspathsave;
    }
    buffer = (char*) xmalloc (256);
    rrr = SearchPath (NULL, argv0, strchr (argv0, '.') ? NULL : ".EXE",
                      256, buffer, &ptr);
    if (rrr && rrr <= 256)
    {
        *ptr = '\0';
        wspathsave = (char*) xmalloc (strlen(buffer)+1);
        strcpy (wspathsave, buffer);
        return wspathsave;
    }
    free (buffer);
    return NULL;
}

void
ws_help (const char *name)
{
    char *mypath = ws_mypath ();

    if (mypath)
    {
        struct stat sbuf;
        char *buf = (char *)alloca (strlen (mypath) + strlen (name) + 4 + 1);
        sprintf (buf, "%s%s.HLP", mypath, name);
        if (stat (buf, &sbuf) == 0)
        {
            printf (_("Starting WinHelp %s\n"), buf);
            WinHelp (NULL, buf, HELP_INDEX, NULL);
        }
        else
        {
            printf ("%s: %s\n", buf, strerror (errno));
        }
    }
}

void
ws_startup (void)
{
    WORD requested;
    WSADATA data;
    int err;
    OSVERSIONINFO os;

	//z 获取 windows 版本信息
    if (GetVersionEx (&os) == TRUE
            && os.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
        windows_nt_p = 1;

	//z 对 winsock 进行初始化，这里使用的 1.1 版本
    requested = MAKEWORD (1, 1);
    err = WSAStartup (requested, &data);

	//z 调用函数出错；直接结束程序
    if (err != 0)
    {
        fprintf (stderr, _("%s: Couldn't find usable socket driver.\n"),
                 exec_name);
        exit (1);
    }

	//z 未发现可用的 socket driver （发现的版本号小于所需要的）
    if (LOBYTE (requested) < 1 || (LOBYTE (requested) == 1 &&
                                   HIBYTE (requested) < 1))
    {
        fprintf (stderr, _("%s: Couldn't find usable socket driver.\n"),
                 exec_name);
        WSACleanup ();
        exit (1);
    }

	//z 在程序结束的时候运行 ws_cleanup 函数
    atexit (ws_cleanup);
	//z 设置控制台处理程序
    SetConsoleCtrlHandler (ws_handler, TRUE);
}
