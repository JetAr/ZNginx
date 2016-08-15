//=============================================================================
// Desc: 使用顶点缓冲区绘制三角形
//=============================================================================

#include <d3d9.h>

//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL;    //Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    //Direct3D设备对象
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL;    //顶点缓冲区对象


//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)  //顶点格式


//-----------------------------------------------------------------------------
// Desc: 初始化Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	//创建Direct3D对象, 该对象用于创建Direct3D设备对象
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	//设置D3DPRESENT_PARAMETERS结构, 准备创建Direct3D设备对象
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	//查询当前显示模式
	D3DDISPLAYMODE d3ddm;
	if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT
		, &d3ddm ) ) )
		return E_FAIL;

	d3dpp.BackBufferWidth  = d3ddm.Width;
	d3dpp.BackBufferHeight = d3ddm.Height;
	d3dpp.BackBufferFormat = d3ddm.Format;

	//创建Direct3D设备对象
	if( FAILED( g_pD3D->CreateDevice( 
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 创建并填充顶点缓冲区
//-----------------------------------------------------------------------------
HRESULT InitVB()
{
	//顶点数据
	CUSTOMVERTEX vertices[] =
	{
		{ 100.0f,  650.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 500.0f,  100.0f, 0.5f, 1.0f, 0xff00ff00, }, 
		{ 900.0f,  650.0f, 0.5f, 1.0f, 0xff0000ff, },
	};

	//创建顶点缓冲区
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 
		3*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	//填充顶点缓冲区
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, vertices, sizeof(vertices) );
	g_pVB->Unlock();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 释放创建的对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//释放顶点缓冲区对象
	if( g_pVB != NULL )        
		g_pVB->Release();

	//释放Direct3D设备对象
	if( g_pd3dDevice != NULL ) 
		g_pd3dDevice->Release();

	//释放Direct3D对象
	if( g_pD3D != NULL )       
		g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: 渲染图形 
//-----------------------------------------------------------------------------
VOID Render()
{
	//清空后台缓冲区
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(45, 50, 170)
		, 1.0f, 0 );

	//开始在后台缓冲区绘制图形
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//在后台缓冲区绘制图形
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );

		//结束在后台缓冲区绘制图形
		g_pd3dDevice->EndScene();
	}

	//将在后台缓冲区绘制的图形提交到前台缓冲区显示
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Desc: 消息处理
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	switch( msg )
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage( 0 );
		return 0;

	case WM_KEYDOWN:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Desc: 入口函数
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	//注册窗口类
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
	RegisterClassEx( &wc );

	//创建窗口
	HWND hWnd = CreateWindow( L"ClassName", L"顶点缓冲区",
		WS_OVERLAPPEDWINDOW, 200, 100, 600, 500,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//初始化Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		//创建并填充顶点缓冲区
		if( SUCCEEDED( InitVB() ) )
		{
			//显示窗口
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

			//进入消息循环
			MSG msg;
			ZeroMemory( &msg, sizeof(msg) );
			while( msg.message!=WM_QUIT )
			{
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
				{
					Render();  //渲染图形
				}
			}
		}
	}

	UnregisterClass(L"ClassName", wc.hInstance);
	return 0;
}
