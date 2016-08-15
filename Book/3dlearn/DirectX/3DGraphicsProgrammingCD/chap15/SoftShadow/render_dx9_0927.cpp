#include "render_data.h"

#include "Gut.h"
#include "GutModel_DX9.h"
#include "GutWin32.h"

CGutModel_DX9 g_Model_DX9;

LPDIRECT3DTEXTURE9 g_pShadowmap = NULL;
LPDIRECT3DTEXTURE9 g_pShadowmapDepthStencil = NULL;

LPDIRECT3DSURFACE9 g_pMainSurface = NULL;
LPDIRECT3DSURFACE9 g_pMainDepthStencilSurface = NULL;

LPDIRECT3DSURFACE9 g_pShadowmapSurface = NULL;
LPDIRECT3DSURFACE9 g_pShadowmapDepthStencilSurface = NULL;

LPD3DXEFFECT g_pShadowEffect = NULL;

D3DXHANDLE g_pShadowmapD24Shader = NULL;
D3DXHANDLE g_pShadowmapR32Shader = NULL;

D3DXHANDLE g_pLerpShadowmapD24Shader = NULL;
D3DXHANDLE g_pLerpShadowmapR32Shader = NULL;


LPD3DXEFFECT g_pZPassEffect = NULL;
D3DXHANDLE g_pZPassShader = NULL;

static bool g_bUseD24S = false;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_light_matrix, g_remap_matrix;
static Matrix4x4 g_light_proj_matrix, g_light_view_matrix;

bool InitStateDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// 設定視角轉換矩陣
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

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	return true;
}

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DCAPS9 caps;
	device->GetDeviceCaps(&caps);

	if ( caps.PixelShaderVersion < D3DPS_VERSION(3,0) )
	{
		printf("Requires Pixel Shader 3.0\n");
		return false;
	}

	if ( !InitStateDX9() )
		return false;

	CGutModel::SetTexturePath("../../textures/");

	g_Model_DX9.ConvertToDX9Model(&g_Model);

	// 計算陰影用的Shader
	g_pShadowEffect = GutLoadFXShaderDX9("../../shaders/Shadowmap.fx");
	if ( NULL==g_pShadowEffect )
		return false;
	g_pShadowmapR32Shader = g_pShadowEffect->GetTechniqueByName("ShadowmapR32_PCF");
	//g_pShadowmapR32Shader = g_pShadowEffect->GetTechniqueByName("ShadowmapR32F");
	if ( NULL==g_pShadowmapR32Shader )
		return false;
	g_pLerpShadowmapR32Shader = g_pShadowEffect->GetTechniqueByName("ShadowmapR32_PCF_Lerp");
	if ( NULL==g_pLerpShadowmapR32Shader )
		return false;
	g_pShadowmapD24Shader = g_pShadowEffect->GetTechniqueByName("ShadowmapD24_PCF");
	if ( NULL==g_pShadowmapD24Shader )
		return false;
	g_pLerpShadowmapD24Shader = g_pShadowEffect->GetTechniqueByName("ShadowmapD24_PCF_Lerp");
	if ( NULL==g_pLerpShadowmapD24Shader )
		return false;

	// 計算陰影用的Shader
	g_pZPassEffect = GutLoadFXShaderDX9("../../shaders/ZPass.fx");
	if ( NULL==g_pZPassEffect )
		return false;
	g_pZPassShader = g_pZPassEffect->GetTechniqueByName("NonLinearZ");
	if ( NULL==g_pZPassShader )
		return false;

	g_light_proj_matrix = GutMatrixOrthoRH_DirectX(10, 10, g_fLightNearZ, g_fLightFarZ);
	//g_light_proj_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, g_fLightNearZ, g_fLightFarZ);
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

	SAFE_RELEASE(g_pShadowEffect);
	SAFE_RELEASE(g_pZPassEffect);

	g_Model_DX9.Release();

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
	// 取得Direct3D 9裝置
	InitStateDX9();
}

