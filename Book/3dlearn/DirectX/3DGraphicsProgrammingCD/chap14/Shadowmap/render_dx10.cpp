#ifdef _ENABLE_DX10_

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_DX10.h"

static ID3D10Device	*g_pDevice = NULL;

static ID3D10PixelShader *g_pZPassPS = NULL;
static ID3D10VertexShader *g_pZPassVS = NULL;
static ID3D10Blob *g_pZPassVSCode = NULL;

static ID3D10PixelShader *g_pShadowmapPS = NULL;
static ID3D10VertexShader *g_pShadowmapVS = NULL;
static ID3D10Blob *g_pShadowmapVSCode = NULL;

static ID3D10Texture2D *g_pShadowmapR32F = NULL;
static ID3D10ShaderResourceView *g_pShadowmapR32F_SRView = NULL;
static ID3D10RenderTargetView *g_pShadowmapR32F_RTView = NULL;

static ID3D10Texture2D *g_pShadowmapD32 = NULL;
static ID3D10ShaderResourceView *g_pShadowmapD32_SRView = NULL;
static ID3D10DepthStencilView *g_pShadowmapD32_RTView = NULL;

static ID3D10SamplerState *g_pShadowCompareSampler = NULL;
static ID3D10SamplerState *g_pDefaultSampler = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_light_matrix, g_remap_matrix;
static Matrix4x4 g_light_proj_matrix, g_light_view_matrix;

static CGutModel_DX10 g_Model_DX10;
static bool g_bUseZCompare = false;

