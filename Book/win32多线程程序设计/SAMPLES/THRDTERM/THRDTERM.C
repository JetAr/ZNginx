/*
 * ThrdTerm.c
 *
 * Sample code for "Multithreading Applications in Win32"
 * This is from Chapter 5, Listing 5-1
 *
 * Demonstrates how to request threads to exit.
 *
 * Build command: cl /MD ThrdTerm.c
 */

//z 2015-04-14 11:11 如何请求线程结束。

#define WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <time.h>
#include "MtVerify.h"

DWORD WINAPI ThreadFunc(LPVOID);

HANDLE hRequestExitEvent = FALSE;

int main()
{
    HANDLE hThreads[2];
    DWORD dwThreadId;
    DWORD dwExitCode = 0;
    int i;
    
    //z 手动事件；初始状态为 non-signaled
    hRequestExitEvent = CreateEvent(
        NULL, TRUE, FALSE, NULL);

    //z 创建两个线程
    for (i=0; i<2; i++)
        MTVERIFY( hThreads[i] = CreateThread(NULL,
            0,
            ThreadFunc,
            (LPVOID)i,
            0,
            &dwThreadId )
        );

    // Wait around for awhile, make
    // sure the thread is running.
    //z 休眠，确保线程开始运行
    Sleep(1000);

    //z 设置线程结束事件
    SetEvent(hRequestExitEvent);
    //z 等待线程结束
    WaitForMultipleObjects(2, hThreads, TRUE, INFINITE);

    for (i=0; i<2; i++)
        MTVERIFY( CloseHandle(hThreads[i]) );

    return EXIT_SUCCESS;
}

//z 有各种方式来控制线程的交互。
DWORD WINAPI ThreadFunc(LPVOID p)
{
    int i;
    int inside = 0;

    UNREFERENCED_PARAMETER(p);

    /* Seed the random-number generator */
    srand( (unsigned)time( NULL ) );

    for (i=0; i<1000000; i++)
    {
        double x = (double)(rand())/RAND_MAX;
        double y = (double)(rand())/RAND_MAX;
        if ( (x*x + y*y) <= 1.0 )
            inside++;
        //z 使用事件来
        //z If dwMilliseconds is zero, the function does not enter a wait state if the object is not signaled; it always returns immediately. 
        //z WAIT_TIMEOUT The time-out interval elapsed, and the object's state is nonsignaled.
        //z WAIT_OBJECT_0 The state of the specified object is signaled.
        //z 这里的具体的情况：如果等待时间为0，那么迅速返回。由于event处于non-signaled状态，返回WAIT_TIMEOUT
        //z 直到 event 被 reset ，返回 WAIT_OBJECT_0，那么线程也将直接返回。
        if (WaitForSingleObject(hRequestExitEvent, 0) != WAIT_TIMEOUT)
        {
            printf("Received request to terminate\n");
            return (DWORD)-1;
        }
    }
    printf("PI = %.4g\n", (double)inside / i * 4);
    return 0;
}
