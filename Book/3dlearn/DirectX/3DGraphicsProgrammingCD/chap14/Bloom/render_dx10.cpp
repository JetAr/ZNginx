#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutWin32.h"
#include "GutDX10.h"
#include "GutTexture_DX10.h"

#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer		= NULL;
static ID3D10Buffer			*g_pVSConstantBuffer	= NULL;
static ID3D10Buffer			*g_pBlurConstantBuffer	= NULL;
static ID3D10Buffer			*g_pBrightnessConstantBuffer	= NULL;

static ID3D10VertexShader	*g_pBlurVS = NULL;
static ID3D10PixelShader	*g_pBlurPS = NULL;
static ID3D10PixelShader	*g_pBrightnessPS = NULL;
static ID3D10PixelShader	*g_pRGBAPixelShader = NULL;

static sImageInfo g_ImageInfo;

static ID3D10ShaderResourceView *g_pTexture = NULL;

static ID3D10Texture2D *g_pFrameTexture[2] = {NULL, NULL};
static ID3D10ShaderResourceView *g_pFrameSRView[2] = {NULL, NULL};
static ID3D10RenderTargetView *g_pFrameRTView[2] = {NULL, NULL};

static ID3D10RasterizerState *g_pNoCull = NULL;
static ID3D10DepthStencilState *g_pNoZWrite = NULL;

static ID3D10SamplerState *g_pSamplerState = NULL;

static ID3D10BlendState *g_pAddBlend = NULL;
static ID3D10BlendState *g_pNoBlend = NULL;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	g_pTexture = GutLoadTexture_DX10("../../textures/space.tga", &g_ImageInfo);
	if ( NULL==g_pTexture )
		return false;

	// 載入Vertex Shader
	{
		g_pBlurVS = GutLoadVertexShaderDX10_HLSL("../../shaders/Posteffect_blur_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
		g_pBlurPS = GutLoadPixelShaderDX10_HLSL("../../shaders/Posteffect_blur_dx10.hlsl", "PS", "ps_4_0");
		g_pBrightnessPS = GutLoadPixelShaderDX10_HLSL("../../shaders/Posteffect_brightness_dx10.hlsl", "PS", "ps_4_0");
		g_pRGBAPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/texture_dx10.hlsl", "PS", "ps_4_0");

		if ( NULL==g_pBlurVS || NULL==g_pBlurPS ||
			NULL==g_pRGBAPixelShader || NULL==g_pBrightnessPS )
			return false;
	}

	// 設定Vertex資料格式
	{
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(Vertex_VT)*4, g_FullScreenQuad);

	Matrix4x4 ident_mat; ident_mat.Identity();

	g_pVSConstantBuffer = GutCreateShaderConstant_DX10(sizeof(Matrix4x4), &ident_mat);
	g_pBlurConstantBuffer = GutCreateShaderConstant_DX10(sizeof(Vector4)*9);
	g_pBrightnessConstantBuffer = GutCreateShaderConstant_DX10(sizeof(Vector4)*2);

	int w = g_ImageInfo.m_iWidth/4;
	int h = g_ImageInfo.m_iHeight/4;

	if ( !GutCreateRenderTarget_DX10(w, h, 
		DXGI_FORMAT_R8G8B8A8_UNORM, &g_pFrameTexture[0], &g_pFrameSRView[0], &g_pFrameRTView[0]) )
		return false;

	if ( !GutCreateRenderTarget_DX10(w, h,
		DXGI_FORMAT_R8G8B8A8_UNORM, &g_pFrameTexture[1], &g_pFrameSRView[1], &g_pFrameRTView[1]) )
		return false;

	{
		D3D10_RASTERIZER_DESC desc;
		GutSetDX10DefaultRasterizerDesc(desc);
		desc.CullMode = D3D10_CULL_NONE;

		g_pDevice->CreateRasterizerState(&desc, &g_pNoCull);
		g_pDevice->RSSetState(g_pNoCull);
	}

	{
		D3D10_BLEND_DESC desc;
		GutSetDX10DefaultBlendDesc(desc);

		g_pDevice->CreateBlendState(&desc, &g_pNoBlend);

		desc.BlendEnable[0] = TRUE;
		desc.SrcBlend = D3D10_BLEND_ONE;
		desc.DestBlend = D3D10_BLEND_ONE;

		g_pDevice->CreateBlendState(&desc, &g_pAddBlend);
	}

	{
		D3D10_DEPTH_STENCIL_DESC desc;
		GutSetDX10DefaultDepthStencilDesc(desc);

		desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
		desc.DepthEnable = FALSE;

		g_pDevice->CreateDepthStencilState(&desc, &g_pNoZWrite);
		g_pDevice->OMSetDepthStencilState(g_pNoZWrite, 0);
	}

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexBuffer);

	SAFE_RELEASE(g_pBlurVS);
	SAFE_RELEASE(g_pBlurPS);
	SAFE_RELEASE(g_pRGBAPixelShader);
	SAFE_RELEASE(g_pBrightnessPS);

	SAFE_RELEASE(g_pVSConstantBuffer);
	SAFE_RELEASE(g_pBlurConstantBuffer);
	SAFE_RELEASE(g_pBrightnessConstantBuffer);

	SAFE_RELEASE(g_pAddBlend);

	for ( int i=0; i<2; i++ )
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
}

