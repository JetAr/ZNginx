//------------------------------------------------------------------------------
//           Name: dx9_view_ports.cpp
//         Author: Kevin Harris
//  Last Modified: 06/02/05
//    Description: This sample demonstrates how to set multiple view ports
//                 using DirectX 9.0.
//
//   Control Keys: Left Mouse Button - Spin the teapot
//------------------------------------------------------------------------------

#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "resource.h"

//------------------------------------------------------------------------------
// GLOBALS
//------------------------------------------------------------------------------
HWND              g_hWnd        = NULL;
LPDIRECT3D9       g_pD3D        = NULL;
LPDIRECT3DDEVICE9 g_pd3dDevice  = NULL;
LPD3DXMESH        g_pTeapotMesh = NULL;
D3DMATERIAL9      g_teapotMtrl;
D3DLIGHT9         g_pLight0;

DWORD g_dwBackBufferWidth  = 0;
DWORD g_dwBackBufferHeight = 0;

float g_fSpinX = 0.0f;
float g_fSpinY = 0.0f;

struct Vertex
{
    float x, y, z;    // Position of vertex in 3D space
    float nx, ny, nz; // Normal for lighting calculations
    DWORD diffuse;    // Diffuse color of vertex

	enum FVF
	{
		FVF_Flags = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE
	};
};

//------------------------------------------------------------------------------
// PROTOTYPES
//------------------------------------------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
				   LPSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
void init(void);
void shutDown(void);
void render(void);

//------------------------------------------------------------------------------
// Name: WinMain()
// Desc: The application's entry point
//------------------------------------------------------------------------------
int WINAPI WinMain(	HINSTANCE hInstance,
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
                             "Direct3D (DX9) - Setting Multiple View Ports",
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

//------------------------------------------------------------------------------
// Name: WindowProc()
// Desc: The window's message handler
//------------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND   hWnd,
							 UINT   msg,
							 WPARAM wParam,
							 LPARAM lParam )
{
	static POINT ptLastMousePosit;
	static POINT ptCurrentMousePosit;
	static bool bMousing;
	
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

		case WM_LBUTTONDOWN:
		{
			ptLastMousePosit.x = ptCurrentMousePosit.x = LOWORD (lParam);
            ptLastMousePosit.y = ptCurrentMousePosit.y = HIWORD (lParam);
			bMousing = true;
		}
		break;

		case WM_LBUTTONUP:
		{
			bMousing = false;
		}
		break;

		case WM_MOUSEMOVE:
		{
			ptCurrentMousePosit.x = LOWORD (lParam);
			ptCurrentMousePosit.y = HIWORD (lParam);

			if( bMousing )
			{
				g_fSpinX -= (ptCurrentMousePosit.x - ptLastMousePosit.x);
				g_fSpinY -= (ptCurrentMousePosit.y - ptLastMousePosit.y);
			}
			
			ptLastMousePosit.x = ptCurrentMousePosit.x;
            ptLastMousePosit.y = ptCurrentMousePosit.y;
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

    D3DDISPLAYMODE d3ddm;

    g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &d3ddm );

    D3DPRESENT_PARAMETERS d3dpp;
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
	
    d3dpp.Windowed               = TRUE;
    d3dpp.SwapEffect             = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat       = d3ddm.Format;
    d3dpp.EnableAutoDepthStencil = TRUE;
    d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
    d3dpp.PresentationInterval   = D3DPRESENT_INTERVAL_IMMEDIATE;

    g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
						  g_hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING,
						  &d3dpp, &g_pd3dDevice );

    D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DXToRadian( 45.0f ), 
                                640.0f / 480.0f, 0.1f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

    g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
    g_pd3dDevice->SetRenderState( D3DRS_SPECULARENABLE, TRUE );

    // Setup a simple directional light and some ambient...
	g_pLight0.Type = D3DLIGHT_DIRECTIONAL;
	g_pLight0.Direction = D3DXVECTOR3( 1.0f, 0.0f, 1.0f );

    g_pLight0.Diffuse.r = 1.0f;
    g_pLight0.Diffuse.g = 1.0f;
    g_pLight0.Diffuse.b = 1.0f;
	g_pLight0.Diffuse.a = 1.0f;

	g_pLight0.Specular.r = 1.0f;
	g_pLight0.Specular.g = 1.0f;
	g_pLight0.Specular.b = 1.0f;
	g_pLight0.Specular.a = 1.0f;

    g_pd3dDevice->SetLight( 0, &g_pLight0 );
    g_pd3dDevice->LightEnable( 0, TRUE );

    g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, D3DCOLOR_COLORVALUE( 0.2f, 0.2f, 0.2f, 1.0f ) );

    // Setup a material for the teapot
    ZeroMemory( &g_teapotMtrl, sizeof(D3DMATERIAL9) );

    g_teapotMtrl.Diffuse.r = 1.0f;
    g_teapotMtrl.Diffuse.g = 1.0f;
    g_teapotMtrl.Diffuse.b = 1.0f;
    g_teapotMtrl.Diffuse.a = 1.0f;

    // Load up the teapot mesh...
    D3DXLoadMeshFromX( "teapot.x", D3DXMESH_SYSTEMMEM, g_pd3dDevice, 
                       NULL, NULL, NULL, NULL, &g_pTeapotMesh );

    //
    // Cache the width & height of the back-buffer...
    //

    LPDIRECT3DSURFACE9 pBackBuffer = NULL;
    D3DSURFACE_DESC d3dsd;
    g_pd3dDevice->GetBackBuffer( 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer );
    pBackBuffer->GetDesc( &d3dsd );
    pBackBuffer->Release();
    g_dwBackBufferWidth  = d3dsd.Width;
    g_dwBackBufferHeight = d3dsd.Height;
}

