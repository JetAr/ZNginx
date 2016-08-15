#ifdef _ENABLE_DX10_

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_DX10.h"

static ID3D10Device	*g_pDevice = NULL;

static ID3D10Effect			*g_pZPassEffect = NULL;
static ID3D10EffectTechnique *g_pZPassShader = NULL;

static ID3D10Effect *g_pShadowEffect = NULL;
static ID3D10EffectTechnique *g_pShadowShader = NULL;

static ID3D10InputLayout *g_pZPassLayout = NULL;
static ID3D10InputLayout *g_pShadowLayout = NULL;

static ID3D10Texture2D *g_pShadowmapR32F = NULL;
static ID3D10ShaderResourceView *g_pShadowmapR32F_SRView = NULL;
static ID3D10RenderTargetView *g_pShadowmapR32F_RTView = NULL;

static ID3D10Texture2D *g_pShadowmapD32 = NULL;
static ID3D10ShaderResourceView *g_pShadowmapD32_SRView = NULL;
static ID3D10DepthStencilView *g_pShadowmapD32_RTView = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_light_matrix, g_remap_matrix;
static Matrix4x4 g_light_proj_matrix, g_light_view_matrix;

static CGutModel_DX10 g_Model_DX10;
static bool g_bUseZCompare = false;

bool ReInitResourceDX10(void)
{
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

	g_pZPassEffect = GutLoadFXShaderDX10("../../shaders/ZPass_dx10.fx");
	if ( NULL==g_pZPassEffect )
		return false;
	g_pZPassShader = g_pZPassEffect->GetTechniqueByName("NonLinearZ");
	if ( NULL==g_pZPassShader )
		return false;

	g_pShadowEffect = GutLoadFXShaderDX10("../../shaders/Shadowmap_dx10.fx");
	if ( NULL==g_pShadowEffect )
		return false;
	g_pShadowShader = g_pShadowEffect->GetTechniqueByName("ShadowmapR32_PCF");
	if ( NULL==g_pShadowShader )
		return false;

	const sVertexDecl *pVertexDecl = g_Model.GetVertexFormat();

	D3D10_PASS_DESC PassDesc;

	g_pZPassShader->GetPassByIndex(0)->GetDesc(&PassDesc);
	g_pZPassLayout = GutCreateInputLayoutDX10(pVertexDecl, 
		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize);

	g_pShadowShader->GetPassByIndex(0)->GetDesc(&PassDesc);
	g_pShadowLayout = GutCreateInputLayoutDX10(pVertexDecl, 
		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize);

	g_Model_DX10.ConvertToDX10Model(&g_Model);

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
	SAFE_RELEASE(g_pShadowLayout);
	SAFE_RELEASE(g_pZPassLayout);

	SAFE_RELEASE(g_pZPassEffect);
	SAFE_RELEASE(g_pShadowEffect);

	SAFE_RELEASE(g_pShadowmapR32F);
	SAFE_RELEASE(g_pShadowmapR32F_SRView);
	SAFE_RELEASE(g_pShadowmapR32F_RTView);

	SAFE_RELEASE(g_pShadowmapD32);
	SAFE_RELEASE(g_pShadowmapD32_SRView);
	SAFE_RELEASE(g_pShadowmapD32_RTView);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	SAFE_RELEASE(g_pShadowmapR32F);
	SAFE_RELEASE(g_pShadowmapR32F_SRView);
	SAFE_RELEASE(g_pShadowmapR32F_RTView);

	SAFE_RELEASE(g_pShadowmapD32);
	SAFE_RELEASE(g_pShadowmapD32_SRView);
	SAFE_RELEASE(g_pShadowmapD32_RTView);

	GutResetGraphicsDeviceDX10();

	ReInitResourceDX10();
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ID3D10ShaderResourceView *dummy_views[3] = {NULL, NULL, NULL};

	// 取得轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wvp_matrix;
	Matrix4x4 ident_matrix = Matrix4x4::IdentityMatrix();
	Matrix4x4 zbias; zbias.Translate_Replace(0.0f, 0.0f, g_fZBias);

	wvp_matrix = world_matrix * g_light_view_matrix * g_light_proj_matrix;

	ID3D10EffectMatrixVariable *pWVPMatrixVar = g_pZPassEffect->GetVariableByName("wvp_matrix")->AsMatrix();
	pWVPMatrixVar->SetMatrix(&wvp_matrix[0][0]);

	// shadowmap pass
	{
		g_pDevice->OMSetRenderTargets(1, &g_pShadowmapR32F_RTView, g_pShadowmapD32_RTView);
		D3D10_VIEWPORT viewport = {0, 0, shadowmap_size, shadowmap_size, 0.0f, 1.0f};
		g_pDevice->RSSetViewports(1, &viewport);
		// 清除顏色
		g_pDevice->ClearRenderTargetView(g_pShadowmapR32F_RTView, (float *)&vClearColor);
		g_pDevice->ClearDepthStencilView(g_pShadowmapD32_RTView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

		g_pDevice->PSSetShaderResources(0, 3, dummy_views);

		g_pDevice->IASetInputLayout(g_pZPassLayout);
		g_pZPassShader->GetPassByIndex(0)->Apply(0);
		g_Model_DX10.Render(0);
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

		// update matrix
		Matrix4x4 light_wvp_matrix = world_matrix * g_light_view_matrix * zbias * g_light_proj_matrix * g_remap_matrix;
		Matrix4x4 light_wv_matrix = world_matrix * g_light_view_matrix;
		wvp_matrix = world_matrix * view_matrix * g_proj_matrix;

		ID3D10EffectMatrixVariable *pWVPMatrixVar = g_pShadowEffect->GetVariableByName("wvp_matrix")->AsMatrix();
		ID3D10EffectMatrixVariable *pLightWVPMatrixVar = g_pShadowEffect->GetVariableByName("light_wvp_matrix")->AsMatrix();
		ID3D10EffectMatrixVariable *pLightWVMatrixVar = g_pShadowEffect->GetVariableByName("light_wv_matrix")->AsMatrix();
		ID3D10EffectVectorVariable *pTexOffsetVar = g_pShadowEffect->GetVariableByName("vTexOffset")->AsVector();
		ID3D10EffectShaderResourceVariable *pShadowmapVar = g_pShadowEffect->GetVariableByName("Shadowmap")->AsShaderResource();

		pWVPMatrixVar->SetMatrix(&wvp_matrix[0][0]);
		pLightWVPMatrixVar->SetMatrix(&light_wvp_matrix[0][0]);
		pLightWVMatrixVar->SetMatrix(&light_wv_matrix[0][0]);

		pTexOffsetVar->SetFloatVectorArray(&g_vTexOffset[0][0], 0, PCF_samples);
		pShadowmapVar->SetResource(g_pShadowmapD32_SRView);

		g_pDevice->IASetInputLayout(g_pShadowLayout);
		g_pShadowShader->GetPassByIndex(0)->Apply(0);

		g_Model_DX10.Render(0);
	}

	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_