//=============================================================================
// Desc: ���Դʾ������
//=============================================================================

#include <d3dx9.h>


//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9               g_pD3D         = NULL;   //Direct3D����
LPDIRECT3DDEVICE9         g_pd3dDevice   = NULL;   //Direct3D�豸����
LPDIRECT3DVERTEXBUFFER9   g_pVB          = NULL;   //���㻺��������


//-----------------------------------------------------------------------------
// Desc: ����ṹ
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	D3DXVECTOR3 position;   //����λ��
	D3DXVECTOR3 normal;     //���㷨��
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL)


//-----------------------------------------------------------------------------
// Desc: ���ñ任����
//-----------------------------------------------------------------------------
VOID SetMatrices()
{
	//�����������������
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//���������ù۲����
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIX matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//����������ͶӰ����
	D3DXMATRIX matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
	g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


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
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;


	//����Direct3D�豸����
	if( FAILED( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice ) ) )
	{
		return E_FAIL;
	}

	//�����޳�ģʽΪ���޳��κ���(����ͷ���)
	g_pd3dDevice->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	//������Ȳ���
	g_pd3dDevice->SetRenderState( D3DRS_ZENABLE, TRUE );

	//���þ��淴�����ģ��
	g_pd3dDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	//���ñ任����
	SetMatrices();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: ��������ͼ��
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	//�����㻺����
	if( FAILED( g_pd3dDevice->CreateVertexBuffer( 50*2*sizeof(CUSTOMVERTEX),
		0, D3DFVF_CUSTOMVERTEX,
		D3DPOOL_DEFAULT, &g_pVB, NULL ) ) )
	{
		return E_FAIL;
	}

	//��䶥�㻺����
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
// Desc: ���ò��ʺ͵ƹ�
//-----------------------------------------------------------------------------
VOID SetupLights()
{
	//���ò�������, ֻ������������̹����, �����Ͽ���������Ϸ����ɫ��
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

	//����һ�Ź�ԴΪ����⣬��ɫΪ��ɫ
	D3DXVECTOR3 vecDir;
	D3DLIGHT9 light1;
	ZeroMemory( &light1, sizeof(D3DLIGHT9) );
	light1.Type       = D3DLIGHT_DIRECTIONAL;
	light1.Diffuse.r  = 0.0f;
	light1.Diffuse.g  = 1.0f;
	light1.Diffuse.b  = 0.0f;
	vecDir = D3DXVECTOR3(-10, 0 ,10); //����ⷽ��

	D3DXVec3Normalize( (D3DXVECTOR3*)&light1.Direction, &vecDir );
	g_pd3dDevice->SetLight( 0, &light1 );

	//���Ź�ԴΪ���Դ����ɫΪ��ɫ
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

	//���Ź�ԴΪ�����,��ɫΪ��ɫ, *���淴��*
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

	//����һ���Ļ�����
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0x00505050);
}


//--------------------------------------------------------------
// Name: �ͷŴ����Ķ���
//--------------------------------------------------------------
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
	//��պ�̨����������Ȼ�����
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,
		D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0 );

	//��ʼ�ں�̨����������ͼ��
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//���ò��Ϻ͵ƹ�, ��Ϊ�ƹ����Բ��ϱ仯,�����ڴ�����
		SetupLights();

		//�ں�̨����������ͼ��
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*50-2 );

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
		switch(wParam)
		{
		case 48:  //"0"��, �������еĹ�Դ,ֻʹ�û�����
			g_pd3dDevice->LightEnable( 0, false );
			g_pd3dDevice->LightEnable( 1, false );
			g_pd3dDevice->LightEnable( 2, false );
			break;
		case 49:  //"1"��, ����1�Ź�Դ, �����
			g_pd3dDevice->LightEnable( 0, true );
			g_pd3dDevice->LightEnable( 1, false );
			g_pd3dDevice->LightEnable( 2, false );
			break;
		case 50:  //"2"��, ����2�Ź�Դ, ���Դ
			g_pd3dDevice->LightEnable( 0, false );
			g_pd3dDevice->LightEnable( 1, true );
			g_pd3dDevice->LightEnable( 2, false );
			break;
		case 51:  //"3"��, ����3�Ź�Դ, �����
			g_pd3dDevice->LightEnable( 0, false );
			g_pd3dDevice->LightEnable( 1, false );
			g_pd3dDevice->LightEnable( 2, true );
			break;
		case 52:  //"4"��, ͬʱ����������Դ
			g_pd3dDevice->LightEnable( 0, true );
			g_pd3dDevice->LightEnable( 1, true );
			g_pd3dDevice->LightEnable( 2, true );
			break;
		case 53:  //"5"��,ƽ����ɫģʽ
			g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_FLAT);
			break;

		case 54:  //"6"�������͵���ɫģʽ
			g_pd3dDevice->SetRenderState(D3DRS_SHADEMODE,D3DSHADE_GOURAUD);
			break;
		}
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
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		L"ClassName", NULL };
	RegisterClassEx( &wc );

	//��������
	HWND hWnd = CreateWindow( L"ClassName", L"���Դ",
		WS_OVERLAPPEDWINDOW, 200, 100, 500, 500,
		GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//��ʼ��Direct3D
	if( SUCCEEDED( InitD3D( hWnd ) ) )
	{
		//��������ͼ��
		if( SUCCEEDED( InitGeometry() ) )
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
					Render();  //��Ⱦ����
				}
			}
		}
	}

	UnregisterClass( L"ClassName", wc.hInstance );
	return 0;
}



