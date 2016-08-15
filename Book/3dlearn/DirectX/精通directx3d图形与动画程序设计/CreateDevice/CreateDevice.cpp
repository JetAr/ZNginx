//=============================================================================
// Desc: 最简单的Direct3D程序, 
//=============================================================================

#include <d3d9.h>


//-----------------------------------------------------------------------------
// 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; //Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; //Direct3D设备对象


//-----------------------------------------------------------------------------
// Desc: 初始化Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	//创建Direct3D对象, 该对象用来创建Direct3D设备对象
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
// Desc: 释放创建对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//释放Direct3D设备对象
    if( g_pd3dDevice != NULL) 
        g_pd3dDevice->Release();

	//释放Direct3D对象
    if( g_pD3D != NULL)
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
        //在此在后台缓冲区绘制图形
    
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

        case WM_PAINT:
            Render();
            ValidateRect( hWnd, NULL );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Desc: 程序入口
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    //注册窗口类
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx( &wc );

    //创建窗口
    HWND hWnd = CreateWindow( L"ClassName", L"最简单的Direct3D程序", 
                              WS_OVERLAPPEDWINDOW, 200, 100, 600, 500,
                              NULL, NULL, wc.hInstance, NULL );

    //初始化Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
        //显示主窗口
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

    UnregisterClass( L"ClassName", wc.hInstance );
    return 0;
}

