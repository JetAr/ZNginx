#ifdef WIN32

#include <windows.h>

#ifndef _GUTILWIN32_
#define _GUTILWIN32_

HWND GutGetWindowHandleWin32(void);
void GutSetWindowHandleWin32(HWND hWnd);
void GutCloseWindow(void);
HINSTANCE GutGetWindowInstanceWin32(void);
void GutGetWindowSize(int &w, int &h);

#endif // _GUTILWIN32_

#endif // WIN32
