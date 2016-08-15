//=============================================================================
// Desc: 索引缓冲区
//=============================================================================

#include <d3d9.h>
#include <math.h>


//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; // Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Direct3D设备对象
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // 顶点缓冲区对象
LPDIRECT3DINDEXBUFFER9  g_pIB		 = NULL; // 索引缓冲区对象


//-----------------------------------------------------------------------------
// Desc: 顶点结构和灵活顶点格式
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{   FLOAT x, y, z, rhw; 		// 经过坐标转换的顶点位置
DWORD color;       			// 顶点漫反射颜色值
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


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

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 创建并填充顶点缓冲区和索引缓冲区
//-----------------------------------------------------------------------------
HRESULT InitVBAndIB()
{
	//顶点数据
	CUSTOMVERTEX g_Vertices[9];
	g_Vertices[0].x = 300;
	g_Vertices[0].y = 250;
	g_Vertices[0].z = 0.5f;
	g_Vertices[0].rhw = 1.0f;
	g_Vertices[0].color = 0xffff0000;
	for(int i=0; i<8; i++)
	{
		g_Vertices[i+1].x =  (float)(200*sin(i*3.14159/4.0)) + 300;
		g_Vertices[i+1].y = -(float)(200*cos(i*3.14159/4.0)) + 250;
		g_Vertices[i+1].z = 0.5f;
		g_Vertices[i+1].rhw = 1.0f;
		g_Vertices[i+1].color = 0xff00ff00;
	}

	//顶点索引数组
	WORD g_Indices[] ={ 0,1,2, 0,2,3, 0,3,4, 0,4,5, 0,5,6, 0,6,7, 0,7,8, 0,8,1 };

	//创建顶点缓冲区
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 
		9*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB,NULL ) ) )
	{
		return E_FAIL;
	}

	//填充顶点缓冲区
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
	g_pVB->Unlock();

	//创建索引缓冲区
	if( FAILED( g_pd3dDevice->CreateIndexBuffer( 
		24*sizeof(WORD),
		0, D3DFMT_INDEX16,  //索引类型
		D3DPOOL_DEFAULT, &g_pIB,NULL ) ) )
	{
		return E_FAIL;
	}

	//填充顶点缓冲区
	VOID* pIndices;
	if( FAILED( g_pIB->Lock( 0, sizeof(g_Indices), (void**)&pIndices, 0 ) ) )
		return E_FAIL;
	memcpy( pIndices, g_Indices, sizeof(g_Indices) );
	g_pIB->Unlock();

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

	//释放索引缓冲区对象
	if(g_pIB != NULL)
		g_pIB->Release();

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
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	//开始在后台缓冲区绘制图形
	if( SUCCEEDED( g_pd3dDevice->BeginScene()))
	{
		//在后台缓冲区绘制图形
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->SetIndices( g_pIB );  //设置索引缓冲区
		g_pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0,0,9,0,8 );

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
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC | CS_HREDRAW | CS_VREDRAW, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
	RegisterClassEx( &wc );

	//创建窗口
	HWND hWnd = CreateWindow( L"ClassName", L"索引缓冲区",
		WS_OVERLAPPEDWINDOW, 200, 100, 600, 550,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//初始化Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		//创建并填充顶点缓冲区和索引缓冲区
		if( SUCCEEDED( InitVBAndIB() ) )
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
