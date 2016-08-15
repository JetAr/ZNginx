#include "ElSamplePCH.h"
#include "commdlg.h"
#include "ElSampleApplication.h"
#include "GUIHelper.h"

ElSampleApplication theApp;
const int width = 1024;
const int height = 768;

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
    static TCHAR szAppName[] = TEXT ("ElApplication");
    HWND         hwnd;
    MSG          msg;
    WNDCLASSEX   wndclassex = {0};
    wndclassex.cbSize        = sizeof(WNDCLASSEX);
    wndclassex.style         = CS_HREDRAW | CS_VREDRAW;
    wndclassex.lpfnWndProc   = WndProc;
    wndclassex.cbClsExtra    = 0;
    wndclassex.cbWndExtra    = 0;
    wndclassex.hInstance     = hInstance;
    wndclassex.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
    wndclassex.hCursor       = LoadCursor (NULL, IDC_ARROW);
    wndclassex.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
    wndclassex.lpszMenuName  = NULL;
    wndclassex.lpszClassName = szAppName;
    wndclassex.hIconSm       = wndclassex.hIcon;
	
    if (!RegisterClassEx (&wndclassex))
    {
        MessageBox (NULL, TEXT ("RegisterClassEx failed!"), szAppName, MB_ICONERROR);
        return 0;
    }

    hwnd = CreateWindowEx (WS_EX_OVERLAPPEDWINDOW, 
		                  szAppName, 
        		          TEXT ("ElSample"),
                		  WS_OVERLAPPEDWINDOW,
		                  50, 
        		          50, 
                		  width, 
		                  height, 
        		          NULL, 
                		  NULL, 
		                  hInstance,
        		          NULL);
						  
    ShowWindow (hwnd, iCmdShow);
    UpdateWindow (hwnd);

	if (!theApp.initialize(hwnd))
		return 0;
	
	ZeroMemory (&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
			theApp.process();
	}

	theApp.terminate();

    return msg.wParam;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;

    switch (message)
    {
	case WM_CREATE:
		{
			// center the window
			RECT rect;
			GetWindowRect(hwnd, &rect);

			int nWndWidth = rect.right - rect.left;
			int nWndHeight = rect.bottom - rect.top;
			
			int nScreenWidth = GetSystemMetrics(SM_CXSCREEN);
			int nScreenHeight = GetSystemMetrics(SM_CYSCREEN);

			rect.left = (nScreenWidth - nWndWidth) / 2;
			rect.top = (nScreenHeight - nWndHeight) / 2;

			SetWindowPos(hwnd, HWND_TOP, rect.left, rect.top, nWndWidth, nWndHeight, SWP_SHOWWINDOW);
		}
		break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
	case WM_KEYDOWN:
		{
			ELGUI::System::getSingleton().injectKeyDown(ElGUIHelper::Virtualkey2scancode(wParam, lParam));

			switch (wParam)
			{
			case VK_UP:
				theApp.getCamera()->walk(0.5f);
				break;
			case VK_DOWN:
				theApp.getCamera()->walk(-0.5f);
				break;
			case VK_LEFT:
				theApp.getCamera()->strafe(-0.5f);
				break;
			case VK_RIGHT:
				theApp.getCamera()->strafe(0.5f);
				break;
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
			}
		}
		break;
	case WM_KEYUP:
		{
			ELGUI::System::getSingleton().injectKeyUp(ElGUIHelper::Virtualkey2scancode(wParam, lParam));

			switch(wParam)
			{
			case VK_SNAPSHOT:
				theApp.saveScreen();
				break;
			}
		}
		break;
	case WM_SYSKEYDOWN:
		{
			ELGUI::System::getSingleton().injectKeyDown(ElGUIHelper::Virtualkey2scancode(wParam, lParam));
		}
		break;
	case WM_SYSKEYUP:
		{
			ELGUI::System::getSingleton().injectKeyUp(ElGUIHelper::Virtualkey2scancode(wParam, lParam));
		}
		break;
	case WM_MOUSEMOVE:
		{
			int x_pos = LOWORD(lParam);
			int y_pos = HIWORD(lParam);
			
			// move the camera
			static int x = 0;
			static int y = 0;
			if (wParam == MK_RBUTTON)
			{
				int delta_x = x - x_pos;
				int delta_y = y - y_pos;

				theApp.getCamera()->yaw(-D3DX_PI * ((float)delta_x / (float)width));
				theApp.getCamera()->pitch(-D3DX_PI* ((float)delta_y / (float)height));
			}
			x = x_pos;
			y = y_pos;

			ELGUI::System::getSingleton().injectMousePosition((float)x_pos, (float)y_pos);

			return 0;
		}
		break;
	case WM_LBUTTONDOWN:
		{
			ELGUI::System::getSingleton().injectMouseButtonDown(ELGUI::MouseButton::LeftButton);
		}
		break;
	case WM_LBUTTONUP:
		{
			ELGUI::System::getSingleton().injectMouseButtonUp(ELGUI::MouseButton::LeftButton);
		}
		break;
	case WM_RBUTTONDOWN:
		{
			ELGUI::System::getSingleton().injectMouseButtonDown(ELGUI::MouseButton::RightButton);
		}
		break;
	case WM_RBUTTONUP:
		{
			ELGUI::System::getSingleton().injectMouseButtonUp(ELGUI::MouseButton::RightButton);
		}
		break;
	case WM_MBUTTONDOWN:
		{
			ELGUI::System::getSingleton().injectMouseButtonDown(ELGUI::MouseButton::MiddleButton);
		}
		break;
	case WM_MBUTTONUP:
		{
			ELGUI::System::getSingleton().injectMouseButtonUp(ELGUI::MouseButton::MiddleButton);
		}
		break;
	case WM_MOUSEWHEEL:
		{
			ELGUI::System::getSingleton().injectMouseWheelChange((float)HIWORD(wParam));
		}
	case WM_CHAR:
		switch (wParam)
		{
		case ']':
			theApp.getCamera()->fly(2.0f);
			break;
		case '[':
			theApp.getCamera()->fly(-2.0f);
			break;
		case 'w':
			theApp.getCamera()->walk(2.0f);
			break;
		case 's':
			theApp.getCamera()->walk(-2.0f);
			break;
		case 'a':
			theApp.getCamera()->strafe(-2.0f);
			break;
		case 'd':
			theApp.getCamera()->strafe(2.0f);
			break;
		case '=':
			theApp.getScene()->getTerrain()->increaseLodFactor1();
			break;
		case '-':
			theApp.getScene()->getTerrain()->decreaseLodFactor1();
			break;
		case '0':
			theApp.getScene()->getTerrain()->increaseLodFactor2();
			break;
		case '9':
			theApp.getScene()->getTerrain()->decreaseLodFactor2();
			break;
		case 'h':
			theApp.toggleFillMode();
			break;
		case 'c':
			theApp.toggleCamera();
			break;
		}
		break;
    }

    return DefWindowProc(hwnd, message, wParam, lParam);
}