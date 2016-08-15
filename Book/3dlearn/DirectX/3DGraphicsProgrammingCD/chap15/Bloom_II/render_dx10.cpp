#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutWin32.h"
#include "GutDX10.h"
#include "GutTexture_DX10.h"
#include "GutModel_DX10.h"

#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;
static ID3D10Buffer			*g_pVertexBuffer	= NULL;

static ID3D10Effect			*g_pEffect = NULL;
static ID3D10EffectTechnique *g_pBlurShader = NULL;
static ID3D10EffectTechnique *g_pBrightnessShader = NULL;
static ID3D10EffectTechnique *g_pImageShader = NULL;

static sImageInfo g_ImageInfo;
static sImageInfo g_DownSampledImageInfo;

static ID3D10ShaderResourceView *g_pTexture = NULL;
static ID3D10DepthStencilState *g_pZWrite = NULL;

static ID3D10Texture2D *g_pFrameTexture[3] = {NULL, NULL, NULL};
static ID3D10ShaderResourceView *g_pFrameSRView[3] = {NULL, NULL, NULL};
static ID3D10RenderTargetView *g_pFrameRTView[3] = {NULL, NULL, NULL};

static CGutModel_DX10 g_ModelSun_DX10;
static CGutModel_DX10 g_ModelEarth_DX10;

static Matrix4x4 g_proj_matrix;

bool ReInitResourceDX10(void)
{
	int w, h;
	GutGetWindowSize(w, h);

	g_ImageInfo.m_iWidth = w;
	g_ImageInfo.m_iHeight = h;

	g_DownSampledImageInfo.m_iWidth = w/4;
	g_DownSampledImageInfo.m_iHeight = h/4;

	for ( int i=0; i<3; i++ )
	{
		SAFE_RELEASE(g_pFrameTexture[i]);
		SAFE_RELEASE(g_pFrameSRView[i]);
		SAFE_RELEASE(g_pFrameRTView[i]);
	}

	if ( !GutCreateRenderTarget_DX10(w/4, h/4, 
		DXGI_FORMAT_R8G8B8A8_UNORM, &g_pFrameTexture[0], &g_pFrameSRView[0], &g_pFrameRTView[0]) )
		return false;

	if ( !GutCreateRenderTarget_DX10(w/4, h/4,
		DXGI_FORMAT_R8G8B8A8_UNORM, &g_pFrameTexture[1], &g_pFrameSRView[1], &g_pFrameRTView[1]) )
		return false;

	if ( !GutCreateRenderTarget_DX10(w, h,
		DXGI_FORMAT_R8G8B8A8_UNORM, &g_pFrameTexture[2], &g_pFrameSRView[2], &g_pFrameRTView[2]) )
		return false;

	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNearZ, g_fFarZ);

	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Shader
	{
		g_pEffect = GutLoadFXShaderDX10("../../shaders/Posteffect_dx10.fx");
		if ( NULL==g_pEffect )
			return false;

		g_pBlurShader = g_pEffect->GetTechniqueByName("BlurImage");
		g_pBrightnessShader = g_pEffect->GetTechniqueByName("Brightness");
		g_pImageShader = g_pEffect->GetTechniqueByName("AddImage");

		if ( NULL==g_pBlurShader || NULL==g_pBrightnessShader || NULL==g_pImageShader )
			return false;
	}

	// 設定Vertex資料格式
	{
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		D3D10_PASS_DESC PassDesc;
		g_pBlurShader->GetPassByIndex(0)->GetDesc(&PassDesc);

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	if ( !ReInitResourceDX10() )
		return false;

	ID3D10EffectDepthStencilVariable *pZEnableVar = g_pEffect->GetVariableByName("ZEnable")->AsDepthStencil();
	pZEnableVar->GetDepthStencilState(0, &g_pZWrite);

	CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl");

	g_ModelEarth_DX10.ConvertToDX10Model(&g_ModelEarth);
	g_ModelSun_DX10.ConvertToDX10Model(&g_ModelSun);

	g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(Vertex_VT)*4, g_FullScreenQuad);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexBuffer);

	SAFE_RELEASE(g_pEffect);

	for ( int i=0; i<3; i++ )
	{
		SAFE_RELEASE(g_pFrameTexture[i]);
		SAFE_RELEASE(g_pFrameSRView[i]);
		SAFE_RELEASE(g_pFrameRTView[i]);
	}

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	ReInitResourceDX10();
}

static void DrawImage(ID3D10ShaderResourceView *pTexture)
{
	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	ID3D10EffectPass *pImagePass = g_pImageShader->GetPassByIndex(0);
	ID3D10EffectShaderResourceVariable *pInputTexture = g_pEffect->GetVariableByName("Image")->AsShaderResource();

	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	pInputTexture->SetResource(pTexture);

	g_pImageShader->GetPassByIndex(0)->Apply(0);

	g_pDevice->IASetInputLayout(g_pVertexLayout);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDevice->Draw(4, 0);
}

