#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutDX10.h"
#include "GutTexture_DX10.h"

#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer		= NULL;
static ID3D10Buffer			*g_pConstantBuffer	= NULL;

static ID3D10VertexShader	*g_pVertexShader = NULL;
static ID3D10PixelShader	*g_pGrayscalePS	= NULL;
static ID3D10PixelShader	*g_pRGBAPixelShader = NULL;
static ID3D10PixelShader	*g_pColorTransformPS = NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;

static ID3D10RasterizerState *g_pRasterizerState= NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Vertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/Posteffect_grayscale.hlsl", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// 載入Pixel Shader
	g_pGrayscalePS = GutLoadPixelShaderDX10_HLSL("../../shaders/Posteffect_grayscale.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pGrayscalePS )
		return false;
	g_pRGBAPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/texture_dx10.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pRGBAPixelShader )
		return false;
	g_pColorTransformPS = GutLoadPixelShaderDX10_HLSL("../../shaders/Posteffect_ColorTransform_dx10.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pColorTransformPS )
		return false;

	g_pTexture = GutLoadTexture_DX10("../../textures/lena.tga");
	if ( NULL==g_pTexture )
		return false;

	// 設定Vertex資料格式
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(Vertex_VT)*4, g_FullScreenQuad);

	Matrix4x4 ident_mat[2];
	ident_mat[0].Identity();
	ident_mat[1].Identity();

	g_pConstantBuffer = GutCreateShaderConstant_DX10(sizeof(Matrix4x4)*2, ident_mat);

	// Rasterizer state物件
	{
		D3D10_RASTERIZER_DESC desc;
		GutSetDX10DefaultRasterizerDesc(desc);
		desc.CullMode = D3D10_CULL_NONE;

		if ( D3D_OK != g_pDevice->CreateRasterizerState(&desc, &g_pRasterizerState) )
			return false;

		g_pDevice->RSSetState(g_pRasterizerState);
	}

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pConstantBuffer);

	SAFE_RELEASE(g_pVertexShader);

	SAFE_RELEASE(g_pGrayscalePS);
	SAFE_RELEASE(g_pColorTransformPS);
	SAFE_RELEASE(g_pRGBAPixelShader);

	SAFE_RELEASE(g_pTexture);

	SAFE_RELEASE(g_pRasterizerState);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	// 取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer

	// 清除顏色
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// 清除Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// 設定Shader
	g_pDevice->VSSetShader(g_pVertexShader);

	Matrix4x4 *pMatrices = NULL;
	g_pConstantBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pMatrices );
	pMatrices[0].Identity();

	switch(g_iPosteffect)
	{
	default:
		g_pDevice->PSSetShader(g_pRGBAPixelShader);
		break;
	case 2:
		g_pDevice->PSSetShader(g_pGrayscalePS);
		break;
	case 3:
		pMatrices[1] = g_SepiaMatrix;
		g_pDevice->PSSetShader(g_pColorTransformPS);
		break;
	case 4:
		pMatrices[1] = g_SaturateMatrix;
		g_pDevice->PSSetShader(g_pColorTransformPS);
		break;
	}

	g_pConstantBuffer->Unmap();

	// 設定vertex shader讀取參數的記憶體位罝
	g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	g_pDevice->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// 設定vertex資料格式
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// 設定三角形頂點索引值資料排列是triangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//
	g_pDevice->PSSetShaderResources(0, 1, &g_pTexture);
	// 
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDevice->Draw(4, 0);
	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_