//-----------------------------------------------------------------------------
//           Name: dx9_fullscreen.cpp
//         Author: Kevin Harris
//  Last Modified: 05/27/05
//    Description: This sample demonstrates how to probe the hardware for 
//                 specific Display or Adapter Modes and hardware support 
//                 suitable for a full-screen application with Direct3D.
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

	if( RegisterClassEx(&winClass) == 0 )
		return E_FAIL;

	g_hWnd = CreateWindowEx( NULL, "MY_WINDOWS_CLASS", 
                             "Direct3D (DX9) - Full Screen",
							 WS_POPUP | WS_SYSMENU | WS_VISIBLE,
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
    g_pD3D = Direct3DCreate9( D3D_SDK_VERSION );

	if( g_pD3D == NULL )
	{
		// TO DO: Respond to failure of Direct3DCreate8
		return;
	}

	//
	// For the default adapter, examine all of its display modes to see if any 
	// of them can give us the hardware support we desire.
	//

	int nMode = 0;
	D3DDISPLAYMODE d3ddm;
	bool bDesiredAdapterModeFound = false;

	int nMaxAdapterModes = g_pD3D->GetAdapterModeCount( D3DADAPTER_DEFAULT, 
		                                                D3DFMT_X8R8G8B8 );

	for( nMode = 0; nMode < nMaxAdapterModes; ++nMode )
	{
		if( FAILED( g_pD3D->EnumAdapterModes( D3DADAPTER_DEFAULT, 
			                                  D3DFMT_X8R8G8B8, nMode, &d3ddm ) ) )
		{
			// TO DO: Respond to failure of EnumAdapterModes
			return;
		}

		// Does this adapter mode support a mode of 640 x 480?
        if( d3ddm.Width != 640 || d3ddm.Height != 480 )
            continue;

		// Does this adapter mode support a 32-bit RGB pixel format?
		if( d3ddm.Format != D3DFMT_X8R8G8B8 )
            continue;

		// Does this adapter mode support a refresh rate of 75 MHz?
		if( d3ddm.RefreshRate != 75 )
			continue;

		// We found a match!
		bDesiredAdapterModeFound = true;
		break;
	}

	if( bDesiredAdapterModeFound == false )
	{
		// TO DO: Handle lack of support for desired adapter mode...
		return;
	}

	//
	// Here's the manual way of verifying hardware support.
	//

	// Can we get a 32-bit back buffer?
	if( FAILED( g_pD3D->CheckDeviceType( D3DADAPTER_DEFAULT,
										 D3DDEVTYPE_HAL,
										 D3DFMT_X8R8G8B8,
										 D3DFMT_X8R8G8B8,
										 FALSE ) ) )
	{
		// TO DO: Handle lack of support for a 32-bit back buffer...
		return;
	}

	// Can we get a z-buffer that's at least 16 bits?
	if( FAILED( g_pD3D->CheckDeviceFormat( D3DADAPTER_DEFAULT,
                                           D3DDEVTYPE_HAL,
										   D3DFMT_X8R8G8B8,
                                           D3DUSAGE_DEPTHSTENCIL,
                                           D3DRTYPE_SURFACE,
                                           D3DFMT_D16 ) ) )
    {
		// TO DO: Handle lack of support for a 16-bit z-buffer...
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

	DWORD flags = 0;

	if( d3dCaps.VertexProcessingCaps != 0 )
		flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
		flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	//
	// Everything checks out - create a simple, full-screen device.
	//

	D3DPRESENT_PARAMETERS d3dpp;
	memset(&d3dpp, 0, sizeof(d3dpp));
	
	d3dpp.Windowed               = FALSE;
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferWidth        = 640;
    d3dpp.BackBufferHeight       = 480;
    d3dpp.BackBufferFormat       = D3DFMT_X8R8G8B8;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd,
									  flags, &d3dpp, &g_pd3dDevice ) ) )
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
