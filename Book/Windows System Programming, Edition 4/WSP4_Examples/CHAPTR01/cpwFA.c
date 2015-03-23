/* Chapter 1. Basic cp file copy program. Win32 Implementation.
	FAST VERSION USING
		1.  LARGE buffer
		2.  PRE-SET OUTPUT FILE SIZE
		3.  SEQUENTIAL SCAN */
/* cp file1 file2: Copy file1 to file2. */

//z 2015-03-23 17:24 使用了一些更为高级的特性使得拷贝起来更快
/*
    1. 大缓冲
    2. 预置的输出文件size ？？ 这点是如何体现的了？
    3. 顺序扫描
*/

#include <windows.h>
#include <stdio.h>
#define BUF_SIZE 8192 //z 定义了一个8K的缓冲区

int main (int argc, LPTSTR argv [])
{
    HANDLE hIn, hOut;
    DWORD nIn, nOut;
    CHAR buffer [BUF_SIZE];
    if (argc != 3)
    {
        fprintf (stderr, "Usage: cp file1 file2\n");
        return 1;
    }
    //z 打开文件的时候，使用了 FILE_FLAG_SEQUENTIAL_SCAN
    hIn = CreateFile (argv [1], GENERIC_READ, 0, NULL, OPEN_EXISTING,
                      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hIn == INVALID_HANDLE_VALUE)
    {
        fprintf (stderr, "Cannot open input file. Error: %x\n", GetLastError ());
        return 2;
    }
    hOut = CreateFile (argv [2], GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hOut == INVALID_HANDLE_VALUE)
    {
        fprintf (stderr, "Cannot open output file. Error: %x\n", GetLastError ());
        CloseHandle(hIn);
        return 3;
    }

    /*  Set the output file size. */
    //z 每次读入8k数据
    while (ReadFile (hIn, buffer, BUF_SIZE, &nIn, NULL) && nIn > 0)
    {
        WriteFile (hOut, buffer, nIn, &nOut, NULL);
        if (nIn != nOut)
        {
            fprintf (stderr, "Fatal write error: %x\n", GetLastError ());
            CloseHandle(hIn);
            CloseHandle(hOut);
            return 4;
        }
    }
    CloseHandle (hIn);
    CloseHandle (hOut);
    return 0;
}
