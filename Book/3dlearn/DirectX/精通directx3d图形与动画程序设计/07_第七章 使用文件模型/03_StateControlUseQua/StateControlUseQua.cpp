//=============================================================================
// Desc: �ļ�����ģ�͵�ʹ��
//=============================================================================
#include <d3dx9.h>


//-----------------------------------------------------------------------------
// Desc: ȫ�ֱ���
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D           = NULL;  //Direct3D����
LPDIRECT3DDEVICE9       g_pd3dDevice     = NULL;  //Direct3D�豸����

LPD3DXMESH              g_pMesh          = NULL;  //����ģ�Ͷ���
D3DMATERIAL9*           g_pMeshMaterials = NULL;  //����ģ�Ͳ���
LPDIRECT3DTEXTURE9*     g_pMeshTextures  = NULL;  //����ģ������
DWORD                   g_dwNumMaterials = 0L;    //����ģ�Ͳ�������
D3DXVECTOR3             g_vCenter;                //����ģ�����ĵ� 

BYTE  m_bKey[256];			// keyboard state buffer

D3DXMATRIX              g_matWorld;


//-----------------------------------------------------------------------------
// Desc: �����������
//-----------------------------------------------------------------------------
VOID SetWorldMatrix()
{
	static long curTime=0;
	static float elapsetime=0;
	elapsetime = (timeGetTime()-curTime)/1000.0f;
	curTime = timeGetTime();

	//�����������������  
	float fRoll, fPitch, fYaw;
	fRoll = fPitch = fYaw = 0.0f;

	if (m_bKey['D']) fRoll  -= 3*elapsetime;
	if (m_bKey['A']) fRoll  +=  3*elapsetime;
	if (m_bKey['S']) fPitch -= 3*elapsetime;
	if (m_bKey['W']) fPitch += 3*elapsetime;
	if (m_bKey['Q']) fYaw   -= 3*elapsetime;
	if (m_bKey['E']) fYaw   += 3*elapsetime;

	//��������ģ����̬
	D3DXQUATERNION qR;
	D3DXMATRIX matRot;
	D3DXQuaternionRotationYawPitchRoll (&qR, fYaw, fPitch, fRoll);	
	D3DXMatrixRotationQuaternion (&matRot, &qR);
	D3DXMatrixMultiply (&g_matWorld, &matRot, &g_matWorld);

	//��ȡ����ģ��ǰ����
	static D3DXVECTOR3 vLook;
	vLook.x = g_matWorld._31;
	vLook.y = g_matWorld._32;
	vLook.z = g_matWorld._33;

	//��ǰ�ƶ�
	if (m_bKey['F'])
	{
		g_matWorld._41 += 10*elapsetime * vLook.x;
		g_matWorld._42 += 10*elapsetime * vLook.y;
		g_matWorld._43 += 10*elapsetime * vLook.z;
	}

	//����ƶ�
	if (m_bKey['V']) 
	{
		g_matWorld._41 -= 10*elapsetime * vLook.x;
		g_matWorld._42 -= 10*elapsetime * vLook.y;
		g_matWorld._43 -= 10*elapsetime * vLook.z;
	}

	g_pd3dDevice->SetTransform( D3DTS_WORLD, &g_matWorld );
}


//-----------------------------------------------------------------------------
// Desc: ���ù۲�����ͶӰ����
//-----------------------------------------------------------------------------
VOID SetViewAndProjMatrix()
{
	//���������ù۲����
	D3DXVECTOR3 vEyePt( 0.0f, 0.0f,-20.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//����������ͶӰ����
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, 1.0f, 1.0f, 500.0f );
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

	//�����������״̬
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	g_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1 );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
	g_pd3dDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );

	//���ù۲�����ͶӰ����
	SetViewAndProjMatrix();

	ZeroMemory( m_bKey, 256 );
	D3DXMatrixIdentity(&g_matWorld);

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �Ӿ���·������ȡ�����ļ���
//-----------------------------------------------------------------------------
void RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName)
{
	//�Ƚ�fullPath�����ͱ任ΪLPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//�Ӿ���·������ȡ�ļ���
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		lstrcpy(fileName, ++pch);
	else
		lstrcpy(fileName, wszFullPath);
}



