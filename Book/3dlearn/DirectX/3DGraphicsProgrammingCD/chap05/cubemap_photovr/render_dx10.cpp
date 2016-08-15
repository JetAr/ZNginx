#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "render_data.h"

#include "Gut.h"
#include "GutTexture_DX10.h"


static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;
static ID3D10Buffer			*g_pSphereIndexBuffer= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// shader loading
	{
		const char *shaders = "../../shaders/cubemap_photovr_dx10.hlsl";
		// 載入Vertex Shader
		g_pVertexShader = GutLoadVertexShaderDX10_HLSL(shaders, "VS", "vs_4_0", &pVSCode);
		if ( NULL==g_pVertexShader )
			return false;
		// 載入Pixel Shader
		g_pPixelShader = GutLoadPixelShaderDX10_HLSL(shaders, "PS", "ps_4_0");
		if ( NULL==g_pPixelShader )
			return false;
	}

	// 載入貼圖
	{
		const char *texture_array[] = 
		{
			"../../textures/uffizi_right.tga",
			"../../textures/uffizi_left.tga",
			"../../textures/uffizi_top.tga",
			"../../textures/uffizi_bottom.tga",
			"../../textures/uffizi_back.tga", // `右手座標系上 Z+ 為鏡頭後方.`
			"../../textures/uffizi_front.tga" // `右手座標系上 Z- 為鏡頭前方.`
		};

		g_pTexture = GutLoadCubemapTexture_DX10(texture_array);
	}

	{
		// 設定Vertex資料格式
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	D3D10_BUFFER_DESC cbDesc;
	D3D10_SUBRESOURCE_DATA subDesc;

	{
		// vertex buffer
		ZeroMemory(&cbDesc, sizeof(cbDesc));
		cbDesc.ByteWidth = sizeof(Vertex_VN) * g_iNumSphereVertices;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		// 開啟Vertex Buffer時同時把資料拷貝過去
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_pSphereVertices;
		// 配置Vertex Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pVertexBuffer ) )
			return false;
	}

	{
		// 設定一塊可以用來放Index的記憶體.
		ZeroMemory(&cbDesc, sizeof(cbDesc));
		cbDesc.ByteWidth = sizeof(unsigned short) * g_iNumSphereIndices;
		cbDesc.Usage = D3D10_USAGE_IMMUTABLE;
		cbDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		// 開啟Index Buffer時同時把資料拷貝過去
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = g_pSphereIndices;
		// 配置Index Buffer.
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, &subDesc, &g_pSphereIndexBuffer ) )
			return false;
	}

	{
		ZeroMemory(&cbDesc, sizeof(cbDesc));
		// 配置Shader參數的記憶體空間
		cbDesc.ByteWidth = sizeof(Matrix4x4);
		cbDesc.Usage = D3D10_USAGE_DYNAMIC;
		cbDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		// 配置Shader Constant
		if ( D3D_OK != g_pDevice->CreateBuffer( &cbDesc, NULL, &g_pConstantBuffer ) )
			return false;
	}

	// rasterizer state物件
	{
		D3D10_RASTERIZER_DESC rs;
		ZeroMemory(&rs, sizeof(rs));

		rs.FillMode = D3D10_FILL_SOLID;
		rs.CullMode = D3D10_CULL_NONE;
		rs.FrontCounterClockwise = true;
		rs.DepthBias = 0;
		rs.DepthBiasClamp = 0.0f;
		rs.SlopeScaledDepthBias = 0.0f;
		rs.DepthClipEnable = false;
		rs.ScissorEnable = false;
		rs.MultisampleEnable = false;
		rs.AntialiasedLineEnable = false;

		if ( D3D_OK != g_pDevice->CreateRasterizerState(&rs, &g_pRasterizerState) )
			return false;

		g_pDevice->RSSetState(g_pRasterizerState);
	}

	// 計算投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexBuffer);
	SAFE_RELEASE(g_pSphereIndexBuffer);
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
	UINT stride = sizeof(Vertex_VN);
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
	// 設定index buffer
	g_pDevice->IASetIndexBuffer(g_pSphereIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// 設定三角形頂點索引值資料排列是triangle strip
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 計算矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_proj_matrix = object_matrix * view_matrix * g_proj_matrix;
	// 更新shader參數
	Matrix4x4 *pConstData;
	g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = world_view_proj_matrix;
	g_pConstantBuffer->Unmap();
	// 畫出格子
	//g_pDevice->Draw(4, 0);
	g_pDevice->DrawIndexed(g_iNumSphereIndices, 0, 0);
	// 等待硬體掃結束, 然後才更新畫面
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_