static void RenderShadowmapDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 記錄原本的RenderTarget & DepthBuffer
	LPDIRECT3DSURFACE9 pRenderTargetBackup = NULL;
	LPDIRECT3DSURFACE9 pZSurfaceBackup = NULL;

	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 light_matrix =  world_matrix * g_light_view_matrix * g_light_proj_matrix;

	device->SetRenderTarget(0, g_pShadowmapSurface);
	device->SetDepthStencilSurface(g_pShadowmapDepthStencilSurface);

	device->Clear(0, NULL, 
		D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
		D3DCOLOR_RGBA(255,255,255,255), 1.0f, 0);

	D3DXHANDLE h_wvp_matrix = g_pZPassEffect->GetParameterByName(NULL, "wvp_matrix");

	g_pZPassEffect->SetTechnique(g_pZPassShader);
	g_pZPassEffect->SetMatrix(h_wvp_matrix, (D3DXMATRIX *)&light_matrix);

	g_pZPassEffect->Begin(NULL, 0);
	g_pZPassEffect->BeginPass(0);
	// 只畫背面, 減少計算誤差.
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	g_Model_DX9.Render(0);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	g_pZPassEffect->EndPass();
	g_pZPassEffect->End();

	// 還原RenderTarget & DepthBuffer設定
	device->SetRenderTarget(0, g_pMainSurface);
	device->SetDepthStencilSurface(g_pMainDepthStencilSurface);
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->BeginScene(); 
	// shadowmap pass
	RenderShadowmapDX9();

	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_RGBA(32, 32, 32, 255), 1.0f, 0);

	// 更新矩陣
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 wvp_matrix = world_matrix * view_matrix * g_proj_matrix;
	// zbias矩陣用來把物件向鏡頭方向做小幅移動
	Matrix4x4 zbias; zbias.Translate_Replace(0.0f, 0.0f, g_fZBias);

	Matrix4x4 light_wvp_matrix = 
		world_matrix * g_light_view_matrix * 
		zbias * 
		g_light_proj_matrix * g_remap_matrix;

	Matrix4x4 light_wv_matrix = world_matrix * g_light_view_matrix;

	D3DXHANDLE h_wvp_matrix = g_pShadowEffect->GetParameterByName(NULL, "wvp_matrix");
	D3DXHANDLE h_light_wvp_matrix = g_pShadowEffect->GetParameterByName(NULL, "light_wvp_matrix");
	D3DXHANDLE h_light_wv_matrix = g_pShadowEffect->GetParameterByName(NULL, "light_wv_matrix");
	D3DXHANDLE h_texoffset = g_pShadowEffect->GetParameterByName(NULL, "vTexOffset");
	D3DXHANDLE h_texsize = g_pShadowEffect->GetParameterByName(NULL, "vTextureSize");
	D3DXHANDLE h_shadowmap = g_pShadowEffect->GetParameterByName(NULL, "ShadowmapTexture");

	Vector4 vTexSize(shadowmap_size, shadowmap_size, 1.0f/(float)shadowmap_size, 1.0f/(float)shadowmap_size);

	LPDIRECT3DTEXTURE9 shadowmap = NULL;

	if ( g_bUseD24S )
	{
		// 使用ZBuffer動態貼圖
		if ( g_iMode==1 )
			g_pShadowEffect->SetTechnique(g_pShadowmapD24Shader);
		else
			g_pShadowEffect->SetTechnique(g_pLerpShadowmapD24Shader);

		g_pShadowEffect->SetTexture(h_shadowmap, g_pShadowmapDepthStencil);
	}
	else
	{
		// 使用float32動態貼圖
		if ( g_iMode==1 )
			g_pShadowEffect->SetTechnique(g_pShadowmapR32Shader);
		else
			g_pShadowEffect->SetTechnique(g_pLerpShadowmapR32Shader);

		g_pShadowEffect->SetTexture(h_shadowmap, g_pShadowmap);
	}

	g_pShadowEffect->SetMatrix(h_wvp_matrix, (D3DXMATRIX *)&wvp_matrix);
	g_pShadowEffect->SetMatrix(h_light_wvp_matrix, (D3DXMATRIX *)&light_wvp_matrix);
	g_pShadowEffect->SetMatrix(h_light_wv_matrix, (D3DXMATRIX*)&light_wv_matrix);
	g_pShadowEffect->SetVector(h_texsize, (D3DXVECTOR4*)&vTexSize);

	if ( g_iMode==1 )
		g_pShadowEffect->SetVectorArray(h_texoffset, (D3DXVECTOR4*)g_vTexOffset, PCF_samples);
	else
		g_pShadowEffect->SetVectorArray(h_texoffset, (D3DXVECTOR4*)g_vTexOffset2, PCF_samples);

	// render teapot
	g_pShadowEffect->Begin(NULL, 0);
	g_pShadowEffect->BeginPass(0);
	g_Model_DX9.Render(0);
	g_pShadowEffect->EndPass();
	g_pShadowEffect->End();

	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
