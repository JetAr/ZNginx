#ifdef _ENABLE_DX10_

#include <d3d10.h>

#include "render_data.h"
#include "Gut.h"
#include "GutModel_DX10.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;
static ID3D10Buffer			*g_pConstantBuffer	= NULL;

static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;

static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10DepthStencilState *g_pZStencilState_Mark = NULL;
static ID3D10DepthStencilState *g_pZStencilState_ZClear = NULL;
static ID3D10DepthStencilState *g_pZStencilState_Mirror = NULL;
static ID3D10DepthStencilState *g_pZStencilState_Regular = NULL;

static ID3D10SamplerState *g_pSamplerState = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_view_matrix;

static CGutModel_DX10 g_Model_DX10;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入畫鏡面用的Vertex Shader
	g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/vertex_color_dx10.hlsl", "VS", "vs_4_0", &pVSCode);
	if ( NULL==g_pVertexShader )
		return false;
	// 載入畫鏡面用的Pixel Shader
	g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/vertex_color_dx10.hlsl", "PS", "ps_4_0");
	if ( NULL==g_pPixelShader )
		return false;

	// 載入畫模型專用, 模擬fixed pipeline的shader
	if ( !CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl") )
		return false;

	// 轉換茶壼格式
	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX10.ConvertToDX10Model(&g_Model);

	// 設定Vertex資料格式
	{
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	D3D10_BUFFER_DESC cbDesc;

	// vertex buffer
	{
		g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(g_Quad_v), g_Quad_v);
		if ( g_pVertexBuffer==NULL )
			return false;
	}
	// 配置Shader參數的記憶體空間
	{
		g_pConstantBuffer = GutCreateShaderConstant_DX10(sizeof(Matrix4x4));
		if ( NULL==g_pConstantBuffer )
			return false;
	}
	// rasterizer state物件
	{
		D3D10_RASTERIZER_DESC desc;
		GutSetDX10DefaultRasterizerDesc(desc);

		desc.CullMode = D3D10_CULL_NONE;

		if ( D3D_OK != g_pDevice->CreateRasterizerState(&desc, &g_pRasterizerState) )
			return false;

		g_pDevice->RSSetState(g_pRasterizerState);
	}
	// depth stencil state
	{
		D3D10_DEPTH_STENCIL_DESC desc;
		GutSetDX10DefaultDepthStencilDesc(desc);

		desc.StencilEnable = TRUE;
		desc.StencilReadMask = 0xff;
		desc.StencilWriteMask = 0xff;

		desc.FrontFace.StencilPassOp = D3D10_STENCIL_OP_REPLACE;
		desc.BackFace = desc.FrontFace;
		// 用來把墻壁所占據的像素stencil值設為1
		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencilState_Mark);
		// 用來把墻壁所占據的像素Z值設為1
		desc.DepthFunc = D3D10_COMPARISON_ALWAYS;
		desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;
		desc.BackFace = desc.FrontFace;
		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencilState_ZClear);
		// 用來畫鏡射物件
		desc.DepthFunc = D3D10_COMPARISON_LESS_EQUAL;
		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencilState_Mirror);
		// 用來畫一般的物件的
		desc.StencilEnable = FALSE;
		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencilState_Regular);
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
	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

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

	SAFE_RELEASE(g_pZStencilState_Mark);
	SAFE_RELEASE(g_pZStencilState_ZClear);
	SAFE_RELEASE(g_pZStencilState_Mirror);
	SAFE_RELEASE(g_pZStencilState_Regular);

	SAFE_RELEASE(g_pSamplerState);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	// 重設水平跟垂直方向的視角, 并重新計算投影矩陣
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
}

static void RenderModelDX10(bool mirror, Vector4 *pPlane)
{
	Matrix4x4 view_matrix;

	if ( mirror )
	{
		Vector4 vEye = g_Control.GetCameraPosition();
		Vector4 vLookAt = g_Control.m_vLookAt;
		Vector4 vUp = g_Control.m_vUp;

		Vector4 mirror_eye = MirrorPoint(vEye, *pPlane);
		Vector4 mirror_lookat = MirrorPoint(vLookAt, *pPlane);
		Vector4 mirror_up = MirrorVector(vUp, *pPlane);

		Matrix4x4 temp_matrix = GutMatrixLookAtRH(mirror_eye, mirror_lookat, mirror_up);

		// 因為是鏡射, 在轉換到鏡頭座標系後要做個左右對調的動作.
		Matrix4x4 mirror_x;
		mirror_x.Identity();
		mirror_x.Scale(-1.0f, 1.0f, 1.0f);

		view_matrix = temp_matrix * mirror_x;
	}
	else
	{
		view_matrix = g_Control.GetViewMatrix();
	}

	CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
	CGutModel_DX10::SetViewMatrix(view_matrix);
	CGutModel_DX10::SetWorldMatrix(g_Control.GetObjectMatrix());
	CGutModel_DX10::UpdateMatrix();

	g_Model_DX10.Render();
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.5f, 0.0f);
	Vector4 vPlane(0.0f, 0.0f, 1.0f, -g_mirror_z);

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

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();

	g_pDevice->OMSetDepthStencilState(g_pZStencilState_Regular, 1);
	RenderModelDX10(false, NULL);

	// 畫墻壁
	{
		UINT stride = sizeof(Vertex_V);
		UINT offset = 0;
		// 設定Shader
		g_pDevice->VSSetShader(g_pVertexShader);
		g_pDevice->PSSetShader(g_pPixelShader);
		// 設定vertex shader讀取參數的記憶體位罝
		g_pDevice->VSSetConstantBuffers(0, 1, &g_pConstantBuffer);
		// 設定vertex資料格式
		g_pDevice->IASetInputLayout(g_pVertexLayout);
		// 設定vertex buffer
		g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
		// 設定三角形頂點索引值資料排列是triangle strip
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
		// 更新轉換矩陣
		Matrix4x4 wvp = view_matrix * g_proj_matrix; 
		Matrix4x4 *pConstData;
		g_pConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		*pConstData = wvp;
		g_pConstantBuffer->Unmap();
		// 把墻壁所填充的像素Stencil值設定1
		g_pDevice->OMSetDepthStencilState(g_pZStencilState_Mark, 1);
		// 畫出墻壁
		g_pDevice->Draw(4, 0);
	}
	// 把墻壁所占據的像素Z值清為1
	{
		// 把墻壁所填充的像素Stencil值設定1
		g_pDevice->OMSetDepthStencilState(g_pZStencilState_ZClear, 1);
		// 把viewport的Z值范圍改成0-1
		UINT nViewports = 1;
		D3D10_VIEWPORT vp;
		g_pDevice->RSGetViewports(&nViewports, &vp);
		vp.MinDepth = vp.MaxDepth = 1.0f;
		g_pDevice->RSSetViewports(1, &vp);
		// 畫出墻壁
		g_pDevice->Draw(4, 0);
		// 還原viewport設定
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		g_pDevice->RSSetViewports(1, &vp);
	}
	// 畫出鏡射的茶壼
	{
		g_pDevice->OMSetDepthStencilState(g_pZStencilState_Mirror, 1);
		RenderModelDX10(true, &vPlane);
	}

	// 等待硬體掃結束, 然後才更新畫面.
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_