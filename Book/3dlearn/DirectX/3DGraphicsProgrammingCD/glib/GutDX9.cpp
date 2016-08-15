//#define STRICT
#include <windows.h>
#include <D3D9.h>
#include <D3DX9.h>
#include <stdio.h>

#include "GutWin32.h"
#include "GutDX9.h"
#include "Gut.h"

static LPDIRECT3D9 g_pD3D = NULL;
static LPDIRECT3DDEVICE9 g_pD3DDevice = NULL;
static D3DPRESENT_PARAMETERS g_pD3DPresent;

LPDIRECT3DDEVICE9 GutGetGraphicsDeviceDX9(void)
{
	// 直接傳回已經初始化好的Direct3D 9裝置
	return g_pD3DDevice;
}

// Direct3D9 初使化.
bool GutInitGraphicsDeviceDX9(GutDeviceSpec *pSpec)
{
	// 開啟視窗時就已取得這個用來代表視窗的指標, 直接拿來使用.
	HWND hWnd = GutGetWindowHandleWin32();

	RECT rect;
	GetWindowRect(hWnd, &rect);

	float w = rect.right - rect.left;
	float h = rect.bottom - rect.top;
	if ( w==0 || h==0 )
		return false;

	int multisamples = 0;
	if ( pSpec )
		multisamples = pSpec->m_iMultiSamples;

	// 取得一個D3D9物件, 它的唯一功用是去開啟真正可以拿來繪圖的D3D9 Device.
	if( NULL == ( g_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
		return false;

	ZeroMemory( &g_pD3DPresent, sizeof(g_pD3DPresent) );
	g_pD3DPresent.Windowed = TRUE;
	g_pD3DPresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
	g_pD3DPresent.BackBufferFormat = D3DFMT_UNKNOWN;
	g_pD3DPresent.BackBufferCount = 1; // 提供一塊backbuffer
	g_pD3DPresent.EnableAutoDepthStencil = TRUE;
	g_pD3DPresent.AutoDepthStencilFormat = D3DFMT_D24S8;
	g_pD3DPresent.MultiSampleType = (D3DMULTISAMPLE_TYPE) multisamples;
	g_pD3DPresent.hDeviceWindow = hWnd;

	bool device_initialized = false;

	/*
	試著用4種不同方法來開啟d3d9
	1. D3DDEVTYPE_HAL + D3DCREATE_HARDWARE_VERTEXPROCESSING 
	完全交由GPU處理
	2. D3DDEVTYPE_HAL + D3DCREATE_MIXED_VERTEXPROCESSING	
	Vertex可由CPU或GPU來處理, 其它是GPU.
	3. D3DDEVTYPE_HAL + D3DCREATE_SOFTWARE_VERTEXPROCESSING 
	Vertex是CPU, 其它是GPU.
	4. D3DDEVTYPE_REF + D3DCREATE_SOFTWARE_VERTEXPROCESSING	
	完全由CPU處理, 這是個非常慢的模式.
	*/

	const int device_types = 4;

	struct sDeviceType
	{
		D3DDEVTYPE type;
		DWORD behavior;
	};

	sDeviceType device_type[device_types] = 
	{
		{D3DDEVTYPE_HAL, D3DCREATE_HARDWARE_VERTEXPROCESSING},
		{D3DDEVTYPE_HAL, D3DCREATE_MIXED_VERTEXPROCESSING},
		{D3DDEVTYPE_HAL, D3DCREATE_SOFTWARE_VERTEXPROCESSING},
		{D3DDEVTYPE_REF, D3DCREATE_SOFTWARE_VERTEXPROCESSING}
	};

	for ( int type=0; type<device_types; type++ )
	{
		// 試著去開啟一個Direct3D9裝置
		if( g_pD3D->CreateDevice( D3DADAPTER_DEFAULT, device_type[type].type, hWnd,
			device_type[type].behavior, &g_pD3DPresent, &g_pD3DDevice )==D3D_OK )
		{
			device_initialized = true;
			break;
		}
	}

	return device_initialized;
}

bool GutResetGraphicsDeviceDX9(void)
{
	RECT rect;
	GetWindowRect( GutGetWindowHandleWin32(), &rect);

	g_pD3DPresent.BackBufferWidth = 0;
	g_pD3DPresent.BackBufferHeight = 0;
	g_pD3DPresent.BackBufferCount = 1; // 提供一塊backbuffer

	if ( D3D_OK==g_pD3DDevice->Reset(&g_pD3DPresent) )
		return true;
	else
		return false;
}

bool GutReleaseGraphicsDeviceDX9(void)
{
	if ( g_pD3DDevice )
	{
		g_pD3DDevice->Release();
		g_pD3DDevice = NULL;
	}

	if ( g_pD3D )
	{
		g_pD3D->Release();
		g_pD3D = NULL;
	}

	return true;
}

// 載入Vertex Shader
// file = HLSL shader file
// entry = vertx shader entry point
// profile = shader version
LPDIRECT3DVERTEXSHADER9 GutLoadVertexShaderDX9_HLSL(const char *filename, const char *entry, const char *profile)
{
	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pErrorMsg = NULL;
	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	char filename_fullpath[256];
	sprintf(filename_fullpath, "%s%s", GutGetShaderPath(), filename);

	HRESULT result = D3DXCompileShaderFromFile(
		filename_fullpath,
		NULL,
		NULL,
		entry,
		profile,
		flags,
		&pShader,
		&pErrorMsg,
		NULL
		);

	if ( result != S_OK )
	{
		if ( pErrorMsg )
		{
			printf("%s\n", pErrorMsg->GetBufferPointer() );
			pErrorMsg->Release();
		}
		return NULL;
	}

	LPDIRECT3DVERTEXSHADER9 pVertexShader = NULL;
	result = g_pD3DDevice->CreateVertexShader( (DWORD*)pShader->GetBufferPointer(), &pVertexShader);
	if ( result!=S_OK )
	{
		printf("Failed to load vertex shader %s\n", filename);	
	}

	pShader->Release();

	return pVertexShader;
}

// 載入Pixel Shader
// file = HLSL shader file
// entry = pixel shader entry point
// profile = shader version
LPDIRECT3DPIXELSHADER9  GutLoadPixelShaderDX9_HLSL(const char *filename, const char *entry, const char *profile)
{
	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pErrorMsg = NULL;
	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	char filename_fullpath[256];
	sprintf(filename_fullpath, "%s%s", GutGetShaderPath(), filename);

	HRESULT result = D3DXCompileShaderFromFile(
		filename_fullpath,
		NULL,
		NULL,
		entry,
		profile,
		flags,
		&pShader,
		&pErrorMsg,
		NULL
		);

	if ( result != S_OK )
	{
		if ( pErrorMsg )
		{
			printf("%s\n", pErrorMsg->GetBufferPointer() );
			pErrorMsg->Release();
		}
		return NULL;
	}

	LPDIRECT3DPIXELSHADER9 pPixelShader = NULL;
	result = g_pD3DDevice->CreatePixelShader( (DWORD*)pShader->GetBufferPointer(), &pPixelShader);
	if ( result!=S_OK )
	{
		printf("Failed to load pixel shader %s\n", filename);	
	}

	pShader->Release();

	return pPixelShader;
}

LPDIRECT3DVERTEXSHADER9 GutLoadVertexShaderDX9_ASM(const char *filename)
{
	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pErrorMsg = NULL;
	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	char filename_fullpath[256];
	sprintf(filename_fullpath, "%s%s", GutGetShaderPath(), filename);

	HRESULT result = D3DXAssembleShaderFromFile(
		filename_fullpath,
		NULL,
		NULL,
		flags,
		&pShader,
		&pErrorMsg
		);

	if ( result != S_OK )
	{
		if ( pErrorMsg )
		{
			printf("%s\n", pErrorMsg->GetBufferPointer() );
			pErrorMsg->Release();
		}
	}

	LPDIRECT3DVERTEXSHADER9 pVertexShader = NULL;
	result = g_pD3DDevice->CreateVertexShader( (DWORD*)pShader->GetBufferPointer(), &pVertexShader);
	if ( result!=S_OK )
	{
		printf("Failed to load vertex shader %s\n", filename);	
	}

	pShader->Release();

	return pVertexShader;
}

LPDIRECT3DPIXELSHADER9  GutLoadPixelShaderDX9_ASM(const char *filename)
{
	LPD3DXBUFFER pShader = NULL;
	LPD3DXBUFFER pErrorMsg = NULL;
	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	char filename_fullpath[256];
	sprintf(filename_fullpath, "%s%s", GutGetShaderPath(), filename);

	HRESULT result = D3DXAssembleShaderFromFile(
		filename_fullpath,
		NULL,
		NULL,
		flags,
		&pShader,
		&pErrorMsg
		);

	if ( result != S_OK )
	{
		if ( pErrorMsg )
		{
			printf("%s\n", pErrorMsg->GetBufferPointer() );
			pErrorMsg->Release();
		}
	}

	LPDIRECT3DPIXELSHADER9 pPixelShader = NULL;
	result = g_pD3DDevice->CreatePixelShader( (DWORD*)pShader->GetBufferPointer(), &pPixelShader);
	if ( result!=S_OK )
	{
		printf("Failed to load pixel shader %s\n", filename);	
	}

	pShader->Release();

	return pPixelShader;
}

LPDIRECT3DVERTEXSHADER9 GutLoadVertexShaderDX9_Binary(const char *filename)
{
	return NULL;
}

LPDIRECT3DPIXELSHADER9  GutLoadPixelShaderDX9_Binary(const char *filename)
{
	return NULL;
}

void GutSetupLightDX9(int index, sGutLight &light)
{
	D3DLIGHT9 d3dlight;
	ZeroMemory( &d3dlight, sizeof(d3dlight) );

	switch(light.m_iLightType)
	{
	case LIGHT_POINT:
		d3dlight.Type = D3DLIGHT_POINT;
		break;
	case LIGHT_DIRECTIONAL:
		d3dlight.Type = D3DLIGHT_DIRECTIONAL;
		break;
	case LIGHT_SPOT:
		d3dlight.Type = D3DLIGHT_SPOT;
		break;
	}

	d3dlight.Position = *(D3DVECTOR*) &light.m_vPosition;
	d3dlight.Direction = *(D3DVECTOR*) &light.m_vDirection;
	d3dlight.Ambient = *(D3DCOLORVALUE*) &light.m_vAmbient;
	d3dlight.Diffuse  = *(D3DCOLORVALUE*) &light.m_vDiffuse;
	d3dlight.Specular = *(D3DCOLORVALUE*) &light.m_vSpecular;

	d3dlight.Attenuation0 = light.m_vAttenuation[0];
	d3dlight.Attenuation1 = light.m_vAttenuation[1];
	d3dlight.Attenuation2 = light.m_vAttenuation[2];

	d3dlight.Range = 1e10;

	g_pD3DDevice->SetLight(index, &d3dlight);
	g_pD3DDevice->LightEnable(index, light.m_bEnabled ? TRUE : FALSE);
}

LPD3DXEFFECT GutLoadFXShaderDX9(const char *filename)
{
	LPD3DXEFFECT pEffect;
	LPD3DXBUFFER pErrorMsg = NULL;
	DWORD flags = 0;
#ifdef _DEBUG
	flags |= D3DXSHADER_DEBUG;
#endif

	char filename_fullpath[256];
	sprintf(filename_fullpath, "%s%s", GutGetShaderPath(), filename);

	if ( D3D_OK!=D3DXCreateEffectFromFile(g_pD3DDevice, filename_fullpath, NULL, NULL, flags, NULL, &pEffect, &pErrorMsg) )
	{
		if ( pErrorMsg )
		{
			printf("%s\n", pErrorMsg->GetBufferPointer() );
		}
	}

	SAFE_RELEASE(pErrorMsg);

	return pEffect;
}
