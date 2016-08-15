//=============================================================================
// Desc: ����������
//=============================================================================

#include <d3d9.h>
#include <math.h>


//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL; // Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL; // Direct3D�豸����
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL; // ���㻺��������
LPDIRECT3DINDEXBUFFER9  g_pIB		 = NULL; // ��������������


//-----------------------------------------------------------------------------
// Desc: ����ṹ�������ʽ
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{   FLOAT x, y, z, rhw; 		// ��������ת���Ķ���λ��
DWORD color;       			// ������������ɫֵ
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


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
// Desc: ��������䶥�㻺����������������
//-----------------------------------------------------------------------------
HRESULT InitVBAndIB()
{
	//��������
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

	//������������
	WORD g_Indices[] ={ 0,1,2, 0,2,3, 0,3,4, 0,4,5, 0,5,6, 0,6,7, 0,7,8, 0,8,1 };

	//�������㻺����
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 
		9*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB,NULL ) ) )
	{
		return E_FAIL;
	}

	//��䶥�㻺����
	VOID* pVertices;
	if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
		return E_FAIL;
	memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
	g_pVB->Unlock();

	//��������������
	if( FAILED( g_pd3dDevice->CreateIndexBuffer( 
		24*sizeof(WORD),
		0, D3DFMT_INDEX16,  //��������
		D3DPOOL_DEFAULT, &g_pIB,NULL ) ) )
	{
		return E_FAIL;
	}

	//��䶥�㻺����
	VOID* pIndices;
	if( FAILED( g_pIB->Lock( 0, sizeof(g_Indices), (void**)&pIndices, 0 ) ) )
		return E_FAIL;
	memcpy( pIndices, g_Indices, sizeof(g_Indices) );
	g_pIB->Unlock();

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

	//�ͷ���������������
	if(g_pIB != NULL)
		g_pIB->Release();

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
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	//��ʼ�ں�̨����������ͼ��
	if( SUCCEEDED( g_pd3dDevice->BeginScene()))
	{
		//�ں�̨����������ͼ��
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->SetIndices( g_pIB );  //��������������
		g_pd3dDevice->DrawIndexedPrimitive ( D3DPT_TRIANGLELIST, 0,0,9,0,8 );

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
		PostQuitMessage( 0 );
		return 0;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}


//-----------------------------------------------------------------------------
// Desc: ��ں���
//-----------------------------------------------------------------------------
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, INT )
{
	//ע�ᴰ����
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC | CS_HREDRAW | CS_VREDRAW, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
	RegisterClassEx( &wc );

	//��������
	HWND hWnd = CreateWindow( L"ClassName", L"����������",
		WS_OVERLAPPEDWINDOW, 200, 100, 600, 550,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//��ʼ��Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		//��������䶥�㻺����������������
		if( SUCCEEDED( InitVBAndIB() ) )
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
					TranslateMessage( &msg );
					DispatchMessage( &msg );
				}
				else
				{
					Render();  //��Ⱦͼ��
				}
			}
		}
	}

	UnregisterClass( L"ClassName", wc.hInstance );
	return 0;
}
