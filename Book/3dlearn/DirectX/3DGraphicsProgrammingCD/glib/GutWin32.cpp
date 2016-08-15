
#include <windows.h>

#include "Gut.h"
#include "GUtWin32.h"

static HWND g_hWnd = NULL;
static HINSTANCE g_hInstance = NULL;
static HANDLE g_ThreadHandle = NULL;
static unsigned long g_ThreadID = 0;

static int g_iWindowPosX = 100;
static int g_iWindowPosY = 100;
static int g_iWindowWidth = 512;
static int g_iWindowHeight = 512;

void GutSetWindowHandleWin32(HWND hWnd)
{
	g_hWnd = hWnd;
}

HWND GutGetWindowHandleWin32(void)
{
	return g_hWnd;
}

HINSTANCE GutGetWindowInstanceWin32(void)
{
	return g_hInstance;
}

void GutGetWindowSize(int &w, int &h)
{
	w = g_iWindowWidth;
	h = g_iWindowHeight;
}

void GutGetMouseState(int &x, int &y, int button[3])
{
	POINT p;

	GetCursorPos(&p);
	ScreenToClient(g_hWnd, &p);

	x = p.x;
	y = p.y;

	button[0] = GetKeyState(VK_LBUTTON) & 0x80 ? 1 : 0;
	button[1] = GetKeyState(VK_MBUTTON) & 0x80 ? 1 : 0;
	button[2] = GetKeyState(VK_RBUTTON) & 0x80 ? 1 : 0;
}

// 視窗訊息處理函式
static LRESULT WINAPI WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// 檢查訊息種類
	switch (message)
	{
	case WM_DESTROY:
		{
			// 如果視窗被關掉了, 就主動發出程式該結束的訊息.
			PostQuitMessage(0);
			break;
		}
	case WM_CLOSE:
		{
			// 如果視窗被關掉了, 就主動發出程式該結束的訊息.
			PostQuitMessage(0);
			break;
		}

	case WM_SIZE: // 視窗大小被改變
		{
			int w = LOWORD(lParam);
			int h = HIWORD(lParam);
			g_iWindowWidth = w;
			g_iWindowHeight = h;
			if ( g_GutCallBack.OnSize && GutGetGraphicsDeviceType()!=GUT_UNKNOWN ) 
			{
				// 有設定視窗大小改變的處理函式的話, 就去呼叫它.
				g_GutCallBack.OnSize(w, h);
			}
			break;
		}

	case WM_PAINT: // 視窗需要重畫
		{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			if ( g_GutCallBack.OnPaint )
			{
				// 有設定重繪訊息處理函式的話, 就去呼叫它.
				g_GutCallBack.OnPaint();
			}
			break;
		}

	default: // 其它訊息都直接交給內建的訊息處理函式去做
		{
			return DefWindowProc(hwnd, message, wParam, lParam);
			break;
		}
	}

	return 0;
} // WndProc

void GutCloseWindow(void)
{
	PostQuitMessage(0);
}

// 檢查Windows作業系統傳來的訊息
bool GutProcessMessage(void)
{
	MSG	msg;
	// 看看有沒有任何訊息
	if ( PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) )
	{
		// 有訊息的話, 就把它送給訊息處理函式.
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		if ( msg.message==WM_QUIT )
		{
			// 收到關閉視窗的訊息會傳回false來結束回圈.
			return false;
		}
	}

	return true;
}

