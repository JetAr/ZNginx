#ifdef _ENABLE_DX10_

#include "render_data.h"
#include "Gut.h"
#include "GutDX10.h"
#include "GutModel_DX10.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10Buffer			*g_pVertexBuffer	= NULL;

static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;

static ID3D10DepthStencilState *g_pZStencil_Incr = NULL;
static ID3D10DepthStencilState *g_pZStencil_Test = NULL;
static ID3D10DepthStencilState *g_pZStencil_Less = NULL;

static ID3D10RasterizerState *g_pRasterizerCull = NULL;
static ID3D10RasterizerState *g_pRasterizerNoCull = NULL;
static ID3D10SamplerState	*g_pSamplerState = NULL;

static Matrix4x4 g_proj_matrix;

static CGutModel_DX10 g_Model_DX10;

struct ShaderConstant
{
	Matrix4x4 m_wvp_matrix;
	Vector4   m_color;
};

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	{
		// 載入Vertex Shader
		g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/color_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pVertexShader )
			return false;
		// 載入Pixel Shader
		g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/color_dx10.hlsl", "PS", "ps_4_0");
		if ( NULL==g_pPixelShader )
			return false;
	}

	CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl");
	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX10.ConvertToDX10Model(&g_Model);

	// 設定Vertex資料格式
	{
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	// 配置Shader讀取參數的記憶體空間
	{
		D3D10_BUFFER_DESC cbDesc;
		ZeroMemory(&cbDesc, sizeof(cbDesc));

		cbDesc.ByteWidth = sizeof(ShaderConstant);
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
			return false;
	}

	// 配置Vertex Buffer
	{
		D3D10_BUFFER_DESC cbDesc;
		ZeroMemory(&cbDesc, sizeof(cbDesc));

		cbDesc.ByteWidth = sizeof(Vertex_V) * 4;
		cbDesc.Usage = D3D10_USAGE_DYNAMIC ;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA subDesc;
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_Quad;

		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBuffer ) )
			return false;
	}

	// rasterizer state物件
	{
		D3D10_RASTERIZER_DESC desc;

		GutSetDX10DefaultRasterizerDesc(desc);

		desc.CullMode = D3D10_CULL_NONE;
		desc.FrontCounterClockwise = true;

		if ( D3D_OK != g_pDevice->CreateRasterizerState(&desc, &g_pRasterizerNoCull) )
			return false;

		desc.CullMode = D3D10_CULL_BACK;
		desc.FrontCounterClockwise = true;

		if ( D3D_OK != g_pDevice->CreateRasterizerState(&desc, &g_pRasterizerCull) )
			return false;

		CGutModel_DX10::SetRasterizeState(g_pRasterizerCull, g_pRasterizerNoCull);

		g_pDevice->RSSetState(g_pRasterizerNoCull);
	}

	// depth stencil object
	{
		D3D10_DEPTH_STENCIL_DESC desc;
		GutSetDX10DefaultDepthStencilDesc(desc);

		desc.StencilEnable = TRUE;
		desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_INCR;
		desc.BackFace = desc.FrontFace;

		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencil_Incr);

		desc.DepthFunc = D3D10_COMPARISON_ALWAYS;
		desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
		desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;
		desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_KEEP;
		desc.BackFace = desc.FrontFace;

		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencil_Test);

		desc.FrontFace.StencilFunc = D3D10_COMPARISON_LESS;
		desc.BackFace = desc.FrontFace;

		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencil_Less);
	}

	// sampler state / texture filter
	{
		D3D10_SAMPLER_DESC sampler_desc;
		ZeroMemory(&sampler_desc, sizeof(sampler_desc));

		sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;

		g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState);

		for ( int i=0; i<4; i++ )
		{
			g_pDevice->PSSetSamplers(i, 1, &g_pSamplerState);
		}
	}

	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pConstantBuffer);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pRasterizerCull);
	SAFE_RELEASE(g_pRasterizerNoCull);
	SAFE_RELEASE(g_pSamplerState);

	SAFE_RELEASE(g_pZStencil_Test);
	SAFE_RELEASE(g_pZStencil_Incr);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();

	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// frame buffer
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); 
	// depth/stencil buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	// front/back buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 

	Matrix4x4 ident_matrix; ident_matrix.Identity();
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_proj_matrix = world_matrix * view_matrix * g_proj_matrix;

	// 清除顏色
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// 清除Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	// 畫出茶壼, 并更新 Stencil Buffer.
	{
		g_pDevice->OMSetDepthStencilState(g_pZStencil_Incr, 0);

		CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
		CGutModel_DX10::SetViewMatrix(view_matrix);
		CGutModel_DX10::SetWorldMatrix(world_matrix);
		CGutModel_DX10::UpdateMatrix();

		g_Model_DX10.Render();
	}

	// 套用 Shader
	{
		UINT stride = sizeof(Vertex_V);
		UINT offset = 0;
		// 設定Shader
		g_pDevice->VSSetShader(g_pVertexShader);
		g_pDevice->PSSetShader(g_pPixelShader);
		// 設定Shader讀取參數的記憶體位罝
		g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
		g_pDevice->PSSetConstantBuffers(0, 1, &g_pConstantBuffer);
		// 設定vertex資料格式
		g_pDevice->IASetInputLayout(g_pVertexLayout);
		// 設定Vertex Buffer
		g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
		// 設定三角形頂點索引值資料排列是triangle strip
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}

	ShaderConstant *pConstData = NULL;

	// 用綠色標示出只更新1次的像素
	{
		// 設定shader參數
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData->m_wvp_matrix = ident_matrix;
		pConstData->m_color.Set(0,1,0,1);
		g_pConstantBuffer->Unmap();
		// 畫出看板
		g_pDevice->OMSetDepthStencilState(g_pZStencil_Test, 1);
		g_pDevice->Draw(4, 0);
	}
	// 用藍色標示出更新2次的像素
	{
		// 設定shader參數
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData->m_wvp_matrix = ident_matrix;
		pConstData->m_color.Set(0,0,1,1);
		g_pConstantBuffer->Unmap();
		// 畫出看板
		g_pDevice->OMSetDepthStencilState(g_pZStencil_Test, 2);
		g_pDevice->Draw(4, 0);
	}
	// 用紅色標示出更新3次的像素
	{
		// 設定shader參數
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData->m_wvp_matrix = ident_matrix;
		pConstData->m_color.Set(1,0,0,1);
		g_pConstantBuffer->Unmap();
		// 畫出看板
		g_pDevice->OMSetDepthStencilState(g_pZStencil_Test, 3);
		g_pDevice->Draw(4, 0);
	}
	// 用白色標示出更新超過3次的像素
	{
		// 設定shader參數
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData->m_wvp_matrix = ident_matrix;
		pConstData->m_color.Set(1,1,1,1);
		g_pConstantBuffer->Unmap();
		// 畫出看板
		g_pDevice->OMSetDepthStencilState(g_pZStencil_Less, 3);
		g_pDevice->Draw(4, 0);
	}

	// 等待硬體掃結束, 然後才更新畫面.
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_