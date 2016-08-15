//=============================================================================
// Desc: ʹ�ö��㻺��������������
//=============================================================================

#include <d3d9.h>

//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL;    //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    //Direct3D�豸����
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL;    //���㻺��������


//-----------------------------------------------------------------------------
// Desc: ����ṹ
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)  //�����ʽ


//-----------------------------------------------------------------------------
// Desc: ��ʼ��Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D( HWND hWnd )
{
	//����Direct3D����, �ö������ڴ���Direct3D�豸����
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return E_FAIL;

	//����D3DPRESENT_PARAMETERS�ṹ, ׼������Direct3D�豸����
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof(d3dpp) );
	d3dpp.Windowed = false;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;

	//��ѯ��ǰ��ʾģʽ
	D3DDISPLAYMODE d3ddm;
	if( FAILED( g_pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT
		, &d3ddm ) ) )
		return E_FAIL;

	d3dpp.BackBufferWidth  = d3ddm.Width;
	d3dpp.BackBufferHeight = d3ddm.Height;
	d3dpp.BackBufferFormat = d3ddm.Format;

	//����Direct3D�豸����
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
// Desc: ��������䶥�㻺����
//-----------------------------------------------------------------------------
HRESULT InitVB()
{
	//��������
	CUSTOMVERTEX vertices[] =
	{
		{ 100.0f,  650.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 500.0f,  100.0f, 0.5f, 1.0f, 0xff00ff00, }, 
		{ 900.0f,  650.0f, 0.5f, 1.0f, 0xff0000ff, },
	};

	//�������㻺����
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 
		3*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	//��䶥�㻺����
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, vertices, sizeof(vertices) );
	g_pVB->Unlock();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �ͷŴ����Ķ���
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//�ͷŶ��㻺��������
	if( g_pVB != NULL )        
		g_pVB->Release();

	//�ͷ�Direct3D�豸����
	if( g_pd3dDevice != NULL ) 
		g_pd3dDevice->Release();

	//�ͷ�Direct3D����
	if( g_pD3D != NULL )       
		g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: ��Ⱦͼ�� 
//-----------------------------------------------------------------------------
VOID Render()
{
	//��պ�̨������
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(45, 50, 170)
		, 1.0f, 0 );

	//��ʼ�ں�̨����������ͼ��
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//�ں�̨����������ͼ��
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 1 );

		//�����ں�̨����������ͼ��
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

	case WM_KEYDOWN:
		PostMessage(hWnd, WM_CLOSE, 0, 0);
		break;
	}

	return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Desc: ��ں���
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	//ע�ᴰ����
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
	RegisterClassEx( &wc );

	//��������
	HWND hWnd = CreateWindow( L"ClassName", L"���㻺����",
		WS_OVERLAPPEDWINDOW, 200, 100, 600, 500,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//��ʼ��Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		//��������䶥�㻺����
		if( SUCCEEDED( InitVB() ) )
		{
			//��ʾ����
			ShowWindow( hWnd, SW_SHOWDEFAULT );
			UpdateWindow( hWnd );

			//������Ϣѭ��
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
					Render();  //��Ⱦͼ��
				}
			}
		}
	}

	UnregisterClass(L"ClassName", wc.hInstance);
	return 0;
}
