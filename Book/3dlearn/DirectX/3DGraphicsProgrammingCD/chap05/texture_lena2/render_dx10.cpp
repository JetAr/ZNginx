#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "render_data.h"

#include "Gut.h"
#include "GutTGA.h"
#include "GutTexture_DX10.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;
static ID3D10Texture2D *g_pTexture2D = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_view_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Vertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/texture_dx10.shader", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// 載入Pixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/texture_dx10.shader", "PS", "ps_4_0");
	if ( NULL==g_pPixelShader )
		return false;
	// 載入貼圖
	TGAImg tga_loader;
	if( IMG_OK!=tga_loader.Load("../../textures/lena_rgba.tga") )
		return false;

	// 設定貼圖物件模式跟大小
	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.Width = tga_loader.GetWidth();
	desc.Height = tga_loader.GetHeight();
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D10_USAGE_DYNAMIC;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;

	// 開啟貼圖物件
	g_pDevice->CreateTexture2D( &desc, NULL, &g_pTexture2D );

	// 取出貼圖物件的記憶體指標
	D3D10_MAPPED_TEXTURE2D mappedtex;
	g_pTexture2D->Map( D3D10CalcSubresource(0, 0, 1), D3D10_MAP_WRITE_DISCARD, 0, &mappedtex );

	unsigned char *target = (unsigned char *)mappedtex.pData;
	unsigned char *source = tga_loader.GetImg();

	// 填入圖片資料
	for ( int y=0; y<tga_loader.GetHeight(); y++ )
	{
		for ( int x=0; x<tga_loader.GetWidth(); x++ )
		{
			// DXGI_FORMAT_R8G8B8A8_UNORM的記憶體排列順序是RGBA
			// 直接拷備就可以了, 不用再轉換.

			target[0] = source[0];
			target[1] = source[1];
			target[2] = source[2];
			target[3] = source[3];

			target+=4;
			source+=4;
		}
	}

	g_pTexture2D->Unmap( D3D10CalcSubresource(0, 0, 1) );

	// 還要經由貼圖物件來產生ID3D10ShaderResourceView才能給Shader使用
	D3D10_SHADER_RESOURCE_VIEW_DESC view_desc;
	ZeroMemory(&view_desc, sizeof(view_desc));

	view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	view_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	view_desc.Texture2D.MostDetailedMip = 0;
	view_desc.Texture2D.MipLevels = 1;

	g_pDevice->CreateShaderResourceView(g_pTexture2D, &view_desc, &g_pTexture);

	// 設定Vertex資料格式
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	D3D10_BUFFER_DESC cbDesc;
	// vertex buffer
	cbDesc.ByteWidth = sizeof(Vertex_VT) * 4;
	cbDesc.Usage = D3D10_USAGE_DYNAMIC ;
	cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
	cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;

	D3D10_SUBRESOURCE_DATA subDesc;
	ZeroMemory(&subDesc, sizeof(subDesc));
	subDesc.pSysMem = g_Quad;
	// 配置一塊可以存放Vertex的記憶體, 也就是Vertex Buffer.
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBuffer ) )
		return false;

	// 配置Shader參數的記憶體空間
	cbDesc.ByteWidth = sizeof(Matrix4x4);
	cbDesc.Usage = D3D10_USAGE_DYNAMIC;
	cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags = 0;
	if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
		return false;

	// 計算投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);

	// rasterizer state物件
	D3D10_RASTERIZER_DESC rasterizer_state_desc;

	rasterizer_state_desc.FillMode = D3D10_FILL_SOLID;
	rasterizer_state_desc.CullMode = D3D10_CULL_NONE;
	rasterizer_state_desc.FrontCounterClockwise = true;
	rasterizer_state_desc.DepthBias = 0;
	rasterizer_state_desc.DepthBiasClamp = 0.0f;
	rasterizer_state_desc.SlopeScaledDepthBias = 0.0f;
	rasterizer_state_desc.DepthClipEnable = false;
	rasterizer_state_desc.ScissorEnable = false;
	rasterizer_state_desc.MultisampleEnable = false;
	rasterizer_state_desc.AntialiasedLineEnable = false;

	if ( D3D_OK != g_pDevice->CreateRasterizerState(&rasterizer_state_desc, &g_pRasterizerState) )
		return false;

	g_pDevice->RSSetState(g_pRasterizerState);

	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pConstantBuffer);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRasterizerState);
	SAFE_RELEASE(g_pTexture);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;
	// 取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	// 清除顏色
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// 清除Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// 設定vertex shader
	g_pDevice->VSSetShader(g_pVertexShader);
	// 設定pixel shader
	g_pDevice->PSSetShader(g_pPixelShader);
	// 設定vertex shader讀取參數的記憶體位罝
	g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// 套用貼圖
	g_pDevice->PSSetShaderResources(0, 1, &g_pTexture);
	// 設定vertex資料格式
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// 設定vertex buffer
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	// 設定三角形頂點索引值資料排列是triangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 計算矩陣
	Matrix4x4 view_proj_matrix = g_view_matrix * g_proj_matrix;
	Matrix4x4 world_view_proj_matrix = g_world_matrix * view_proj_matrix;
	// 更新shader參數
	Matrix4x4 *pConstData;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 畫出格子
	g_pDevice->Draw(4, 0);
	// 等待硬體掃結束, 然後才更新畫面
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_