static ID3D10ShaderResourceView *BrightnessImage(ID3D10ShaderResourceView *pTexture, sImageInfo *pInfo)
{
	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	device->OMSetRenderTargets(1, &g_pFrameRTView[1], NULL);

	D3D10_VIEWPORT vp = {0, 0, w, h, 0.0f, 1.0f};
	device->RSSetViewports(1, &vp);

	ID3D10EffectTechnique *pShader = g_pEffect->GetTechniqueByName("Brightness");

	ID3D10EffectVectorVariable *pColorOffset = g_pEffect->GetVariableByName("IntensityOffset")->AsVector();
	ID3D10EffectVectorVariable *pColorScale = g_pEffect->GetVariableByName("IntensityScale")->AsVector();
	ID3D10EffectShaderResourceVariable *pSRView = g_pEffect->GetVariableByName("Image")->AsShaderResource();

	pColorOffset->SetFloatVector(&g_vBrightnessOffset[0]);
	pColorScale->SetFloatVector(&g_vBrightnessScale[0]);
	pSRView->SetResource(pTexture);

	pShader->GetPassByIndex(0)->Apply(0);

	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	g_pDevice->IASetInputLayout(g_pVertexLayout);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDevice->PSSetShaderResources(0, 1, &pTexture);

	g_pDevice->Draw(4, 0);

	return g_pFrameSRView[1];
}

static ID3D10ShaderResourceView *BlurImage(ID3D10ShaderResourceView *pTexture, sImageInfo *pInfo)
{
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;
	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

	D3D10_VIEWPORT vp = {0, 0, w, h, 0.0f, 1.0f};
	g_pDevice->RSSetViewports(1, &vp);

	const int num_samples = KERNELSIZE;
	Vector4 vTexOffsetX[num_samples];
	Vector4 vTexOffsetY[num_samples];

	for ( int i=0; i<num_samples; i++ )
	{
		vTexOffsetX[i].Set(g_uv_offset_table[i] * fTexelW, 0.0f, 0.0f, g_weight_table[i]);
		vTexOffsetY[i].Set(0.0f, g_uv_offset_table[i] * fTexelH, 0.0f, g_weight_table[i]);
	}

	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	ID3D10EffectPass *pBlurPass = g_pBlurShader->GetPassByIndex(0);
	ID3D10EffectVectorVariable *pTexOffset = g_pEffect->GetVariableByName("vTexOffset")->AsVector();
	ID3D10EffectShaderResourceVariable *pSRView = g_pEffect->GetVariableByName("Image")->AsShaderResource();

	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	g_pDevice->IASetInputLayout(g_pVertexLayout);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 水平模糊
	{
		device->OMSetRenderTargets(1, &g_pFrameRTView[0], NULL);

		pTexOffset->SetFloatVectorArray(&vTexOffsetX[0][0], 0, num_samples);
		pSRView->SetResource(pTexture);
		pBlurPass->Apply(0);

		g_pDevice->Draw(4, 0);
	}
	// 垂直模糊
	{
		device->OMSetRenderTargets(1, &g_pFrameRTView[1], NULL);

		pTexOffset->SetFloatVectorArray(&vTexOffsetY[0][0], 0, num_samples);
		pSRView->SetResource(g_pFrameSRView[0]);
		pBlurPass->Apply(0);

		g_pDevice->Draw(4, 0);
	}

	return g_pFrameSRView[1];
}

void RenderFrameDX10(void)
{
	int w, h;
	GutGetWindowSize(w, h);

	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	g_pDevice->ClearRenderTargetView(pRenderTargetView, &vClearColor[0]);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);

	D3D10_VIEWPORT vp = {0, 0, w, h, 0.0f, 1.0f};
	g_pDevice->RSSetViewports(1, &vp);

	g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	g_pDevice->OMSetDepthStencilState(g_pZWrite, 0);

	CGutModel_DX10::EnableLighting(true);
	CGutModel_DX10::SetLight(0, g_Light0);

	CGutModel_DX10::s_light.m_vGlobalAmbientLight = g_vAmbientLight;
	CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
	CGutModel_DX10::SetViewMatrix(g_Control.GetViewMatrix());

	CGutModel_DX10::SetWorldMatrix(g_sun_matrix);
	CGutModel_DX10::UpdateMatrix();
	g_ModelSun_DX10.Render();

	CGutModel_DX10::SetWorldMatrix(g_earth_matrix);
	CGutModel_DX10::UpdateMatrix();
	g_ModelEarth_DX10.Render();

	if ( g_bPosteffect )
	{
		g_pDevice->OMSetRenderTargets(1, &g_pFrameRTView[2], pDepthStencilView);
		g_pDevice->ClearRenderTargetView(g_pFrameRTView[2], &vClearColor[0]);

		CGutModel_DX10::SetWorldMatrix(g_sun_matrix);
		CGutModel_DX10::UpdateMatrix();
		g_ModelSun_DX10.Render();

		ID3D10ShaderResourceView *pRSView = g_pFrameSRView[2];
		pRSView = BrightnessImage(pRSView, &g_DownSampledImageInfo);
		pRSView = BlurImage(pRSView, &g_DownSampledImageInfo);

		g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, NULL);
		g_pDevice->RSSetViewports(1, &vp);

		DrawImage(pRSView);
	}

	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain();
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_