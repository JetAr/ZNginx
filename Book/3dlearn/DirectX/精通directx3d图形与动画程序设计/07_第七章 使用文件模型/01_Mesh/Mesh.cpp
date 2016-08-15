//=============================================================================
// Desc: 文件网格模型的使用
//=============================================================================
#include <d3dx9.h>


//-----------------------------------------------------------------------------
// Desc: 全局变量
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D           = NULL;  //Direct3D对象
LPDIRECT3DDEVICE9       g_pd3dDevice     = NULL;  //Direct3D设备对象

LPD3DXMESH              g_pMesh          = NULL;  //网格模型对象
D3DMATERIAL9*           g_pMeshMaterials = NULL;  //网格模型材质
LPDIRECT3DTEXTURE9*     g_pMeshTextures  = NULL;  //网格模型纹理
DWORD                   g_dwNumMaterials = 0L;    //网格模型材质数量
DWORD					g_dwNumShowSubMesh = 0; 

//-----------------------------------------------------------------------------
// Desc: 设置世界矩阵
//-----------------------------------------------------------------------------
VOID SetWorldMatrix()
{
	//创建并设置世界矩阵
	D3DXMATRIXA16 matWorld;
	D3DXMatrixRotationY( &matWorld, timeGetTime()/1000.0f );
	g_pd3dDevice->SetTransform( D3DTS_WORLD, &matWorld );
}


//-----------------------------------------------------------------------------
// Desc: 设置观察矩阵和投影矩阵
//-----------------------------------------------------------------------------
VOID SetViewAndProjMatrix()
{
	//创建并设置观察矩阵
	D3DXVECTOR3 vEyePt( 0.0f, 10.0f,-20.0f );
	D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
	g_pd3dDevice->SetTransform( D3DTS_VIEW, &matView );

	//创建并设置投影矩阵
	D3DXMATRIXA16 matProj;
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

	//设置环境光
	g_pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	//设置观察矩阵和投影矩阵
	SetViewAndProjMatrix();

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 从绝对路径中提取纹理文件名
//-----------------------------------------------------------------------------
void RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName)
{
	//先将fullPath的类型变换为LPWSTR
	WCHAR wszBuf[MAX_PATH];
	MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
	wszBuf[MAX_PATH-1] = L'\0';

	WCHAR* wszFullPath = wszBuf;

	//从绝对路径中提取文件名
	LPWSTR pch=wcsrchr(wszFullPath,'\\');
	if (pch)
		lstrcpy(fileName, ++pch);
	else
		lstrcpy(fileName, wszFullPath);
}



//-----------------------------------------------------------------------------
// Desc: 创建场景图形
//-----------------------------------------------------------------------------
HRESULT InitGeometry()
{
	LPD3DXBUFFER pD3DXMtrlBuffer;  //存储网格模型材质的缓冲区对象

	//从磁盘文件加载网格模型
	if( FAILED( D3DXLoadMeshFromX( 
		L"airplane.x", D3DXMESH_MANAGED, 
		g_pd3dDevice, NULL, 
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials, 
		&g_pMesh ) )
		)
	{
		MessageBox(NULL, L"Could not find airplane.x", L"Mesh", MB_OK);
		return E_FAIL;
	}

	//从网格模型中提取材质属性和纹理文件名 
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];

	if( g_pMeshMaterials == NULL )
		return E_OUTOFMEMORY;

	g_pMeshTextures  = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
	if( g_pMeshTextures == NULL )
		return E_OUTOFMEMORY;

	//逐块提取网格模型材质属性和纹理文件名
	for( DWORD i=0; i<g_dwNumMaterials; i++ )
	{
		//材料属性
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;
		//设置模型材料的环境光反射系数, 因为模型材料本身没有设置环境光反射系数
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		if( d3dxMaterials[i].pTextureFilename != NULL && 
			strlen(d3dxMaterials[i].pTextureFilename) > 0 )
		{
			//获取纹理文件名
			WCHAR filename[256];
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, filename);

			//创建纹理
			if( FAILED( D3DXCreateTextureFromFile( g_pd3dDevice, filename, 
				&g_pMeshTextures[i] ) ) )
			{
				MessageBox(NULL, L"Could not find texture file", L"Mesh", MB_OK);
			}
		}
	}

	//释放在加载模型文件时创建的保存模型材质和纹理数据的缓冲区对象
	pD3DXMtrlBuffer->Release();  

	return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 释放创建的对象
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	//释放网格模型材质
	if( g_pMeshMaterials != NULL ) 
		delete[] g_pMeshMaterials;

	//释放网格模型纹理
	if( g_pMeshTextures )
	{
		for( DWORD i = 0; i < g_dwNumMaterials; i++ )
		{
			if( g_pMeshTextures[i] )
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}

	//释放网格模型对象
	if( g_pMesh != NULL )
		g_pMesh->Release();

	//释放Direct3D设备对象
	if( g_pd3dDevice != NULL )
		g_pd3dDevice->Release();

	//释放Direct3D对象
	if( g_pD3D != NULL )
		g_pD3D->Release();
}


//-----------------------------------------------------------------------------
// Desc: 渲染场景
//-----------------------------------------------------------------------------
VOID Render()
{
	// 清除缓冲区
	g_pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 
		D3DCOLOR_XRGB(0,0,255), 1.0f, 0 );

	//开始渲染场景
	if( SUCCEEDED( g_pd3dDevice->BeginScene() ) )
	{
		SetWorldMatrix();  //设置世界矩阵

		//逐块渲染网格模型
		for( DWORD i=0; i<g_dwNumShowSubMesh && i<g_dwNumMaterials; i++ )
		{
			//设置材料和纹理
			g_pd3dDevice->SetMaterial( &g_pMeshMaterials[i] );
			g_pd3dDevice->SetTexture( 0, g_pMeshTextures[i] );

			//渲染模型
			g_pMesh->DrawSubset( i );
		}

		//场景渲染结束
		g_pd3dDevice->EndScene();
	}

	//在屏幕上显示场景
	g_pd3dDevice->Present( NULL, NULL, NULL, NULL );
}


//-----------------------------------------------------------------------------
// Desc: 窗口过程, 处理消息
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
		{
			switch(wParam)
			{
			case VK_UP:
				++g_dwNumShowSubMesh;
				break;
			case  VK_DOWN:
				--g_dwNumShowSubMesh;
				break;
			}
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
	HWND hWnd = CreateWindow( L"ClassName", L"网格模型", 
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
				if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
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



