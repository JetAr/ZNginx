#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "Gut.h"
#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;
static ID3D10Texture2D *g_pTexture2D = NULL;
static ID3D10SamplerState	*g_pSamplerState[4];

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Vertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/texture_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// 載入Pixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/texture_dx10.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pPixelShader )
		return false;

	if ( D3D_OK!=D3DX10CreateShaderResourceViewFromFile(g_pDevice, "../../textures/lena.bmp", NULL, NULL, &g_pTexture, NULL) )
		return false;

	// 設定Vertex資料格式
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};

	if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	int mipmap_levels = 10;

	D3D10_TEXTURE2D_DESC desc;
	ZeroMemory( &desc, sizeof(desc) );

	desc.Width = g_iWidth;
	desc.Height = g_iHeight;
	desc.MipLevels = mipmap_levels;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Usage = D3D10_USAGE_IMMUTABLE;
	desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

	// 透過設定初值的方法來拷備貼圖資料
	D3D10_SUBRESOURCE_DATA init_data[10];

	int w = g_iWidth;

	for ( int i=0; i<mipmap_levels; i++ )
	{
		// 放入每個圖層的資料
		init_data[i].pSysMem = g_pImages[i];
		init_data[i].SysMemPitch = w * 4;
		init_data[i].SysMemSlicePitch = 0;
		w/=2;
	}

	ID3D10Texture2D *g_pTexture2D = NULL;
	g_pDevice->CreateTexture2D( &desc, init_data, &g_pTexture2D );

	// 還要經由貼圖物件來產生ID3D10ShaderResourceView才能給Shader使用
	D3D10_SHADER_RESOURCE_VIEW_DESC view_desc;
	ZeroMemory(&view_desc, sizeof(view_desc));

	view_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	view_desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	view_desc.Texture2D.MostDetailedMip = 0;
	view_desc.Texture2D.MipLevels = mipmap_levels;

	g_pDevice->CreateShaderResourceView(g_pTexture2D, &view_desc, &g_pTexture);

	D3D10_FILTER filters[4] = 
	{
		D3D10_FILTER_MIN_MAG_MIP_POINT,
		D3D10_FILTER_MIN_MAG_LINEAR_MIP_POINT,
		D3D10_FILTER_MIN_MAG_MIP_LINEAR,
		D3D10_FILTER_ANISOTROPIC
	};

	D3D10_SAMPLER_DESC sampler_desc;
	ZeroMemory(&sampler_desc, sizeof(sampler_desc));
	// 座標解讀模式為wrap
	sampler_desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
	sampler_desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;
	// anisotropic filter
	sampler_desc.MaxAnisotropy = 8;

	for ( int i=0; i<4; i++ )
	{
		sampler_desc.Filter = filters[i];

		if ( i==0 )
		{
			// 只使用mipmap第0層, 等於是關閉mipmap
			sampler_desc.MaxLOD = 0;
		}
		else
		{
			// 設定成一個超大值, 代表會使用所有mipmap圖層
			sampler_desc.MaxLOD = FLT_MAX;
		}

		g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState[i]);
	}

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

	for ( int i=0; i<4; i++ )
	{
		SAFE_RELEASE(g_pSamplerState[i]);
	}

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
	// 設定貼圖座標解讀格式
	g_pDevice->PSSetSamplers(0, 1, &g_pSamplerState[g_iFilterMode-1]);
	// 設定vertex資料格式
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// 設定vertex buffer
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	// 設定三角形頂點索引值資料排列是triangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 計算矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 view_proj_matrix = view_matrix * g_proj_matrix;
	Matrix4x4 world_view_proj_matrix = object_matrix * view_proj_matrix;
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