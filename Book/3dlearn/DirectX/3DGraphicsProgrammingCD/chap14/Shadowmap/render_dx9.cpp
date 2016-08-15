#include "render_data.h"

#include "Gut.h"
#include "GutModel_DX9.h"
#include "GutWin32.h"

CGutModel_DX9 g_Model_DX9;
CGutModel_DX9 g_SpotLightModel_DX9;

LPDIRECT3DTEXTURE9 g_pShadowmap = NULL;
LPDIRECT3DTEXTURE9 g_pShadowmapDepthStencil = NULL;

LPDIRECT3DSURFACE9 g_pShadowmapSurface = NULL;
LPDIRECT3DSURFACE9 g_pShadowmapDepthStencilSurface = NULL;

LPDIRECT3DSURFACE9 g_pMainSurface = NULL;
LPDIRECT3DSURFACE9 g_pMainDepthStencilSurface = NULL;

LPDIRECT3DVERTEXSHADER9 g_pZPassVS = NULL;
LPDIRECT3DPIXELSHADER9 g_pZPassPS = NULL;

LPDIRECT3DVERTEXSHADER9 g_pShadowmapVS = NULL;
LPDIRECT3DPIXELSHADER9  g_pShadowmapPS_R32F = NULL;
LPDIRECT3DPIXELSHADER9  g_pShadowmapPS_D24S = NULL;

static bool g_bUseD24S = false;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_light_matrix, g_remap_matrix;
static Matrix4x4 g_light_proj_matrix, g_light_view_matrix;

bool InitStateDX9(void)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	// `設定視角轉換矩陣`
	int w, h;
	GutGetWindowSize(w, h);
	float fAspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, fAspect, 0.1f, 100.0f);

	device->GetRenderTarget(0, &g_pMainSurface);
	device->GetDepthStencilSurface(&g_pMainDepthStencilSurface);

	device->CreateTexture(
		shadowmap_size, shadowmap_size, 
		1, D3DUSAGE_RENDERTARGET, 
		D3DFMT_R32F, D3DPOOL_DEFAULT, 
		&g_pShadowmap, NULL);

	if ( g_pShadowmap==NULL )
	{
		device->CreateTexture(
			shadowmap_size, shadowmap_size, 
			1, D3DUSAGE_RENDERTARGET, 
			D3DFMT_R16F, D3DPOOL_DEFAULT, 
			&g_pShadowmap, NULL);
	}

	device->CreateTexture(
		shadowmap_size, shadowmap_size, 
		1, D3DUSAGE_DEPTHSTENCIL, 
		D3DFMT_D24S8, D3DPOOL_DEFAULT, 
		&g_pShadowmapDepthStencil, NULL);

	g_pShadowmap->GetSurfaceLevel(0, &g_pShadowmapSurface);

	if ( g_pShadowmapDepthStencil )
	{
		g_pShadowmapDepthStencil->GetSurfaceLevel(0, &g_pShadowmapDepthStencilSurface);
	}
	else
	{
		device->CreateDepthStencilSurface(shadowmap_size, shadowmap_size, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &g_pShadowmapDepthStencilSurface, NULL);
		if ( g_pShadowmapDepthStencilSurface==NULL )
			return false;

		g_bUseD24S = false;
	}

	if ( g_bUseD24S )
	{
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	}
	else
	{
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
	}

	return true;
}

