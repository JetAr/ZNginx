//=============================================================================
// Desc: 多级渐进纹理
//=============================================================================

#include <d3dx9.h>

//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL;    //Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    //Direct3D设备对象
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL;    //顶点缓冲区对象
LPDIRECT3DTEXTURE9      g_pMipTex    = NULL;    //多级渐进纹理
LPDIRECT3DTEXTURE9      g_pTexture   = NULL;    //非多级渐进纹理
float                   g_fDistance  = -10;     //摄影机距图形的距离
bool                    g_bUseMipTex = true;    //标志是否使用多级渐进纹理


//-----------------------------------------------------------------------------
// Desc: 顶点结构
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z;    //顶点位置  
	FLOAT u,v ;		  //顶点纹理坐标
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ | D3DFVF_TEX1)


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

	
	//设置纹理过滤方式
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 创建场景图形
//-----------------------------------------------------------------------------
HRESULT InitGriphics()
{
	//创建多级渐进纹理
    if( FAILED( D3DXCreateTextureFromFileEx( g_pd3dDevice, 
		                                    L"texture.jpg",
											 0, 0,             //纹理宽度和高度, "0"表示以图形文件的宽度和高度作为纹理的宽度和高度
									         0,                //渐进纹理序列级数
											 0,                //纹理使用方式, 一般为0
											 D3DFMT_X8R8G8B8,  //纹理图形格式
											 D3DPOOL_MANAGED,  //纹理资源的内存类型
											 D3DX_DEFAULT,    
											 D3DX_DEFAULT, 
									         0xFF000000,0,0,
									         &g_pMipTex ) ) )
    {
		return E_FAIL;
    }

	//创建非多级渐进纹理纹理
    if( FAILED( D3DXCreateTextureFromFileEx( g_pd3dDevice, 
		                                    L"texture.jpg",
											 0, 0,             //纹理宽度和高度, "0"表示以图形文件的宽度和高度作为纹理的宽度和高度
									         1,                //渐进纹理序列级数
											 0,                //纹理使用方式, 一般为0
											 D3DFMT_X8R8G8B8,  //纹理图形格式
											 D3DPOOL_MANAGED,  //纹理资源的内存类型
											 D3DX_DEFAULT,    
											 D3DX_DEFAULT, 
									         0xFF000000,0,0,
									         &g_pTexture ) ) )
    {
		return E_FAIL;
    }

	//顶点数据
    CUSTOMVERTEX g_Vertices[] =
    {
        #define fdiv 80.f
		{ -3,   -3,  0.0f, 0.0f, 1.0f},   
	    { -3,    3,  0.0f, 0.0f, 0.0f},	
	    {  3,   -3,  0.0f, 1.0f, 1.0f},	
	    {  3,    3,  0.0f, 1.0f, 0.0f }

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
// Desc: 释放创建的对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//释放多级渐进纹理对象
	if( g_pMipTex != NULL )
        g_pMipTex->Release();

	//释放多级渐进纹理对象
	if( g_pTexture != NULL )
        g_pTexture->Release();

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
// Desc: 设置变换矩阵
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    //世界矩阵
    D3DXMATRIXA16 matWorld;
    D3DXMatrixIdentity( &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//观察矩阵, 观察点根据输入逐渐拉近或变远
    D3DXVECTOR3 vEyePt( 0.0f, 0.0f, g_fDistance );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//投影矩阵
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//-----------------------------------------------------------------------------
// Desc: 渲染图形 
//-----------------------------------------------------------------------------
VOID Render()
{
	//清空后台缓冲区
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0 );
	SetupMatrices();  //设置变换矩阵,

	//开始在后台缓冲区绘制图形
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//在后台缓冲区绘制图形
		if(g_bUseMipTex==true)
			g_pd3dDevice->SetTexture( 0, g_pMipTex );  //使用多级渐进纹理
		else
			g_pd3dDevice->SetTexture( 0, g_pTexture ); //使用普通纹理
		
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
		case 49:    //“1”键, 使用多级渐进纹理
			g_bUseMipTex = true;
			break;

		case 50:    //“2”键, 不使用多级渐进纹理
			g_bUseMipTex = false;
			break;

		case VK_DOWN:
			g_fDistance -= 0.1f;
			break;

		case VK_UP:
			g_fDistance += 0.1f;
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
    HWND hWnd = CreateWindow(  L"ClassName", L"多级渐进纹理",
                              WS_OVERLAPPEDWINDOW, 200, 100, 600, 500,
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

    UnregisterClass(  L"ClassName", wc.hInstance );
    return 0;
}