//-----------------------------------------------------------------------------
// Desc: ��������ͼ��
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;  //�洢����ģ�Ͳ��ʵĻ���������

	//�Ӵ����ļ���������ģ��
	if( FAILED( D3DXLoadMeshFromX( L"airplane.x", D3DXMESH_MANAGED, 
		g_pd3dDevice, NULL, 
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, 
		&g_pMesh ) ) )
	{
		MessageBox(NULL, L"Could not find airplane.x", L"Mesh", MB_OK);
		return E_FAIL;
	}

	//������ģ������ȡ�������Ժ������ļ��� 
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];

	if( g_pMeshMaterials == NULL )
		return E_OUTOFMEMORY;

	g_pMeshTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
	if( g_pMeshTextures == NULL )
		return E_OUTOFMEMORY;

	//�����ȡ����ģ�Ͳ������Ժ������ļ���
	for( DWORD i=0; i<g_dwNumMaterials; i++ )
	{
		//��������
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		//����ģ�Ͳ��ϵĻ����ⷴ��ϵ��, ��Ϊģ�Ͳ��ϱ���û�����û����ⷴ��ϵ��
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		if( d3dxMaterials[i].pTextureFilename != NULL && 
			strlen(d3dxMaterials[i].pTextureFilename) > 0 )
		{
			//��ȡ�����ļ���
			WCHAR filename[256];
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, filename);

			//��������
			if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, filename, 
				&g_pMeshTextures[i] ) ) )
			{
				MessageBox(NULL, L"Could not find texture file", L"Mesh", MB_OK);
			}
		}
	}

	//�ͷ��ڼ���ģ���ļ�ʱ�����ı���ģ�Ͳ��ʺ��������ݵĻ���������
	pD3DXMtrlBuffer->Release();

	//��������ģ�ͱ߽�������
	LPDIRECT3DVERTEXBUFFER9 pVB; //������󶥵㻺�����ӿ�
	if( SUCCEEDED( g_pMesh->GetVertexBuffer( &pVB ) ) )
	{
		struct VERTEX { FLOAT x,y,z,tu,tv; };
		VERTEX*      pVertices; //���㻺����ָ��
		float        fRadius;

		pVB->Lock( 0, 0, (void**)&pVertices, 0 ); 

		//�����������ı߽������ĺͰ뾶
		D3DXComputeBoundingSphere( (D3DXVECTOR3*)pVertices, g_pMesh->GetNumVertices(),
			D3DXGetFVFVertexSize(g_pMesh->GetFVF()),
			&g_vCenter, &fRadius );
		pVB->Unlock();
		pVB->Release();
	}

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: �ͷŴ����Ķ���
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//�ͷ�����ģ�Ͳ���
	if( g_pMeshMaterials != NULL ) 
		delete[] g_pMeshMaterials;

	//�ͷ�����ģ������
	if( g_pMeshTextures )
	{
		for( DWORD i = 0; i < g_dwNumMaterials; i++ )
		{
			if( g_pMeshTextures[i] )
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}

	//�ͷ�����ģ�Ͷ���
	if( g_pMesh != NULL )
		g_pMesh->Release();

	//�ͷ�Direct3D�豸����
	if( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	//�ͷ�Direct3D����
	if( g_pD3D != NULL )
		g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: ��Ⱦ����
//-----------------------------------------------------------------------------
VOID Render()
{
	// ���������
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	//��ʼ��Ⱦ����
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		SetWorldMatrix();  //�����������

		//�����Ⱦ����ģ��
		for( DWORD i=0; i<g_dwNumMaterials; i++ )
		{
			//���ò��Ϻ�����
			g_pd3dDevice->SetMaterial( &g_pMeshMaterials[i] );
			g_pd3dDevice->SetTexture( 0, g_pMeshTextures[i] );

			//��Ⱦģ��
			g_pMesh->DrawSubset( i );
		}

		//������Ⱦ����
		g_pd3dDevice->EndScene();
	}

	//����Ļ����ʾ����
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Desc: ���ڹ���, ������Ϣ
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
		m_bKey[wParam] = 1;
		return 0;

	case WM_KEYUP:
		m_bKey[wParam] = 0;
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
	HWND hWnd = CreateWindow( L"ClassName", L"ͨ����Ԫ��ʵ��ģ�Ϳռ䶨λ", 
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
				if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
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



