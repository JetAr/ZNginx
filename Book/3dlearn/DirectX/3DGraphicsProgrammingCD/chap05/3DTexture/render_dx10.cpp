#ifdef _ENABLE_DX10_

#include "render_data.h"

#include "Gut.h"
#include "GutTexture_DX10.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;
static ID3D10SamplerState	 *g_pSamplerState	= NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_view_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	{
		const char *shader = "../../shaders/texture3D_dx10.hlsl";
		// 載入Vertex Shader
		g_pVertexShader = GutLoadVertexShaderDX10_HLSL(shader, "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pVertexShader )
			return false;
		// 載入Pixel Shader
		g_pPixelShader = GutLoadPixelShaderDX10_HLSL(shader, "PS", "ps_4_0");
		if ( NULL==g_pPixelShader )
			return false;
	}

	// 載入貼圖
	g_pTexture = GutLoadVolumeTexture_DX10("../../textures/CT_256x256x256.dds");
	if ( NULL==g_pTexture )
		return false;

	// 設定Vertex資料格式
	D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
	};
	if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
		return false;

	SAFE_RELEASE(pVSCode);

	D3D10_BUFFER_DESC cbDesc;
	D3D10_SUBRESOURCE_DATA subDesc;

	{
		// vertex buffer
		cbDesc.ByteWidth = sizeof(Vertex_VT) * 4;
		cbDesc.Usage = D3D10_USAGE_DYNAMIC ;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_Quad;
		// 配置一塊可以存放Vertex的記憶體, 也就是Vertex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBuffer ) )
			return false;
	}

	{
		// 配置Shader參數的記憶體空間
		cbDesc.ByteWidth = sizeof(Matrix4x4) * 2;
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
			return false;
	}

	// 計算投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);

	{
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
	}

	{
		D3D10_SAMPLER_DESC sampler_desc;
		ZeroMemory(&sampler_desc, sizeof(sampler_desc));

		sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;

		if ( D3D_OK != g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState) )
			return false;

		g_pDevice->PSSetSamplers(0, 1, &g_pSamplerState);
	}

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
	// `取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件`
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView();
	// front/back buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 
	// `清除顏色`
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// `清除Depth/Stencil buffer`
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// `設定vertex shader`
	g_pDevice->VSSetShader(g_pVertexShader);
	// `設定pixel shader`
	g_pDevice->PSSetShader(g_pPixelShader);
	// `設定vertex shader讀取參數的記憶體位罝`
	g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
	// `套用貼圖`
	g_pDevice->PSSetShaderResources(0, 1, &g_pTexture);
	// `設定vertex資料格式`
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// `設定vertex buffer`
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	// `設定三角形頂點索引值資料排列是triangle strip`
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// `計算矩陣`
	Matrix4x4 view_proj_matrix = g_view_matrix * g_proj_matrix;
	Matrix4x4 world_view_proj_matrix = g_world_matrix * view_proj_matrix;
	// `更新shader參數`
	Matrix4x4 *pConstData;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	pConstData[0] = world_view_proj_matrix;
	pConstData[1] = g_texture_matrix;
	g_pConstantBuffer->Unmap();
	// `畫出矩形`
	g_pDevice->Draw(4, 0);
	// `等待硬體掃結束, 然後才更新畫面.`
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_