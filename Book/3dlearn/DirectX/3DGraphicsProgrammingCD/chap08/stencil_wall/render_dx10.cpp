#ifdef _ENABLE_DX10_

#include <d3d10.h>

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

static ID3D10DepthStencilState *g_pStencilState_Replace = NULL;
static ID3D10DepthStencilState *g_pStencilState_Test = NULL;

static ID3D10BlendState *g_pBlendState = NULL;
static ID3D10SamplerState *g_pSamplerState = NULL;

static ID3D10ShaderResourceView *g_pTexture0 = NULL;
static ID3D10ShaderResourceView *g_pTexture1 = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_view_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Vertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/texture_alphatest_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// 載入Pixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/texture_alphatest_dx10.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pPixelShader )
		return false;

	g_pTexture0 = GutLoadTexture_DX10("../../textures/brickwall_broken.tga");
	g_pTexture1 = GutLoadTexture_DX10("../../textures/spotlight_effect.tga");
	if ( g_pTexture0==NULL || g_pTexture1==NULL )
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

	D3D10_BUFFER_DESC cbDesc;

	// vertex buffer
	{
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
	}

	// 配置Shader參數的記憶體空間
	{
		cbDesc.ByteWidth = sizeof(Matrix4x4);
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		cbDesc.MiscFlags = 0;
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
			return false;
	}

	// rasterizer state物件
	{
		D3D10_RASTERIZER_DESC desc;
		GutSetDX10DefaultRasterizerDesc(desc);

		desc.CullMode = D3D10_CULL_NONE;
		desc.FrontCounterClockwise = true;

		if ( D3D_OK != g_pDevice->CreateRasterizerState(&desc, &g_pRasterizerState) )
			return false;

		g_pDevice->RSSetState(g_pRasterizerState);
	}

	// depth stencil state
	{
		D3D10_DEPTH_STENCIL_DESC desc;
		GutSetDX10DefaultDepthStencilDesc(desc);

		desc.DepthEnable = FALSE;
		desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D10_COMPARISON_ALWAYS;

		desc.StencilEnable = TRUE;
		desc.StencilReadMask = 0xff;
		desc.StencilWriteMask = 0xff;

		desc.FrontFace.StencilFailOp = D3D10_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
		desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;

		desc.BackFace = desc.FrontFace;
		// 這個設定用來設定像素的stencil值
		g_pDevice->CreateDepthStencilState(&desc, &g_pStencilState_Replace);

		desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;
		desc.BackFace = desc.FrontFace;
		// 這個設定用來做stencil test測試
		g_pDevice->CreateDepthStencilState(&desc, &g_pStencilState_Test);
	}

	// blend state
	{
		D3D10_BLEND_DESC desc;
		GutSetDX10DefaultBlendDesc(desc);

		desc.BlendEnable[0] = TRUE;

		desc.SrcBlend = D3D10_BLEND_ONE;
		desc.DestBlend = D3D10_BLEND_ONE;
		desc.BlendOp = D3D10_BLEND_OP_ADD;

		desc.SrcBlendAlpha = D3D10_BLEND_ONE;
		desc.DestBlendAlpha = D3D10_BLEND_ONE;
		desc.BlendOpAlpha = D3D10_BLEND_OP_ADD;

		g_pDevice->CreateBlendState(&desc, &g_pBlendState);
	}

	// sampler state / texture filter
	{
		D3D10_SAMPLER_DESC desc;
		GutSetDX10DefaultSamplerDesc(desc);

		desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;

		g_pDevice->CreateSamplerState(&desc, &g_pSamplerState);

		for ( int i=0; i<4; i++ )
		{
			g_pDevice->PSSetSamplers(i, 1, &g_pSamplerState);
		}
	}

	// 計算投影矩陣
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	g_proj_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);

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
	SAFE_RELEASE(g_pStencilState_Replace);
	SAFE_RELEASE(g_pStencilState_Test);

	SAFE_RELEASE(g_pBlendState);
	SAFE_RELEASE(g_pSamplerState);

	SAFE_RELEASE(g_pTexture0);
	SAFE_RELEASE(g_pTexture1);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;

	g_fOrthoWidth = g_fOrthoSize;
	g_fOrthoHeight = g_fOrthoSize;

	if ( aspect > 1.0f )
		g_fOrthoHeight *= aspect;
	else
		g_fOrthoWidth /= aspect;

	g_proj_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	float fDummy[4] = {0,0,0,0};
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;
	//frame buffer
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); 
	//depth/stencil buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView();
	// front/back buffer chain
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 
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
	// 設定vertex資料格式
	g_pDevice->IASetInputLayout(g_pVertexLayout);
	// 設定vertex buffer
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	// 設定三角形頂點索引值資料排列是triangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// 畫墻壁
	{
		Matrix4x4 IdentityMatrix; 
		IdentityMatrix.Identity();
		// 更新shader參數
		Matrix4x4 *pConstData;
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		*pConstData = IdentityMatrix;
		g_pConstantBuffer->Unmap();
		// 把混色關閉
		g_pDevice->OMSetBlendState(NULL, fDummy, 0xff);
		// 把墻壁所填充的像素 Stencil 值設定 1, 最後一個參數是 Stencil 參考值.
		g_pDevice->OMSetDepthStencilState(g_pStencilState_Replace, 1);
		// 套用貼圖
		g_pDevice->PSSetShaderResources(0, 1, &g_pTexture0);
		// 畫出墻壁
		g_pDevice->Draw(4, 0);
	}
	// 畫手電筒光源
	{
		// 計算矩陣
		//Matrix4x4 view_proj_matrix = g_view_matrix * g_proj_matrix;
		Matrix4x4 world_view_proj_matrix = g_world_matrix * g_view_matrix * g_proj_matrix;
		// 更新shader參數
		Matrix4x4 *pConstData;
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		*pConstData = world_view_proj_matrix;
		g_pConstantBuffer->Unmap();
		// 使用加色混色
		g_pDevice->OMSetBlendState(g_pBlendState, fDummy, 0xff);
		// 把墻壁所填充的像素Stencil值設定1
		if ( g_bStencil )
			g_pDevice->OMSetDepthStencilState(g_pStencilState_Test, 1);
		else
			g_pDevice->OMSetDepthStencilState(NULL, 1);

		// 套用貼圖
		g_pDevice->PSSetShaderResources(0, 1, &g_pTexture1);
		// 畫出光點
		g_pDevice->Draw(4, 0);
	}
	// 等待硬體掃結束, 然後才更新畫面.
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_