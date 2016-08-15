#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutDX10.h"
#include "GutTexture_DX10.h"

#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer		= NULL;
static ID3D10Buffer			*g_pVSConstantBuffer	= NULL;
static ID3D10Buffer			*g_pPSConstantBuffer	= NULL;

static ID3D10VertexShader	*g_pBlurVS = NULL;
static ID3D10PixelShader	*g_pBlurPS = NULL;
static ID3D10PixelShader	*g_pRGBAPixelShader = NULL;

static sImageInfo g_ImageInfo;

static ID3D10ShaderResourceView *g_pTexture = NULL;

static ID3D10Texture2D *g_pFrameTexture[2] = {NULL, NULL};
static ID3D10ShaderResourceView *g_pFrameSRView[2] = {NULL, NULL};
static ID3D10RenderTargetView *g_pFrameRTView[2] = {NULL, NULL};
static ID3D10RasterizerState *g_pNoCull = NULL;
static ID3D10SamplerState *g_pSamplerState = NULL;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	g_pTexture = GutLoadTexture_DX10("../../textures/lena.tga", &g_ImageInfo);
	if ( NULL==g_pTexture )
		return false;

	// 載入Vertex Shader
	{
		g_pBlurVS = GutLoadVertexShaderDX10_HLSL("../../shaders/Posteffect_blur_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pBlurVS )
			return false;

		g_pBlurPS = GutLoadPixelShaderDX10_HLSL("../../shaders/Posteffect_blur_dx10.hlsl", "PS", "ps_4_0");
		if ( NULL==g_pBlurPS )
			return false;

		g_pRGBAPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/texture_dx10.hlsl", "PS", "ps_4_0");
		if ( NULL==g_pRGBAPixelShader )
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
	g_pPSConstantBuffer = GutCreateShaderConstant_DX10(sizeof(Vector4)*9);

	if ( !GutCreateRenderTarget_DX10(g_ImageInfo.m_iWidth, g_ImageInfo.m_iHeight, 
		DXGI_FORMAT_R8G8B8A8_UNORM, &g_pFrameTexture[0], &g_pFrameSRView[0], &g_pFrameRTView[0]) )
		return false;

	if ( !GutCreateRenderTarget_DX10(g_ImageInfo.m_iWidth, g_ImageInfo.m_iHeight, 
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
		D3D10_SAMPLER_DESC desc;
		GutSetDX10DefaultSamplerDesc(desc);

		desc.AddressU = D3D10_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D10_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D10_TEXTURE_ADDRESS_CLAMP;

		g_pDevice->CreateSamplerState(&desc, &g_pSamplerState);
		g_pDevice->PSSetSamplers(0, 1, &g_pSamplerState);
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

	SAFE_RELEASE(g_pPSConstantBuffer);
	SAFE_RELEASE(g_pVSConstantBuffer);

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

static ID3D10ShaderResourceView *BlurImage(ID3D10ShaderResourceView *pTexture, sImageInfo *pInfo)
{
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;
	float fTexelW = 1.0f/(float)pInfo->m_iWidth;
	float fTexelH = 1.0f/(float)pInfo->m_iHeight;

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

	ID3D10Buffer *buffer_array[2] = {g_pVSConstantBuffer, g_pPSConstantBuffer};

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

		g_pPSConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstants);
		memcpy(pConstants, vTexOffsetX, sizeof(Vector4)*num_samples);
		g_pPSConstantBuffer->Unmap();

		g_pDevice->PSSetShaderResources(0, 1, &pTexture);
		g_pDevice->Draw(4, 0);
	}
	// 垂直模糊
	{
		device->OMSetRenderTargets(1, &g_pFrameRTView[1], NULL);

		g_pPSConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstants);
		memcpy(pConstants, vTexOffsetY, sizeof(Vector4)*num_samples);
		g_pPSConstantBuffer->Unmap();

		g_pDevice->PSSetShaderResources(0, 1, &g_pFrameSRView[0]);
		g_pDevice->Draw(4, 0);
	}

	return g_pFrameSRView[1];
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	ID3D10ShaderResourceView *pRSView = g_pTexture;

	if ( g_bPosteffect )
	{
		pRSView = BlurImage(g_pTexture, &g_ImageInfo);
	}

	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer

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
	g_pDevice->PSSetShaderResources(0, 1, &pRSView);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDevice->Draw(4, 0);

	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_