bool InitResourceDX9(void)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	InitStateDX9();

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model);

	// `畫出距離值的Shader`
	g_pZPassVS = GutLoadVertexShaderDX9_HLSL("../../shaders/ZPass.hlsl", "VS", "vs_2_0");
	g_pZPassPS = GutLoadPixelShaderDX9_HLSL ("../../shaders/ZPass.hlsl", "PS", "ps_2_0");

	if ( NULL==g_pZPassVS || NULL==g_pZPassPS )
		return false;

	// `計算陰影用的Shader`
	g_pShadowmapVS = GutLoadVertexShaderDX9_HLSL("../../shaders/shadowmap.hlsl", "VS", "vs_2_0");
	g_pShadowmapPS_R32F = GutLoadPixelShaderDX9_HLSL ("../../shaders/shadowmap.hlsl", "PS_R32F", "ps_2_0");
	g_pShadowmapPS_D24S = GutLoadPixelShaderDX9_HLSL ("../../shaders/shadowmap.hlsl", "PS_D24S8", "ps_2_0");

	if ( NULL==g_pShadowmapVS || NULL==g_pShadowmapPS_R32F || NULL==g_pShadowmapPS_D24S )
		return false;

	g_light_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fLightFOV, 1.0f, g_fLightNearZ, g_fLightFarZ);
	g_light_view_matrix = GutMatrixLookAtRH(g_vLightPos, g_vLightLookAt, g_vLightUp);

	float offset = 0.5f/(float)shadowmap_size;

	g_remap_matrix.Identity();
	g_remap_matrix.Scale(0.5f, -0.5f, 1.0f);
	g_remap_matrix[3].Set(0.5f+offset, 0.5f+offset, 0.0f, 1.0f);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pMainSurface);
	SAFE_RELEASE(g_pMainDepthStencilSurface);

	SAFE_RELEASE(g_pShadowmapSurface);
	SAFE_RELEASE(g_pShadowmapDepthStencilSurface);

	SAFE_RELEASE(g_pShadowmap);
	SAFE_RELEASE(g_pShadowmapDepthStencil);

	SAFE_RELEASE(g_pZPassVS);
	SAFE_RELEASE(g_pZPassPS);

	SAFE_RELEASE(g_pShadowmapVS);
	SAFE_RELEASE(g_pShadowmapPS_D24S);
	SAFE_RELEASE(g_pShadowmapPS_R32F);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	SAFE_RELEASE(g_pShadowmapSurface);
	SAFE_RELEASE(g_pShadowmapDepthStencilSurface);

	SAFE_RELEASE(g_pShadowmap);
	SAFE_RELEASE(g_pShadowmapDepthStencil);

	// Reset Device
	GutResetGraphicsDeviceDX9();

	InitStateDX9();
}

static void RenderShadowmapDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 light_matrix =  world_matrix * g_light_view_matrix * g_light_proj_matrix;

	device->SetRenderTarget(0, g_pShadowmapSurface);
	device->SetDepthStencilSurface(g_pShadowmapDepthStencilSurface);

	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		D3DCOLOR_RGBA(255,255,255,255), 1.0f, 0);

	device->SetVertexShaderConstantF(0, (float *)&light_matrix, 4);

	CGutModel_DX9::SetShaderOverwrite(g_pZPassVS, g_pZPassPS);
	g_Model_DX9.Render();
	CGutModel_DX9::SetShaderOverwrite(NULL, NULL);

	// `還原` RenderTarget & DepthBuffer `設定`
	device->SetRenderTarget(0, g_pMainSurface);
	device->SetDepthStencilSurface(g_pMainDepthStencilSurface);
}

// `使用Direct3D9來繪圖`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->BeginScene(); 
	// shadowmap pass
	RenderShadowmapDX9();

	// `消除畫面`
	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_RGBA(32, 32, 32, 255), 1.0f, 0);

	// `更新矩陣`
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 wvp_matrix = world_matrix * view_matrix * g_proj_matrix;
	// zbias `矩陣用來把物件向鏡頭方向做小幅移動`
	Matrix4x4 zbias; zbias.Translate_Replace(0.0f, 0.0f, g_fZBias);

	Matrix4x4 light_wvp_matrix = 
		world_matrix * g_light_view_matrix * 
		zbias * 
		g_light_proj_matrix * g_remap_matrix;

	Matrix4x4 light_wv_matrix = world_matrix * g_light_view_matrix;

	device->SetVertexShaderConstantF(0, (float *)&wvp_matrix, 4);
	device->SetVertexShaderConstantF(4, (float *)&light_wvp_matrix, 4);
	device->SetVertexShaderConstantF(8, (float *)&light_wv_matrix, 4);

	if ( g_bUseD24S )
	{
		// `使用ZBuffer動態貼圖`
		CGutModel_DX9::SetShaderOverwrite(g_pShadowmapVS, g_pShadowmapPS_D24S);
		CGutModel_DX9::SetTextureOverwrite(0, g_pShadowmapDepthStencil);
	}
	else
	{
		// `使用float32動態貼圖`
		CGutModel_DX9::SetShaderOverwrite(g_pShadowmapVS, g_pShadowmapPS_R32F);
		CGutModel_DX9::SetTextureOverwrite(0, g_pShadowmap);
	}

	g_Model_DX9.Render();

	CGutModel_DX9::SetTextureOverwrite(0, NULL);
	CGutModel_DX9::SetShaderOverwrite(NULL, NULL);

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 
	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
