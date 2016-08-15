//=============================================================================
// Desc: 坐标变换
//=============================================================================

#include <d3dx9.h>  //D3DX实用库函数, 该头文件中又包含了d3d9.h头文件


//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9                g_pD3D       = NULL;    //Direct3D对象
LPDIRECT3DDEVICE9          g_pd3dDevice = NULL;    //Direct3D设备对象
LPDIRECT3DVERTEXBUFFER9    g_pVB        = NULL;    //顶点缓冲区对象
HWND					   g_Wnd	    = NULL;    //窗口句柄
D3DRECT                    g_ClentRect;


//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_DIFFUSE)


//-----------------------------------------------------------------------------
// Desc: 设置世界矩阵
//-----------------------------------------------------------------------------
VOID SetupWorldMatrice()
{
	//建立一个绕X轴动态旋转的世界矩阵
	D3DXMATRIX matWorld;
	UINT  iTime  = timeGetTime() % 1000;
	FLOAT fAngle = iTime * (2.0f * D3DX_PI) / 1000.0f;
	D3DXMatrixIdentity( &matWorld );
	D3DXMatrixRotationX( &matWorld, fAngle );

	//设置世界矩阵
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}


//-----------------------------------------------------------------------------
// Desc: 设置观察矩阵和投影矩阵
//-----------------------------------------------------------------------------
VOID SetupViewandProjMatrices()
{
	//建立并设置观察矩阵
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//建立并设置投影矩阵
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//-----------------------------------------------------------------------------
// Desc: 设置视区
//-----------------------------------------------------------------------------
VOID SetupViewPort()
{
	RECT rect;
	GetClientRect(g_Wnd,&rect);
	D3DVIEWPORT9 vp;
	vp.X      = 0;
	vp.Y      = 0;
	vp.Width  = rect.right;
	vp.Height = rect.bottom;
	vp.MinZ   = 0.0f;
	vp.MaxZ   = 1.0f;
	g_pd3dDevice->SetViewport(&vp);
}


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
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//创建Direct3D设备对象
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	//设置剔出模式为不剔出任何面(正面和反面)
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	//关闭光照处理, 默认情况下启用光照处理
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	//设置观察和投影矩阵
	SetupViewandProjMatrices();

	//设置视区
	SetupViewPort();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 创建场景图形
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	//创顶点缓冲区
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	//填充顶点缓冲区
	CUSTOMVERTEX* pVertices;
	if( FAILED( g_pVB->Lock( 0, 0, (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	for( DWORD i=0; i<50; i++ )
	{
		FLOAT theta = (2*D3DX_PI*i)/(50-1);
		pVertices[2*i+0].position = D3DXVECTOR3( sinf(theta),-1.0f, cosf(theta) );
		pVertices[2*i+0].color   =0xffffff00;
		pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
		pVertices[2*i+1].color   =0xffffff00;
	}
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
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0 );

	//开始在后台缓冲区绘制图形
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//设置世界矩阵
		SetupWorldMatrice();

		//在后台缓冲区绘制图形
		static bool first = true;
		if (first)
		{
			g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
			g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
			first = false;
		}
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

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
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Desc: 入口函数
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	//注册窗口类
	WNDCLASSEX wc = { 
		sizeof(WNDCLASSEX), CS_CLASSDC , MsgProc, 0L, 0L, 
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
		RegisterClassEx( &wc );

		//创建窗口
		HWND hWnd = CreateWindow( 
			L"ClassName", L"坐标变换",
			WS_OVERLAPPEDWINDOW, 200, 100, 500,500,
			GetDesktopWindow(), NULL, wc.hInstance, NULL );
		g_Wnd=hWnd;

		//初始化Direct3D
		if( SUCCEEDED( InitD3D( hWnd ) ) )
		{
			//创建场景图形
			if( SUCCEEDED( InitGeometry() ) )
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
						TranslateMessage( &msg );
						DispatchMessage( &msg );
					}
					else
					{
						Render();  //渲染图形
					}
				}
			}
		}

		UnregisterClass( L"ClassName", wc.hInstance );
		return 0;
}
