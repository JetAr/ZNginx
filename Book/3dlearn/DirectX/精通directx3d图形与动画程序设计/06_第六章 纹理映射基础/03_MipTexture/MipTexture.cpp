//=============================================================================
// Desc: �༶��������
//=============================================================================

#include <d3dx9.h>

//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D       = NULL;    //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;    //Direct3D�豸����
LPDIRECT3DVERTEXBUFFER9 g_pVB        = NULL;    //���㻺��������
LPDIRECT3DTEXTURE9      g_pMipTex    = NULL;    //�༶��������
LPDIRECT3DTEXTURE9      g_pTexture   = NULL;    //�Ƕ༶��������
float                   g_fDistance  = -10;     //��Ӱ����ͼ�εľ���
bool                    g_bUseMipTex = true;    //��־�Ƿ�ʹ�ö༶��������


//-----------------------------------------------------------------------------
// Desc: ����ṹ
//-----------------------------------------------------------------------------
struct CUSTOMVERTEX
{
	FLOAT x, y, z;    //����λ��  
	FLOAT u,v ;		  //������������
};
#define D3DFVF_CUSTOMVERTEX   (D3DFVF_XYZ | D3DFVF_TEX1)


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

	//��������Ч��
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );

	
	//����������˷�ʽ
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: ��������ͼ��
//-----------------------------------------------------------------------------
HRESULT InitGriphics()
{
	//�����༶��������
    if( FAILED( D3DXCreateTextureFromFileEx( g_pd3dDevice, 
		                                    L"texture.jpg",
											 0, 0,             //�����Ⱥ͸߶�, "0"��ʾ��ͼ���ļ��Ŀ�Ⱥ͸߶���Ϊ����Ŀ�Ⱥ͸߶�
									         0,                //�����������м���
											 0,                //����ʹ�÷�ʽ, һ��Ϊ0
											 D3DFMT_X8R8G8B8,  //����ͼ�θ�ʽ
											 D3DPOOL_MANAGED,  //������Դ���ڴ�����
											 D3DX_DEFAULT,    
											 D3DX_DEFAULT, 
									         0xFF000000,0,0,
									         &g_pMipTex ) ) )
    {
		return E_FAIL;
    }

	//�����Ƕ༶������������
    if( FAILED( D3DXCreateTextureFromFileEx( g_pd3dDevice, 
		                                    L"texture.jpg",
											 0, 0,             //�����Ⱥ͸߶�, "0"��ʾ��ͼ���ļ��Ŀ�Ⱥ͸߶���Ϊ����Ŀ�Ⱥ͸߶�
									         1,                //�����������м���
											 0,                //����ʹ�÷�ʽ, һ��Ϊ0
											 D3DFMT_X8R8G8B8,  //����ͼ�θ�ʽ
											 D3DPOOL_MANAGED,  //������Դ���ڴ�����
											 D3DX_DEFAULT,    
											 D3DX_DEFAULT, 
									         0xFF000000,0,0,
									         &g_pTexture ) ) )
    {
		return E_FAIL;
    }

	//��������
    CUSTOMVERTEX g_Vertices[] =
    {
        #define fdiv 80.f
		{ -3,   -3,  0.0f, 0.0f, 1.0f},   
	    { -3,    3,  0.0f, 0.0f, 0.0f},	
	    {  3,   -3,  0.0f, 1.0f, 1.0f},	
	    {  3,    3,  0.0f, 1.0f, 0.0f }

    };
	
	//�������㻺����
    if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(CUSTOMVERTEX),
                                                 0, D3DFVF_CUSTOMVERTEX,
                                                 D3DPOOL_MANAGED, &g_pVB,NULL ) ) )
    {
        return E_FAIL;
    }

    //��䶥�㻺����
    VOID* pVertices;
    if( FAILED( g_pVB->Lock( 0, sizeof(g_Vertices), (void**)&pVertices, 0 ) ) )
        return E_FAIL;
    memcpy( pVertices, g_Vertices, sizeof(g_Vertices) );
    g_pVB->Unlock();

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �ͷŴ����Ķ���
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//�ͷŶ༶�����������
	if( g_pMipTex != NULL )
        g_pMipTex->Release();

	//�ͷŶ༶�����������
	if( g_pTexture != NULL )
        g_pTexture->Release();

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
// Desc: ���ñ任����
//-----------------------------------------------------------------------------
VOID SetupMatrices()
{
    //�������
    D3DXMATRIXA16 matWorld;
    D3DXMatrixIdentity( &matWorld );
    g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );

	//�۲����, �۲������������������Զ
    D3DXVECTOR3 vEyePt( 0.0f, 0.0f, g_fDistance );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//ͶӰ����
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f );
    g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );
}


//-----------------------------------------------------------------------------
// Desc: ��Ⱦͼ�� 
//-----------------------------------------------------------------------------
VOID Render()
{
	//��պ�̨������
    g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(45, 50, 170), 1.0f, 0 );
	SetupMatrices();  //���ñ任����,

	//��ʼ�ں�̨����������ͼ��
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		//�ں�̨����������ͼ��
		if(g_bUseMipTex==true)
			g_pd3dDevice->SetTexture( 0, g_pMipTex );  //ʹ�ö༶��������
		else
			g_pd3dDevice->SetTexture( 0, g_pTexture ); //ʹ����ͨ����
		
		g_pd3dDevice->SetStreamSource( 0, g_pVB, 0, sizeof(CUSTOMVERTEX) );
		g_pd3dDevice->SetFVF( D3DFVF_CUSTOMVERTEX );
		g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2);

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
		case 49:    //��1����, ʹ�ö༶��������
			g_bUseMipTex = true;
			break;

		case 50:    //��2����, ��ʹ�ö༶��������
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
    HWND hWnd = CreateWindow(  L"ClassName", L"�༶��������",
                              WS_OVERLAPPEDWINDOW, 200, 100, 600, 500,
                              GetDesktopWindow(), NULL, wc.hInstance, NULL );

	//��ʼ��Direct3D
    if( SUCCEEDED( InitD3D( hWnd ) ) )
    {
		//��������ͼ��
        if( SUCCEEDED( InitGriphics() ) )
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
