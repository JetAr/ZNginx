/* Chapter 1. Basic cp file copy program.
	Windows Implementation using CopyFile for convenience and possible performance. */
/* cp file1 file2: Copy file1 to file2. */

//z 2015-03-23 17:30 直接使用 CopyFile，由其来操心和管理各种问题。

#include <windows.h>
#include <stdio.h>
#define BUF_SIZE 256

int main (int argc, LPTSTR argv [])
{
    if (argc != 3)
    {
        fprintf (stderr, "Usage: cp file1 file2\n");
        return 1;
    }
    if (!CopyFile (argv[1], argv[2], FALSE))
    {
        fprintf (stderr, "CopyFile Error: %x\n", GetLastError ());
        return 2;
    }
    return 0;
}