bool GutFullScreen(int width, int height)
{
	char *title = "fullscreen";

	DEVMODE dmScreenSettings;								// Device Mode
	memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
	dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
	dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
	dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
	dmScreenSettings.dmBitsPerPel	= 32;					// Selected Bits Per Pixel
	dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

	// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
	if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
		// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
		return false;
	}


	WNDCLASS	window_class;

	memset(&window_class, 0, sizeof(WNDCLASS));

	// 設定視窗型態
	window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	// 設定訊息處理函式為WndProc
	window_class.lpfnWndProc = WndProc;
	// 取得目前執行的程式
	window_class.hInstance = GetModuleHandle(NULL);
	// 使用內定的滑鼠游標
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	// 背景設為黑色, 其實有沒有設都沒差.
	window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	// 給定一個注冊視窗時使用的名字
	window_class.lpszClassName = title;
	// 跟作業系統注冊一個新的視窗
	if (RegisterClass(&window_class) == 0)
		return 0;

	DWORD dwExStyle=WS_EX_APPWINDOW;	// Window Extended Style
	DWORD dwStyle=WS_POPUP;				// Windows Style
	ShowCursor(FALSE);					// Hide Mouse Pointer

	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values

	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	g_iWindowPosX = g_iWindowPosY = 0;
	g_iWindowWidth = width;
	g_iWindowHeight = height;

	HWND window_handle = CreateWindowEx(	
		dwExStyle,
		"fullscreen",
		title,
		dwStyle |
		WS_CLIPSIBLINGS |
		WS_CLIPCHILDREN,
		0, 0,
		WindowRect.right-WindowRect.left,
		WindowRect.bottom-WindowRect.top,
		NULL,
		NULL,
		window_class.hInstance,
		NULL
		);

	if (window_handle == NULL)
		return false;

	// 記錄下用來代表視窗跟目前執行程式的物件, 以後可以再用到.
	g_hWnd = window_handle;
	g_hInstance = window_class.hInstance;
	// 顯示視窗
	ShowWindow(window_handle, SW_SHOWNORMAL);
	SetActiveWindow(window_handle);
	SetForegroundWindow(window_handle);	
	SetFocus(window_handle);

	return true;
}


// 開啟視窗
// x, y = 視窗左上角的位置
//width, height = 視窗大小
//title = 視窗標題欄所顯示的文字
bool GutCreateWindow(int x, int y, int width, int height, const char *title)
{
	static bool registered = false;

	WNDCLASS	window_class;

	memset(&window_class, 0, sizeof(WNDCLASS));

	//設定視窗型態
	window_class.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	// 設定訊息處理函式為WndProc
	window_class.lpfnWndProc = WndProc;
	// 取得目前執行的程式
	window_class.hInstance = GetModuleHandle(NULL);
	// 使用內定的滑鼠游標
	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
	// 背景設為黑色, 其實有沒有設都沒差.
	window_class.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	// 給定一個注冊視窗時使用的名字
	window_class.lpszClassName = title;
	// 跟作業系統注冊一個新的視窗
	if ( !registered )
	{
		if (RegisterClass(&window_class) == 0)
			return false;
		registered = true;
	}

	DWORD window_style;

	if ( g_GutCallBack.OnSize )
	{
		// 如果有處理視窗縮放的訊息, 才容許滑鼠改變視窗大小.
		window_style = WS_OVERLAPPEDWINDOW;
	}
	else
	{
		// 如果有沒有處理視窗縮放的訊息, 不容許滑鼠改變視窗大小.
		window_style = WS_BORDER | WS_SYSMENU;
	}

	if ( x<0 || y<0 )
	{
		// 套用內定值
		x = g_iWindowPosX;
		y = g_iWindowPosY;
	}

	if ( width<0 || height < 0 )
	{
		// 套用內定值
		width = g_iWindowWidth;
		height = g_iWindowHeight;
	}

	// 計算視窗的邊線會占掉的點數, 必要時要增加視窗大小.
	RECT window_rect;
	SetRect(&window_rect, x, y, x+width, y+height);
	AdjustWindowRect(&window_rect, window_style, FALSE);

	g_iWindowPosX = x;
	g_iWindowPosY = y;
	g_iWindowWidth = width;
	g_iWindowHeight = height;

	// 準備工作完成, 開啟一個視窗.
	HWND window_handle = CreateWindowEx(
		WS_EX_APPWINDOW,
		title,
		title,
		window_style,
		window_rect.left,	// x
		window_rect.top,	// y
		window_rect.right - window_rect.left,	// width
		window_rect.bottom - window_rect.top,	// height
		NULL,
		NULL,
		window_class.hInstance,
		NULL
		);

	if (window_handle == NULL)
		return false;

	// 記錄下用來代表視窗跟目前執行程式的物件, 以後可以再用到.
	g_hWnd = window_handle;
	g_hInstance = window_class.hInstance;
	// 顯示視窗
	ShowWindow(window_handle, SW_SHOWNORMAL);
	SetActiveWindow(window_handle);

	return true;
}
