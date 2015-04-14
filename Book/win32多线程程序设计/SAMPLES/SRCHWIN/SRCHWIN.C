/*
 * SrchWin.c
 *
 * Sample code for "Multithreading Applications in Win32"
 * This is from Chapter 8, Listing 8-2
 *
 * Uses multiple threads to search the files 
 * "*.c" in the current directory for the string
 * given on the command line.
 *
 * This example avoids most C run-time functions
 * so that it can use the single-threaded
 * C libraries.
 *
 * It is necessary to use a critical section to
 * divvy up output to the screen or the various
 * threads end up with their output intermingled.
 * Normally the multithreaded C run-time does this 
 * automatically if you use printf.
 *
 */

#include <windows.h>
#include "MtVerify.h"

DWORD WINAPI SearchProc( void *arg );
BOOL GetLine( HANDLE hFile, LPSTR buf, DWORD size );

#define MAX_THREADS 3

HANDLE hThreadLimitSemaphore;    /* Counting semaphore */
HANDLE hConsoleOut;              /* Console output */
CRITICAL_SECTION ScreenCritical; /* Lock screen updates */

char szSearchFor[1024];

/*  2015-04-14 14:29
    遍历文件夹下的c文件；逐行读入，对每个文件开启一个线程，对该文件查找字符串，如果找到，输出对应
    的行。
*/

int main(int argc, char *argv[])
{
    WIN32_FIND_DATA *lpFindData;
    HANDLE hFindFile;
    HANDLE hThread;
    DWORD dummy;
    int i;

    //z 得到 output handle 。
    hConsoleOut = GetStdHandle( STD_OUTPUT_HANDLE );

    if (argc != 2)
    {
        char errbuf[512];
        wsprintf(errbuf,
            "Usage: %s <search-string>\n",
            argv[0]);
        //z 得到 console out ，可以不用open，直接写
        WriteFile( hConsoleOut,
            errbuf,
            strlen(errbuf),
            &dummy,
            FALSE );
        return EXIT_FAILURE;
    }

    /* Put search string where everyone can see it */
    //z 将待搜索字符串拷贝到全局变量中去。
    strcpy(szSearchFor, argv[1]);

    /* Allocate a find buffer to be handed
     * to the first thread */
    //z 分配一个 find buffer ， 给第一个thread
    lpFindData = HeapAlloc( GetProcessHeap(),
            HEAP_ZERO_MEMORY,
            sizeof(WIN32_FIND_DATA) );

    /* Semaphore prevents too many threads from running */
    //z 创建 semaphore 。
    MTVERIFY( hThreadLimitSemaphore = CreateSemaphore(
            NULL,   /* Security */
            MAX_THREADS,    /* Make all of them available */
            MAX_THREADS,    /* Allow a total of MAX_THREADS */
            NULL )          /* Unnamed */
         );

    //z 初始化 critical section 。
    InitializeCriticalSection(&ScreenCritical);

    hFindFile = FindFirstFile( "*.c", lpFindData );

    if (hFindFile == INVALID_HANDLE_VALUE)
        return EXIT_FAILURE;

    do {
        //z 在 semaphore 上等待会有什么样的结果了？
        WaitForSingleObject( hThreadLimitSemaphore,
                        INFINITE );

        MTVERIFY( hThread = CreateThread(NULL,
                        0,
                        SearchProc,
                        lpFindData, // arglist
                        0,
                        &dummy )
         );

        MTVERIFY( CloseHandle( hThread ) );

        lpFindData = HeapAlloc( GetProcessHeap(),
                        HEAP_ZERO_MEMORY,
                        sizeof(WIN32_FIND_DATA) );
    } while ( FindNextFile( hFindFile, lpFindData ));

    FindClose( hFindFile );
    hFindFile = INVALID_HANDLE_VALUE;

    for (i=0; i<MAX_THREADS; i++)
        WaitForSingleObject( hThreadLimitSemaphore,
                        INFINITE );

    MTVERIFY( CloseHandle( hThreadLimitSemaphore ) );

    return EXIT_SUCCESS;
}


DWORD WINAPI SearchProc( void *arg )
{
    WIN32_FIND_DATA *lpFindData = (WIN32_FIND_DATA *)arg;
    char buf[1024];
    HANDLE hFile;
    DWORD dummy;

    //z 打开文件
    hFile = CreateFile(lpFindData->cFileName,
                        GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,
                        OPEN_EXISTING,
                        FILE_FLAG_SEQUENTIAL_SCAN,
                        NULL
                    );
    //z 出错，会造成heap分配的没有释放。
    if (!hFile)
        return 1;   /* Silently ignore problem files */

    //z 从文件中逐行读取？
    //z 如果找到该行有待查找字符，那么输出该行到 standout
    while (GetLine(hFile, buf, sizeof(buf)))
    {
        /* Inefficient search strategy, but it's easy */
        //z 查找，看buf中是否有待查找字符。
        if (strstr(buf, szSearchFor))
        {
            /* Make sure that this thread is the
             * only one writing to this handle */
            //z 向 standard out 写入
            EnterCriticalSection( &ScreenCritical );
            
            //z 写入文件名
            WriteFile( hConsoleOut,
                lpFindData->cFileName,
                strlen(lpFindData->cFileName),
                &dummy,
                FALSE );
            //z 
            WriteFile( hConsoleOut,
                ": ", 2, &dummy, FALSE );
            WriteFile( hConsoleOut,
                buf, strlen(buf), &dummy, FALSE );
            WriteFile( hConsoleOut,
                "\r\n", 2, &dummy, FALSE );

            LeaveCriticalSection( &ScreenCritical );
        }
    }
    
    //z 关闭文件
    CloseHandle(hFile);
    //z 释放分配的 heap 。
    HeapFree( GetProcessHeap(), 0, lpFindData);
    
    //z 释放 semaphore 。
    MTVERIFY( ReleaseSemaphore( hThreadLimitSemaphore,
                                    1,
                                    NULL ) );
}

/*
 * Unlike fgets(), this routine throws away CR/LF
 * automatically.  Calling ReadFile() one character
 * at a time is slow, but this illustrates the
 * advantages of using stdio under some conditions
 * (because buffering the stream yourself is difficult)
 */
BOOL GetLine(HANDLE hFile, LPSTR buf, DWORD size)
{
    DWORD total = 0;
    DWORD numread;

    int state = 0;  /* 0 = Looking for non-newline */
                    /* 1 = Stop after first newline */

    for (;;)
    {
        if (total == size-1)
        {
            buf[size-1] = '\0';
            return TRUE;
        }

        //z 使用readfile，每次读入一个字符串。
        if (!ReadFile(hFile, buf+total, 1, &numread, 0)
                || numread == 0)
        {
            buf[total] = '\0';
            return total != 0;
        }
        
        if (buf[total] == '\r' || buf[total] == '\n')
        {
            //z 在continue的情况下，total没有发生变化。如果此时为0，那么会继续读入到buf[0]
            //z 直到读入到第一个非回车换行符。
            if (state == 0)
                continue;
            buf[total] = '\0';
            return TRUE;
        }
        //z 如果第一个字符不是 \r 或者 \n，那么 state 会发生变化。
        //z 如果读取出来的字符串第一个字符是\r或者\n，那么继续。直到找到第一个非回车字符。
        //z 这样一次会读入若干个空白行？
        state = 1;
        total++;
    }
}