//------------------------------------------------------------------------------
// Name: shutDown()
// Desc: 
//------------------------------------------------------------------------------
void shutDown( void )
{
    if( g_pTeapotMesh != NULL )
        g_pTeapotMesh->Release();

    if( g_pd3dDevice != NULL )
        g_pd3dDevice->Release();

    if( g_pD3D != NULL )
        g_pD3D->Release();
}

//------------------------------------------------------------------------------
// Name: render()
// Desc: 
//------------------------------------------------------------------------------
void render( void )
{
    D3DXMATRIX matView;
    D3DXMATRIX matWorld;
    D3DXMATRIX matRotation;
    D3DXMATRIX matTranslation;

    //
    // Render to the left viewport
    //

    D3DVIEWPORT9 leftViewPort;

    leftViewPort.X      = 0;
    leftViewPort.Y      = 0;
    leftViewPort.Width  = g_dwBackBufferWidth / 2;
    leftViewPort.Height = g_dwBackBufferHeight;
    leftViewPort.MinZ   = 0.0f;
    leftViewPort.MaxZ   = 1.0f;

    g_pd3dDevice->SetViewport( &leftViewPort );

    // Now we can clear just view-port's portion of the buffer to red...
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_COLORVALUE( 1.0f, 0.0f, 0.0f, 1.0f ), 1.0f, 0 );

    g_pd3dDevice->BeginScene();
    {
        // For the left view-port, leave the view at the origin...
        D3DXMatrixIdentity( &matView );
        g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

        // ... and use the world matrix to spin and translate the teapot  
        // out where we can see it...
        D3DXMatrixRotationYawPitchRoll( &matRotation, D3DXToRadian(g_fSpinX), D3DXToRadian(g_fSpinY), 0.0f );
        D3DXMatrixTranslation( &matTranslation, 0.0f, 0.0f, 5.0f );
        matWorld = matRotation * matTranslation;
        g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        g_pd3dDevice->SetMaterial( &g_teapotMtrl );
        g_pTeapotMesh->DrawSubset(0);
    }
    g_pd3dDevice->EndScene();
 
    //
    // Render to the right viewport
    //

    D3DVIEWPORT9 rightViewPort;

    rightViewPort.X      = g_dwBackBufferWidth / 2;
    rightViewPort.Y      = 0;
    rightViewPort.Width  = g_dwBackBufferWidth / 2;
    rightViewPort.Height = g_dwBackBufferHeight;
    rightViewPort.MinZ   = 0.0f;
    rightViewPort.MaxZ   = 1.0f;

    g_pd3dDevice->SetViewport( &rightViewPort );

    // Now we can clear just view-port's portion of the buffer to green...
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                         D3DCOLOR_COLORVALUE( 0.0f, 1.0f, 0.0f, 1.0f ), 1.0f, 0 );

    g_pd3dDevice->BeginScene();
    {
        // For the right view-port, translate and rotate the view around 
        // the teapot so we can see it...
        D3DXMatrixRotationYawPitchRoll( &matRotation, D3DXToRadian(g_fSpinX), D3DXToRadian(g_fSpinY), 0.0f );
        D3DXMatrixTranslation( &matTranslation, 0.0f, 0.0f, 5.0f );
        matView = matRotation * matTranslation;
        g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

        // ... and don't bother with the world matrix at all.
        D3DXMatrixIdentity( &matWorld );
        g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

        g_pd3dDevice->SetMaterial( &g_teapotMtrl );
        g_pTeapotMesh->DrawSubset(0);
    }
    g_pd3dDevice->EndScene();

    //
    // We're done! Now, we just call Present()
    //

    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}

