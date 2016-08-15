//=============================================================================
// Desc: ��򵥵�Direct3D����, 
//=============================================================================

#include <d3d9.h>


//-----------------------------------------------------------------------------
// ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; //Direct3D�豸����


//-----------------------------------------------------------------------------
// Desc: ��ʼ��Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	//����Direct3D����, �ö�����������Direct3D�豸����
    if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return E_FAIL;

	//����D3DPRESENT_PARAMETERS�ṹ, ׼������Direct3D�豸����
    D3DPRESENT_PARAMETERS d3dpp; 
    ZeroMemory( &d3dpp, sizeof(d3dpp) );
    d3dpp.Windowed = TRUE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	//����Direct3D�豸����
    if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice ) ) )
    {
        return E_FAIL;
    }

   return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �ͷŴ�������
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//�ͷ�Direct3D�豸����
    if( g_pd3dDevice != NULL) 
        g_pd3dDevice->Release();

	//�ͷ�Direct3D����
    if( g_pD3D != NULL)
        g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: ��Ⱦͼ��
//-----------------------------------------------------------------------------
VOID Render()
{
    //��պ�̨������
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0 );
    
    //��ʼ�ں�̨����������ͼ��
    if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
    {
        //�ڴ��ں�̨����������ͼ��
    
        //�����ں�̨��������Ⱦͼ��
        g_pd3dDevice->EndScene();
    }

    //���ں�̨���������Ƶ�ͼ���ύ��ǰ̨��������ʾ
    g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Desc: ��Ϣ����
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
// Desc: �������
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
    //ע�ᴰ����
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L, 
                      GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
                      L"ClassName", NULL };
    RegisterClassEx( &wc );

    //��������
    HWND hWnd = CreateWindow( L"ClassName", L"��򵥵�Direct3D����", 
                              WS_OVERLAPPEDWINDOW, 200, 100, 600, 500,
                              NULL, NULL, wc.hInstance, NULL );

    //��ʼ��Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    { 
        //��ʾ������
		ShowWindow( hWnd, SW_SHOWDEFAULT );
		UpdateWindow( hWnd );

		//������Ϣѭ��
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
				Render();  //��Ⱦͼ��
			}
		}
    }

    UnregisterClass( L"ClassName", wc.hInstance );
    return 0;
}

