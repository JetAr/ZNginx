//-----------------------------------------------------------------------------
//           Name: dx9_initialization.cpp
//         Author: Kevin Harris
//  Last Modified: 05/27/05
//    Description: This sample demonstrates how to initialize Direct3D
//-----------------------------------------------------------------------------

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "resource.h"

//-----------------------------------------------------------------------------
// GLOBALS
//-----------------------------------------------------------------------------
HWND              g_hWnd       = NULL;
LPDIRECT3D9       g_pD3D       = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice = NULL;

//-----------------------------------------------------------------------------
// PROTOTYPES
//-----------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void init(void);
void shutDown(void);
void render(void);

//-----------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE hInstance,
                    HINSTANCE hPrevInstance,
                    LPSTR     lpCmdLine,
                    int       nCmdShow )
{
	WNDCLASSEX winClass;
	MSG        uMsg;

    memset(&uMsg,0,sizeof(uMsg));

	winClass.lpszClassName = "MY_WINDOWS_CLASS";
	winClass.cbSize        = sizeof(WNDCLASSEX);
	winClass.style         = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc   = WindowProc;
	winClass.hInstance     = hInstance;
	winClass.hIcon	       = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
    winClass.hIconSm	   = LoadIcon(hInstance, (LPCTSTR)IDI_DIRECTX_ICON);
	winClass.hCursor       = LoadCursor(NULL, IDC_ARROW);
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.lpszMenuName  = NULL;
	winClass.cbClsExtra    = 0;
	winClass.cbWndExtra    = 0;

	if( !RegisterClassEx(&winClass) )
		return E_FAIL;

	g_hWnd = CreateWindowEx( NULL, "MY_WINDOWS_CLASS", 
                             "Direct3D (DX9) - Initialization",
						     WS_OVERLAPPEDWINDOW | WS_VISIBLE,
					         0, 0, 640, 480, NULL, NULL, hInstance, NULL );

	if( g_hWnd == NULL )
		return E_FAIL;

    ShowWindow( g_hWnd, nCmdShow );
    UpdateWindow( g_hWnd );

	init();

	while( uMsg.message != WM_QUIT )
	{
		if( PeekMessage( &uMsg, NULL, 0, 0, PM_REMOVE ) )
		{ 
			TranslateMessage( &uMsg );
			DispatchMessage( &uMsg );
		}
        else
		    render();
	}

	shutDown();

    UnregisterClass( "MY_WINDOWS_CLASS", winClass.hInstance );

	return uMsg.wParam;
}

//-----------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND   hWnd, 
							 UINT   msg, 
							 WPARAM wParam, 
							 LPARAM lParam )
{
    switch( msg )
	{
        case WM_KEYDOWN:
		{
			switch( wParam )
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;
			}
		}
        break;

		case WM_CLOSE:
		{
			PostQuitMessage(0);
		}
		
        case WM_DESTROY:
		{
            PostQuitMessage(0);
		}
        break;

		default:
		{
			return DefWindowProc( hWnd, msg, wParam, lParam );
		}
		break;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Name: init()
// Desc: 
//-----------------------------------------------------------------------------
void init( void )
{
    //z 创建一个 IDirect3D9 object
    g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	if( g_pD3D == NULL )
	{
		// TO DO: Respond to failure of Direct3DCreate8
		return;
	}

    D3DDISPLAYMODE d3ddm;

    //z 得到当前适配器的显示模式
    //z D3DADAPTER_DEFAULT 表示主适配器
    if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm ) ) )
	{
		// TO DO: Respond to failure of GetAdapterDisplayMode
		return;
	}

	HRESULT hr;

	if( FAILED( hr = g_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
												d3ddm.Format, D3DUSAGE_DEPTHSTENCIL,
												D3DRTYPE_SURFACE, D3DFMT_D16 ) ) )
	{
		if( hr == D3DERR_NOTAVAILABLE )
			// POTENTIAL PROBLEM: We need at least a 16-bit z-buffer!
			return;
	}

	//
	// Do we support hardware vertex processing? if so, use it. 
	// If not, downgrade to software.
	//

	D3DCAPS9 d3dCaps;

	if( FAILED( g_pD3D->GetDeviceCaps( D3DADAPTER_DEFAULT, 
		                               D3DDEVTYPE_HAL, &d3dCaps ) ) )
	{
		// TO DO: Respond to failure of GetDeviceCaps
		return;
	}

	DWORD dwBehaviorFlags = 0;

	if( d3dCaps.VertexProcessingCaps != 0 )
		dwBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		dwBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//
	// Everything checks out - create a simple, windowed device.
	//

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));

    d3dpp.BackBufferFormat       = d3ddm.Format;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.Windowed               = TRUE;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
                                      dwBehaviorFlags, &d3dpp, &g_pd3dDevice ) ) )
	{
		// TO DO: Respond to failure of CreateDevice
		return;
	}
}

//-----------------------------------------------------------------------------
// Name: shutDown()
// Desc: 
//-----------------------------------------------------------------------------
void shutDown( void )
{
    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}

//-----------------------------------------------------------------------------
// Name: render()
// Desc: 
//-----------------------------------------------------------------------------
void render( void )
{
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_COLORVALUE(0.0f,1.0f,0.0f,1.0f), 1.0f, 0 );

    g_pd3dDevice->BeginScene();

	// Render geometry here...

    g_pd3dDevice->EndScene();

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}
