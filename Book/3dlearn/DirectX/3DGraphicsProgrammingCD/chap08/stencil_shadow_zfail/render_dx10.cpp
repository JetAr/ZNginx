#ifdef _ENABLE_DX10_

#include "render_data.h"

#include "Gut.h"
#include "GutModel_DX10.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10BlendState		*g_pBlendState_ColorWriteOff = NULL;

static ID3D10DepthStencilState *g_pZStencil_Regular = NULL;
static ID3D10DepthStencilState *g_pZStencil_Mask = NULL;
static ID3D10DepthStencilState *g_pZStencil_Test = NULL;

static ID3D10SamplerState	*g_pSamplerState = NULL;

static Matrix4x4 g_proj_matrix;

static CGutModel_DX10 g_Model_DX10;

static sModelMaterial_DX10 g_material_stencilpass;
static sModelMaterial_DX10 g_material_spotlightpass;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl");
	CGutModel::SetTexturePath("../../textures/");

	g_Model_DX10.ConvertToDX10Model(&g_Model_textured);

	// depth stencil object
	{
		D3D10_DEPTH_STENCIL_DESC desc;
		GutSetDX10DefaultDepthStencilDesc(desc);

		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencil_Regular);

		desc.StencilEnable = TRUE;
		desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;
		desc.FrontFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_DECR;
		desc.BackFace.StencilFunc = D3D10_COMPARISON_ALWAYS;
		desc.BackFace.StencilDepthFailOp = D3D10_STENCIL_OP_INCR;

		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencil_Mask);

		desc.DepthEnable = FALSE;
		desc.FrontFace.StencilFunc = D3D10_COMPARISON_EQUAL;
		desc.FrontFace.StencilDepthFailOp = D3D10_STENCIL_OP_KEEP;
		desc.BackFace = desc.FrontFace;

		g_pDevice->CreateDepthStencilState(&desc, &g_pZStencil_Test);
	}
	// disable rgba output
	{
		D3D10_BLEND_DESC desc;
		GutSetDX10DefaultBlendDesc(desc);

		desc.RenderTargetWriteMask[0] = 0;
		g_pDevice->CreateBlendState(&desc, &g_pBlendState_ColorWriteOff);
	}
	// material overwrite
	{
		g_material_stencilpass.m_bCullFace = false;
		g_material_stencilpass.m_pBlendState = g_pBlendState_ColorWriteOff;
		g_material_stencilpass.m_vDiffuse.Set(0.1f);

		g_material_spotlightpass.m_bCullFace = true;
		g_material_spotlightpass.m_pBlendState = CGutModel_DX10::s_pBlend_Add;
		g_material_spotlightpass.m_vDiffuse.Set(0.3f);
	}
	// sampler state / texture filter
	{
		D3D10_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

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
	SAFE_RELEASE(g_pBlendState_ColorWriteOff);
	SAFE_RELEASE(g_pZStencil_Mask);
	SAFE_RELEASE(g_pZStencil_Test);
	SAFE_RELEASE(g_pSamplerState);

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
	// 取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	// 清除顏色
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// 清除Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// 取得轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 ident_matrix = Matrix4x4::IdentityMatrix();

	CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
	CGutModel_DX10::SetViewMatrix(view_matrix);
	// 畫出茶具組
	{
		CGutModel_DX10::SetWorldMatrix(world_matrix);
		CGutModel_DX10::UpdateMatrix();
		g_Model_DX10.Render();
	}
	// 等待硬體掃結束, 然後才更新畫面
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_