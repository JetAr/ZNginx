// OgreWin32V1.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "OgreWin32V1.h"
#include <Ogre.h>

#include <OgreConfigFile.h>
using namespace Ogre;

#include "OgreWrap.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

Ogre::Root         *mRoot = 0;
BOOL InitOgre();
inline Ogre::Root *GetOgreRoot() {return mRoot;}

OgreWrap * ogreHandle = 0;


int APIENTRY _tWinMain(HINSTANCE hInstance,
					   HINSTANCE hPrevInstance,
					   LPTSTR    lpCmdLine,
					   int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: 在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_OGREWIN32V1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}


	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_OGREWIN32V1));

	// 主消息循环:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
//
//  注释:
//
//    仅当希望
//    此代码与添加到 Windows 95 中的“RegisterClassEx”
//    函数之前的 Win32 系统兼容时，才需要此函数及其用法。调用此函数十分重要，
//    这样应用程序就可以获得关联的
//    “格式正确的”小图标。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_OGREWIN32V1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_OGREWIN32V1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // 将实例句柄存储在全局变量中

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	int i  = 0 ;

	static BOOL firstrun = TRUE;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case ID_ROBOT_WALK:
		case ID_ROBOT_SHOOT:
		case ID_ROBOT_IDLE:
		case ID_ROBOT_SLUMP:
		case ID_ROBOT_DIE:
			{
				ogreHandle->SetRobotState( wmId );
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		{
			if (firstrun)
			{
				firstrun = FALSE;
				InitOgre();
				ogreHandle = new OgreWrap;
				ogreHandle->SetupOgre(hWnd);
				SetTimer(hWnd , 1 , 30, NULL);
			}
			else
			{
				GetOgreRoot()->renderOneFrame();
			}
			ValidateRect(hWnd , NULL);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_TIMER:
		{
			InvalidateRect(hWnd ,NULL , FALSE);
		}
		break;
	case WM_CREATE:
		{
		}
		break;
	case  WM_SIZE:
		{
			if (ogreHandle)
			{
				ogreHandle->OnSize();
			}
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL InitOgre()
{
#ifdef _DEBUG
	mRoot = new Ogre::Root("plugins_d.cfg", "OgreMFC.cfg", "OgreMFC.log"); 
#else
	mRoot = new Ogre::Root("plugins.cfg", "OgreMFC.cfg", "OgreMFC.log"); 
#endif

	//
	// Setup paths to all resources
	//

	Ogre::ConfigFile cf;
	cf.load("resources_d.cfg");

	// Go through all sections & settings in the file
	ConfigFile::SectionIterator seci = cf.getSectionIterator();

	String secName, typeName, archName;
	while (seci.hasMoreElements())
	{
		secName = seci.peekNextKey();
		ConfigFile::SettingsMultiMap *settings = seci.getNext();
		ConfigFile::SettingsMultiMap::iterator i;
		for (i = settings->begin(); i != settings->end(); ++i)
		{
			typeName = i->first;
			archName = i->second;
			ResourceGroupManager::getSingleton().addResourceLocation( archName, typeName, secName);
		}
	}

	const RenderSystemList& render =  mRoot->getAvailableRenderers();

	RenderSystemList::const_iterator pRend = render.begin();

	while (pRend != render.end())
	{
		Ogre::String rName = (*pRend)->getName();
		//if (rName == "OpenGL Rendering Subsystem") //
		if (rName == "Direct3D9 Rendering Subsystem") //Direct3D9 Rendering Subsystem
			break;
		pRend++;
	}

	if (pRend == render.end())
	{
		// Unrecognised render system
		//MessageBox("Unable to locate OpenGL rendering system.  Application is terminating");
		return FALSE;
	}

	Ogre::RenderSystem *rsys = *pRend;
	rsys->setConfigOption("Full Screen", "No");
	rsys->setConfigOption("VSync", "Yes");

	// Set the rendering system.
	mRoot->setRenderSystem(rsys);

	//
	// Initialize the system, but don't create a render window.
	//
	mRoot->initialise(false);

	return TRUE;
}