static ID3D10ShaderResourceView *BrightnessImage(ID3D10ShaderResourceView *pTexture, sImageInfo *pInfo)
{
	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	int w = pInfo->m_iWidth/4;
	int h = pInfo->m_iHeight/4;
	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

	D3D10_VIEWPORT vp = {0, 0, w, h, 0.0f, 1.0f};
	device->RSSetViewports(1, &vp);

	device->VSSetShader(g_pBlurVS);
	device->PSSetShader(g_pBrightnessPS);

	ID3D10Buffer *buffer_array[2] = {g_pVSConstantBuffer, g_pBrightnessConstantBuffer};

	device->VSSetConstantBuffers(0, 2, buffer_array);
	device->PSSetConstantBuffers(0, 2, buffer_array);

	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	g_pDevice->IASetInputLayout(g_pVertexLayout);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	device->OMSetRenderTargets(1, &g_pFrameRTView[1], NULL);

	Vector4 *pConstants;
	g_pBrightnessConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstants);
	pConstants[0] = g_vBrightnessOffset;
	pConstants[1] = g_vBrightnessScale;
	g_pBrightnessConstantBuffer->Unmap();

	g_pDevice->PSSetShaderResources(0, 1, &pTexture);
	g_pDevice->Draw(4, 0);

	return g_pFrameSRView[1];
}

static ID3D10ShaderResourceView *BlurImage(ID3D10ShaderResourceView *pTexture, sImageInfo *pInfo)
{
	int w = pInfo->m_iWidth/4;
	int h = pInfo->m_iHeight/4;
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

	Vector4 *pConstants;

	ID3D10Device *device = GutGetGraphicsDeviceDX10();

	device->VSSetShader(g_pBlurVS);
	device->PSSetShader(g_pBlurPS);

	ID3D10Buffer *buffer_array[2] = {g_pVSConstantBuffer, g_pBlurConstantBuffer};

	device->VSSetConstantBuffers(0, 2, buffer_array);
	device->PSSetConstantBuffers(0, 2, buffer_array);

	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	g_pDevice->IASetInputLayout(g_pVertexLayout);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 水平模糊
	{
		device->OMSetRenderTargets(1, &g_pFrameRTView[0], NULL);

		g_pBlurConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstants);
		memcpy(pConstants, vTexOffsetX, sizeof(Vector4)*num_samples);
		g_pBlurConstantBuffer->Unmap();

		g_pDevice->PSSetShaderResources(0, 1, &pTexture);
		g_pDevice->Draw(4, 0);
	}
	// 垂直模糊
	{
		device->OMSetRenderTargets(1, &g_pFrameRTView[1], NULL);

		g_pBlurConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstants);
		memcpy(pConstants, vTexOffsetY, sizeof(Vector4)*num_samples);
		g_pBlurConstantBuffer->Unmap();

		g_pDevice->PSSetShaderResources(0, 1, &g_pFrameSRView[0]);
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

	D3D10_VIEWPORT vp = {0, 0, w, h, 0.0f, 1.0f};
	g_pDevice->RSSetViewports(1, &vp);

	g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, NULL);
	// 設定Shader
	g_pDevice->VSSetShader(g_pBlurVS);
	g_pDevice->PSSetShader(g_pRGBAPixelShader);
	// 設定vertex shader讀取參數的記憶體位罝
	g_pDevice->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer);
	// 設定vertex資料格式
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// 設定三角形頂點索引值資料排列是triangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->PSSetShaderResources(0, 1, &g_pTexture);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDevice->Draw(4, 0);

	if ( g_bPosteffect )
	{
		ID3D10ShaderResourceView *pRSView = NULL;
		pRSView = BrightnessImage(g_pTexture, &g_ImageInfo);
		pRSView = BlurImage(pRSView, &g_ImageInfo);

		g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, NULL);
		g_pDevice->RSSetViewports(1, &vp);

		float dummy[] = {0,0,0,0};
		g_pDevice->OMSetBlendState(g_pAddBlend, dummy, 0xffffffff);
		// 設定Shader
		g_pDevice->VSSetShader(g_pBlurVS);
		g_pDevice->PSSetShader(g_pRGBAPixelShader);
		// 設定vertex shader讀取參數的記憶體位罝
		g_pDevice->VSSetConstantBuffers(0, 1, &g_pVSConstantBuffer);
		// 設定vertex資料格式
		g_pDevice->IASetInputLayout(g_pVertexLayout);
		// 設定三角形頂點索引值資料排列是triangle strip
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		g_pDevice->PSSetShaderResources(0, 1, &pRSView);
		g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
		g_pDevice->Draw(4, 0);

		g_pDevice->OMSetBlendState(g_pNoBlend, dummy, 0xffffffff);
	}

	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain();
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_