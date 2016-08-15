//=============================================================================
// Desc: 纹理寻址模式
//=============================================================================

#include <d3dx9.h>


//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9               g_pD3D        = NULL;  //Direct3D对象
LPDIRECT3DDEVICE9         g_pd3dDevice  = NULL;  //Direct3D设备对象
LPDIRECT3DVERTEXBUFFER9   g_pVB         = NULL;  //顶点缓冲区对象
LPDIRECT3DTEXTURE9        g_pTexture1   = NULL;  //纹理对象1
LPDIRECT3DTEXTURE9        g_pTexture2   = NULL;  //纹理对象2
int                       g_iTexAddressMode = 1;



//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z;    //顶点位置
	FLOAT u,v ;		  //顶点纹理坐标
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ| D3DFVF_TEX1)


//-----------------------------------------------------------------------------
// Desc: 设置观察矩阵和投影矩阵
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
	//创建并设置世界矩阵
	D3DXMATRIXA16 matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//创建并设置观察矩阵
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f, -10 );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//创建并设置投影矩阵
	D3DXMATRIXA16 matProj;
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

	//创建Direct3D设备对象
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	//禁用照明效果
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE ); 

	//设置纹理渲染状态
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

	//设置纹理过滤方式
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_POINT);

	//设置变换矩阵
	SetupMatrices();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 创建场景图形
//-----------------------------------------------------------------------------
HRESULT InitGriphics()
{
	//创建纹理1
	if( FAILED( D3DXCreateTextureFromFile(g_pd3dDevice, L"texture1.bmp", &g_pTexture1)) )
	{
		return E_FAIL;
	}

	//创建纹理2
	if( FAILED( D3DXCreateTextureFromFile(g_pd3dDevice, L"texture2.bmp", &g_pTexture2)) )
	{
		return E_FAIL;
	}

	//顶点数据
	CUSTOMVERTEX g_Vertices[] =
	{
		{ -3,   -3,  0.0f,  0.0f, 3.0f},     //注意纹理坐标
		{ -3,    3,  0.0f,  0.0f, 0.0f},	
		{  3,   -3,  0.0f,  3.0f, 3.0f},	
		{  3,    3,  0.0f,  3.0f, 0.0f }

	};

	//创建顶点缓冲区
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_MANAGED, &g_pVB,NULL ) ) )
	{
		return E_FAIL;
	}

	//填充顶点缓冲区
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
	g_pVB->Unlock();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 释放除创建的对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//释放纹理对象1
	if( g_pTexture1 != NULL )        
		g_pTexture1->Release();

	//释放纹理对象2
	if( g_pTexture2 != NULL ) 
		g_pTexture2->Release();

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
// Desc: 设置纹理和纹理寻址模式
//-----------------------------------------------------------------------------
void SetTexture()
{
	switch(g_iTexAddressMode)
	{
	case 1:
		g_pd3dDevice->SetTexture( 0, g_pTexture1 );  //设置纹理
		//设置纹理寻址模式
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		break;

	case 2:
		g_pd3dDevice->SetTexture( 0, g_pTexture1 );  //设置纹理
		//设置纹理寻址模式
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_MIRROR);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_MIRROR);
		break;

	case 3:
		g_pd3dDevice->SetTexture( 0, g_pTexture2 );  //设置纹理
		//设置纹理寻址模式
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		break;

	case 4:
		g_pd3dDevice->SetTexture( 0, g_pTexture2 );  //设置纹理
		//设置边框颜色
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_BORDERCOLOR, 0xffff0000);
		//设置纹理寻址模式
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
		g_pd3dDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
		break;
	}
}


//-----------------------------------------------------------------------------
// Desc: 渲染图形 
//-----------------------------------------------------------------------------
VOID Render()
{
	//清除后缓冲区和深度缓冲区
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	//开始在后台缓冲区绘制图形
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//在后台缓冲区绘制图形
		SetTexture();  //设置纹理
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

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
		switch(wParam)
		{
		case 49:    //“1”键
			g_iTexAddressMode = 1;
			break;

		case 50:    //“2”键
			g_iTexAddressMode = 2;
			break;

		case 51:    //“3”键
			g_iTexAddressMode = 3;
			break;

		case 52:     //“4”键
			g_iTexAddressMode = 4;
			break;
		}
		return 0;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


//--------------------------------------------------------
// Desc: 入口函数
//--------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{

	//注册窗口类
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
	RegisterClassEx( &wc );

	//创建窗口
	HWND hWnd = CreateWindow( L"ClassName", L"纹理寻址模式",
		WS_OVERLAPPEDWINDOW, 100, 100, 480, 480,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//初始化Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		//创建场景图形
		if( SUCCEEDED( InitGriphics() ) )
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
