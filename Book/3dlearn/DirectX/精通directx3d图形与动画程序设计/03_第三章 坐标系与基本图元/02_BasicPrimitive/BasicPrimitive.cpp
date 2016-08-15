//=============================================================================
// Desc: ����ͼԪ����
//=============================================================================

#include <d3d9.h>


//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL;   //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;   //Direct3D�豸����
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL;   //���㻺��������
int                     g_iType      = 1;      //ͼԪ����


//-----------------------------------------------------------------------------
// Desc: ����ṹ
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z, rhw;
	DWORD color;
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
	if( FAILED( g_pD3D->CreateDevice( 
		D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
		return E_FAIL;

	//�����޳�ģʽΪ���޳��κ���
	g_pd3dDevice->SetRenderState(D3DRS_CULLMODE,  D3DCULL_NONE);

	//����ͼԪ���ģʽΪ�߿�ģʽ
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

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
		{  50.0f, 250.0f, 0.5f, 1.0f, 0xffffff00, },
		{ 150.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, }, 
		{ 250.0f, 250.0f, 0.5f, 1.0f, 0xffff0000, },


		{ 350.0f,  50.0f, 0.5f, 1.0f, 0xffff0000, }, 
		{ 450.0f, 250.0f, 0.5f, 1.0f, 0xffff0000, },
		{ 550.0f, 50.0f,  0.5f, 1.0f, 0xffff0000, }
	};

	//�������㻺����
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 
		6*sizeof(CUSTOMVERTEX),
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
//------------------------------------------------------------------------------
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
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(60, 60, 60), 1.0f, 0 );

	//��ʼ�ں�̨����������ͼ��
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//�ں�̨����������ͼ��
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		switch(g_iType)  //ѡ�����ͼԪ������
		{
		case 1:   //����������
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 4 );
			break;

		case 2:   //�������б�
			g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2 );
			break;

		case 3:   //�߶�����
			g_pd3dDevice->DrawPrimitive( D3DPT_LINESTRIP, 0, 5 );
			break;

		case 4:  //�߶��б�
			g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST, 0, 3 );
			break;

		case 5:  //���б�
			g_pd3dDevice->DrawPrimitive( D3DPT_POINTLIST, 0, 6 );
			break;

		case 6:
			g_pd3dDevice->DrawPrimitive(  D3DPT_TRIANGLEFAN, 0, 4);
			break;
		}

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

	case WM_KEYDOWN:
		switch(wParam)  //������ȾͼԪ����
		{
		case 49:  //"1"��
		case 50:  //"2"��
		case 51:  //"3"��
		case 52:  //"4"��
		case 53:  //"5"��		
		case 54:  //"6"��
			g_iType = (int)wParam-48;
			break;
		}

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
	HWND hWnd = CreateWindow(  L"ClassName", L"����ͼԪ",
		WS_OVERLAPPEDWINDOW, 100, 100, 600, 300,
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

	UnregisterClass(  L"ClassName", wc.hInstance );
	return 0;
}
