//=============================================================================
// Desc: 图形反锯齿
//=============================================================================

#include <d3d9.h>


//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL;    //Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    //Direct3D设备对象
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL;    //顶点缓冲区对象
bool                    antiAlisa    = true;    //标志是否进行反锯齿

//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


//-----------------------------------------------------------------------------
// Desc: 初始化Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	HRESULT hr = S_OK;

	//创建Direct3D对象, 该对象用来创建Direct3D设备对象
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	//设置D3DPRESENT_PARAMETERS结构, 准备创建Direct3D设备对象
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed         = TRUE;
	d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.MultiSampleType  = D3DMULTISAMPLE_4_SAMPLES;

	//创建Direct3D设备对象
	if(FAILED(g_pD3D->CheckDeviceMultiSampleType (D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL
		,D3DFMT_X8R8G8B8,FALSE,D3DMULTISAMPLE_4_SAMPLES,NULL)))
	{
		MessageBox(hWnd, L"硬件不支持多采样反锯齿!\n采用参考设备!"
			, L"AntiAlisa", 0);
		hr = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &g_pd3dDevice );
	}
	else
	{
		hr = g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&d3dpp, &g_pd3dDevice );
	}

	//在此设置整个程序运行期间不变换的渲染状态

	return hr;
}

//-----------------------------------------------------------------------------
// Desc: 创建并填充顶点缓冲区
//-----------------------------------------------------------------------------
HRESULT InitVB()
{
	//顶点数据
	CUSTOMVERTEX vertices[] =
	{
		{  50.0f, 250.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 150.0f,  50.0f, 0.5f, 1.0f, 0xff00ff00, }, 
		{ 250.0f, 250.0f, 0.5f, 1.0f, 0xff0000ff, },

	};

	//创建顶点缓冲区
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 3*sizeof(CUSTOMVERTEX),
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
	//设置是否启用图形反锯齿
	if(antiAlisa)
		g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,TRUE);
	else
		g_pd3dDevice->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS,FALSE);

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

		//结束在后台缓冲区渲染图形
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

	case WM_LBUTTONDOWN:   //单击鼠标左键切换是否采用图形反锯齿
		antiAlisa = !antiAlisa;
		return 0;
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
	HWND hWnd = CreateWindow( L"ClassName", L"图形反锯齿",
		WS_OVERLAPPEDWINDOW, 200, 100, 300, 300,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//初始化Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		//创建并填充顶点缓冲区
		if( SUCCEEDED( InitVB() ) )
		{
			//显示窗口(最大化)
			ShowWindow( hWnd, SW_SHOWMAXIMIZED );
			UpdateWindow( hWnd );

			//进入消息循环
			MSG msg;
			ZeroMemory( &msg, sizeof(msg) );
			while( msg.message!=WM_QUIT )
			{
				if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
				{
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				{
					Render(); //渲染图形
				}
			}
		}
	}

	UnregisterClass( L"ClassName", wc.hInstance );
	return 0;
}
