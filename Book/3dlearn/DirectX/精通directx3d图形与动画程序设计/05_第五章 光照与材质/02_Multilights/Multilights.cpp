//=============================================================================
// Desc: 多光源示例程序
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

	//启用镜面反射光照模型
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

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
VOID SetupLights()
{
	//设置材料属性, 只反射红光分量和绿光分量, 整体上看该物体材料反射黄色光
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );

	mtrl.Ambient.r = 1.0f;
	mtrl.Ambient.g = 1.0f;
	mtrl.Ambient.b = 0.0f;
	mtrl.Ambient.a = 1.0f;

	mtrl.Diffuse.r = 1.0f;
	mtrl.Diffuse.g = 1.0f;
	mtrl.Diffuse.b = 0.0f;
	mtrl.Diffuse.a = 0.5f;

	mtrl.Specular.r = 1.0f;
	mtrl.Specular.g = 1.0f;
	mtrl.Specular.b = 0.0f;
	mtrl.Specular.a = 1.0f;

	g_pd3dDevice->SetMaterial( &mtrl );

	//设置一号光源为方向光，颜色为绿色
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light1;
	ZeroMemory( &light1, sizeof(D3DLIGHT9) );
	light1.Type       = D3DLIGHT_DIRECTIONAL;
	light1.Diffuse.r  = 0.0f;
	light1.Diffuse.g  = 1.0f;
	light1.Diffuse.b  = 0.0f;
	vecDir = D3DXVECTOR3(-10, 0 ,10); //方向光方向

	D3DXVec3Normalize( (D3DXVECTOR3*)&light1.Direction, &vecDir );
	g_pd3dDevice->SetLight( 0, &light1 );

	//二号光源为点光源，颜色为红色
	D3DLIGHT9 light2;
	ZeroMemory( &light2, sizeof(D3DLIGHT9) );
	light2.Type       = D3DLIGHT_POINT;  
	light2.Diffuse.r  = 1.0f;
	light2.Diffuse.g  = 0.0f;
	light2.Diffuse.b  = 0.0f;

	light2.Position= D3DXVECTOR3(10*sinf(timeGetTime()/350.0f) ,
		0,
		10*cosf(timeGetTime()/350.0f) );

	light2.Range        = 100.0f;
	light2.Attenuation0 = 1.0f;
	g_pd3dDevice->SetLight( 1, &light2 );

	//三号光源为方向光,颜色为绿色, *镜面反射*
	D3DXVECTOR3 vecDir3;
	D3DLIGHT9 light3;
	ZeroMemory( &light3, sizeof(D3DLIGHT9) );
	light3.Type       = D3DLIGHT_DIRECTIONAL; 
	light3.Specular.r  = 0.0f;
	light3.Specular.g  = 1.0f;
	light3.Specular.b  = 0.0f;
	light3.Specular.a  = 1.0f;
	vecDir3 = D3DXVECTOR3(-10,0,10);

	D3DXVec3Normalize( (D3DXVECTOR3*)&light3.Direction, &vecDir3 );
	g_pd3dDevice->SetLight( 2, &light3 );

	//设置一定的环境光
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00505050);
}


//--------------------------------------------------------------
// Name: 释放创建的对象
//--------------------------------------------------------------
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
	//清空后台缓冲区和深度缓冲区
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0 );

	//开始在后台缓冲区绘制图形
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//设置材料和灯光, 因为灯光属性不断变化,所以在此设置
		SetupLights();

		//在后台缓冲区绘制图形
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

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
		case 48:  //"0"键, 禁用所有的光源,只使用环境光
			g_pd3dDevice->LightEnable( 0, false );
			g_pd3dDevice->LightEnable( 1, false );
			g_pd3dDevice->LightEnable( 2, false );
			break;
		case 49:  //"1"键, 启用1号光源, 方向光
			g_pd3dDevice->LightEnable( 0, true );
			g_pd3dDevice->LightEnable( 1, false );
			g_pd3dDevice->LightEnable( 2, false );
			break;
		case 50:  //"2"键, 启用2号光源, 点光源
			g_pd3dDevice->LightEnable( 0, false );
			g_pd3dDevice->LightEnable( 1, true );
			g_pd3dDevice->LightEnable( 2, false );
			break;
		case 51:  //"3"键, 启用3号光源, 方向光
			g_pd3dDevice->LightEnable( 0, false );
			g_pd3dDevice->LightEnable( 1, false );
			g_pd3dDevice->LightEnable( 2, true );
			break;
		case 52:  //"4"键, 同时启用三个光源
			g_pd3dDevice->LightEnable( 0, true );
			g_pd3dDevice->LightEnable( 1, true );
			g_pd3dDevice->LightEnable( 2, true );
			break;
		case 53:  //"5"键,平面着色模式
			g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_FLAT);
			break;

		case 54:  //"6"键，戈劳德着色模式
			g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
			break;
		}
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
	HWND hWnd = CreateWindow( L"ClassName", L"多光源",
		WS_OVERLAPPEDWINDOW, 200, 100, 500, 500,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

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
					Render();  //渲染场景
				}
			}
		}
	}

	UnregisterClass( L"ClassName", wc.hInstance );
	return 0;
}



