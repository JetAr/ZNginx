//=============================================================================
// Desc: 简单灯光范例程序
//=============================================================================

#include <d3dx9.h>


//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9               g_pD3D         = NULL;   //Direct3D对象
LPDIRECT3DDEVICE9         g_pd3dDevice   = NULL;   //Direct3D设备对象
LPDIRECT3DVERTEXBUFFER9   g_pVB          = NULL;   //顶点缓冲区对象

//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;   //顶点位置
	D3DXVECTOR3 normal;     //顶点法线
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)


//-----------------------------------------------------------------------------
// Desc: 设置变换矩阵
//-----------------------------------------------------------------------------
VOID SetMatrices()
{
	//建立并设置世界矩阵
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

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
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


	//创建Direct3D设备对象
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	//设置剔出模式为不剔出任何面(正面和方面)
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	//启用深度测试
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );    

	//设置变换矩阵
	SetMatrices();

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
		pVertices[2*i+0].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
		pVertices[2*i+1].position = D3DXVECTOR3( sinf(theta), 1.0f, cosf(theta) );
		pVertices[2*i+1].normal   = D3DXVECTOR3( sinf(theta), 0.0f, cosf(theta) );
	}
	g_pVB->Unlock();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 设置材质和灯光
//-----------------------------------------------------------------------------
VOID SetLight()
{
	//设置材质
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
	mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
	mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
	mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
	mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
	g_pd3dDevice->SetMaterial( &mtrl );

	//设置灯光
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(D3DLIGHT9) );
	light.Type       = D3DLIGHT_DIRECTIONAL;
	light.Diffuse.r  = 1.0f;
	light.Diffuse.g  = 1.0f;
	light.Diffuse.b  = 1.0f;
	vecDir = D3DXVECTOR3(
		cosf(timeGetTime()/350.0f),
		1.0f,
		sinf(timeGetTime()/350.0f) );
	vecDir = D3DXVECTOR3(1.0f, 1.0f, 0.0f);
	D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
	light.Range       = 1000.0f;
	g_pd3dDevice->SetLight( 0, &light );
	g_pd3dDevice->LightEnable( 0, TRUE );
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	//设置环境光
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00505050);
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
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0 );

	//开始在后台缓冲区绘制图形
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//设置灯光和材质
		SetLight();

		//在后台缓冲区绘制图形
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
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
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
	RegisterClassEx( &wc );

	//创建窗口
	HWND hWnd = CreateWindow( L"ClassName", L"灯光",
		WS_OVERLAPPEDWINDOW, 200, 100, 500, 500,
		NULL, NULL, wc.hInstance, NULL );

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