bool ReInitResourceDX10(void)
{
	SAFE_RELEASE(g_pShadowmapR32F);
	SAFE_RELEASE(g_pShadowmapR32F_SRView);
	SAFE_RELEASE(g_pShadowmapR32F_RTView);

	SAFE_RELEASE(g_pShadowmapD32);
	SAFE_RELEASE(g_pShadowmapD32_SRView);
	SAFE_RELEASE(g_pShadowmapD32_RTView);

	GutCreateRenderTarget_DX10(shadowmap_size, shadowmap_size, DXGI_FORMAT_R32_FLOAT, 
		&g_pShadowmapR32F, &g_pShadowmapR32F_SRView, &g_pShadowmapR32F_RTView);

	GutCreateDepthStencil_DX10(shadowmap_size, shadowmap_size, DXGI_FORMAT_D32_FLOAT, 
		&g_pShadowmapD32, &g_pShadowmapD32_SRView, &g_pShadowmapD32_RTView);

	int w, h;
	GutGetWindowSize(w, h);
	float fAspect = (float)h/(float)w;
	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, fAspect, 0.1f, 100.0f);

	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	g_pShadowmapVS = GutLoadVertexShaderDX10_HLSL("../../shaders/Shadowmap_dx10.hlsl", "VS", "vs_4_0", &g_pShadowmapVSCode);

	if ( g_bUseZCompare )
		g_pShadowmapPS = GutLoadPixelShaderDX10_HLSL("../../shaders/Shadowmap_dx10.hlsl", "PS_SamplerCompare", "ps_4_0");
	else
		g_pShadowmapPS = GutLoadPixelShaderDX10_HLSL("../../shaders/Shadowmap_dx10.hlsl", "PS", "ps_4_0");

	if ( NULL==g_pShadowmapVS || NULL==g_pShadowmapPS )
		return false;

	g_pZPassVS = GutLoadVertexShaderDX10_HLSL("../../shaders/ZPass_dx10.hlsl", "VS", "vs_4_0", &g_pZPassVSCode);
	g_pZPassPS = GutLoadPixelShaderDX10_HLSL("../../shaders/ZPass_dx10.hlsl", "PS", "ps_4_0");

	if ( NULL==g_pZPassVS || NULL==g_pZPassPS )
		return false;

	ID3D10PixelShader  *pPS[SHADER_PERMUTATIONS];
	ID3D10VertexShader *pVS[SHADER_PERMUTATIONS];
	ID3D10Blob *pCode[SHADER_PERMUTATIONS];

	for (int i=0; i<SHADER_PERMUTATIONS/2; i++ )
	{
		pVS[i] = g_pZPassVS;
		pCode[i] = g_pZPassVSCode;
		pPS[i] = g_pZPassPS;
	}

	for ( int i=SHADER_PERMUTATIONS/2; i<SHADER_PERMUTATIONS; i++ )
	{
		pVS[i] = g_pShadowmapVS;
		pCode[i] = g_pShadowmapVSCode;
		pPS[i] = g_pShadowmapPS;
	}

	CGutModel_DX10::SetDefaultShader(pVS, pCode, pPS, SHADER_PERMUTATIONS);
	CGutModel_DX10::CreateSharedResource();

	CGutModel::SetTexturePath("../../textures/");

	g_Model_DX10.ConvertToDX10Model(&g_Model);

	{
		D3D10_SAMPLER_DESC desc;
		GutSetDX10DefaultSamplerDesc(desc);

		desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		g_pDevice->CreateSamplerState(&desc, &g_pDefaultSampler);

		desc.Filter = D3D10_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		desc.ComparisonFunc = D3D10_COMPARISON_LESS;

		g_pDevice->CreateSamplerState(&desc, &g_pShadowCompareSampler);
	}

	ReInitResourceDX10();

	g_remap_matrix.Identity();
	g_remap_matrix.Scale(0.5f, -0.5f, 1.0f);
	g_remap_matrix[3].Set(0.5f, 0.5f, 0.0f, 1.0f);

	g_light_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fLightFOV, 1.0f, g_fLightNearZ, g_fLightFarZ);
	g_light_view_matrix = GutMatrixLookAtRH(g_vLightPos, g_vLightLookAt, g_vLightUp);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pZPassPS);
	SAFE_RELEASE(g_pZPassVS);
	SAFE_RELEASE(g_pZPassVSCode);

	SAFE_RELEASE(g_pShadowmapPS);
	SAFE_RELEASE(g_pShadowmapVS);
	SAFE_RELEASE(g_pShadowmapVSCode);

	SAFE_RELEASE(g_pShadowmapR32F);
	SAFE_RELEASE(g_pShadowmapR32F_SRView);
	SAFE_RELEASE(g_pShadowmapR32F_RTView);

	SAFE_RELEASE(g_pShadowmapD32);
	SAFE_RELEASE(g_pShadowmapD32_SRView);
	SAFE_RELEASE(g_pShadowmapD32_RTView);

	SAFE_RELEASE(g_pShadowCompareSampler);
	SAFE_RELEASE(g_pDefaultSampler);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	ReInitResourceDX10();
	GutResetGraphicsDeviceDX10();

	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ID3D10ShaderResourceView *dummy_views[3] = {NULL, NULL, NULL};

	// 取得轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 ident_matrix = Matrix4x4::IdentityMatrix();
	Matrix4x4 zbias; zbias.Translate_Replace(0.0f, 0.0f, g_fZBias);

	CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
	CGutModel_DX10::SetViewMatrix(view_matrix);

	// shadowmap pass
	{
		g_pDevice->OMSetRenderTargets(1, &g_pShadowmapR32F_RTView, g_pShadowmapD32_RTView);
		D3D10_VIEWPORT viewport = {0, 0, shadowmap_size, shadowmap_size, 0.0f, 1.0f};
		g_pDevice->RSSetViewports(1, &viewport);
		// 清除顏色
		g_pDevice->ClearRenderTargetView(g_pShadowmapR32F_RTView, (float *)&vClearColor);
		g_pDevice->ClearDepthStencilView(g_pShadowmapD32_RTView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

		g_pDevice->PSSetShaderResources(0, 3, dummy_views);
		g_pDevice->PSSetSamplers(0, 1, &g_pDefaultSampler);

		CGutModel_DX10::SetProjectionMatrix(g_light_proj_matrix);
		CGutModel_DX10::SetViewMatrix(g_light_view_matrix);
		CGutModel_DX10::SetWorldMatrix(world_matrix);
		CGutModel_DX10::UpdateMatrix();

		//
		g_Model_DX10.SetShaderOverwrite(0);
		g_Model_DX10.Render();
	}

	{
		// 取得主畫面
		ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
		ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
		int w, h; GutGetWindowSize(w, h);
		g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
		D3D10_VIEWPORT viewport={0, 0, w, h, 0.0f, 1.0f};
		g_pDevice->RSSetViewports(1, &viewport);
		// 清除顏色
		g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
		g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

		g_pDevice->PSSetShaderResources(0, 1, &g_pShadowmapD32_SRView);
		// update matrix
		Matrix4x4 light_wvp_matrix = world_matrix * g_light_view_matrix * zbias * g_light_proj_matrix * g_remap_matrix;
		Matrix4x4 light_wv_matrix = world_matrix * g_light_view_matrix;

		CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
		CGutModel_DX10::SetViewMatrix(view_matrix);
		CGutModel_DX10::SetWorldMatrix(world_matrix);
		CGutModel_DX10::SetLightWVPMatrix(light_wvp_matrix);
		CGutModel_DX10::SetLightWVMatrix(light_wv_matrix);
		CGutModel_DX10::UpdateMatrix();
		//
		if ( g_bUseZCompare )
			g_pDevice->PSSetSamplers(0, 1, &g_pShadowCompareSampler);
		else
			g_pDevice->PSSetSamplers(0, 1, &g_pDefaultSampler);

		g_Model_DX10.SetTextureOverwrite(0, g_pShadowmapD32_SRView);
		g_Model_DX10.SetShaderOverwrite(SHADER_PERMUTATIONS-1);
		g_Model_DX10.Render();
		g_Model_DX10.SetTextureOverwrite(0, NULL);
	}
	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_