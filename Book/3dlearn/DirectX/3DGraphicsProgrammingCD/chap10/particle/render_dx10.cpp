#ifdef _ENABLE_DX10_

#include <memory.h>
#include <D3DX10.h>

#include "Gut.h"
#include "GutDX10.h"
#include "GutTexture_DX10.h"
#include "GutModel_DX10.h"

#include "render_data.h"

static ID3D10Device	*g_pDevice = NULL;
static ID3D10VertexShader *g_pVertexShader = NULL;
static ID3D10PixelShader *g_pPixelShader = NULL;
static ID3D10InputLayout *g_pVertexLayout = NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;

static ID3D10Buffer *g_pVertexBuffer = NULL;
static ID3D10Buffer *g_pIndexBuffer = NULL;
static ID3D10Buffer *g_pMatrixConstant = NULL;
static ID3D10Buffer *g_pMaterialConstant = NULL;

static ID3D10BlendState *g_pBlendState = NULL;
static ID3D10DepthStencilState *g_pZWriteOff = NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Shader
	{
		D3D10_SHADER_MACRO macros[] =
		{
			"_VERTEXCOLOR", "1",
			"_DIFFUSETEX0", "1",
			NULL, NULL,
		};

		g_pTexture = GutLoadTexture_DX10("../../textures/particle.tga");
		if ( g_pTexture==NULL )
			return false;

		GutSetHLSLShaderMacrosDX10(macros);

		g_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/gmodel_dx10.hlsl", 
			"VS", "vs_4_0", &pVSCode);
		g_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/gmodel_dx10.hlsl", 
			"PS", "ps_4_0");

		if ( g_pVertexShader==NULL || g_pPixelShader==NULL )
			return false;
	}
	// 設定頂點資料格式
	{
		//
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, 12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};
		//
		if ( D3D_OK != g_pDevice->CreateInputLayout(layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), 
			pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &g_pVertexLayout ) )
			return false;
	}
	// Shader常數的記憶體空間
	g_pMaterialConstant = GutCreateShaderConstant_DX10(sizeof(sMaterialConstants));
	g_pMatrixConstant = GutCreateShaderConstant_DX10(sizeof(sMatrixConstants));
	// 模型記憶體空間
	g_pVertexBuffer = GutCreateVertexBuffer_DX10(g_Particle.m_iNumMaxParticles * sizeof(sParticleVertex) * 4);
	g_pIndexBuffer = GutCreateIndexBuffer_DX10(g_Particle.m_iNumMaxParticles * 6 * sizeof(short), g_Particle.m_pIndexArray);
	// ADD混色
	{
		D3D10_BLEND_DESC desc;

		GutSetDX10DefaultBlendDesc(desc);
		desc.BlendEnable[0] = TRUE;
		desc.SrcBlend = D3D10_BLEND_ONE;
		desc.DestBlend = D3D10_BLEND_ONE;

		g_pDevice->CreateBlendState(&desc, &g_pBlendState);
	}
	// 關閉ZWrite
	{
		D3D10_DEPTH_STENCIL_DESC desc;
		GutSetDX10DefaultDepthStencilDesc(desc);
		desc.DepthWriteMask = D3D10_DEPTH_WRITE_MASK_ZERO;

		g_pDevice->CreateDepthStencilState(&desc, &g_pZWriteOff);
	}

	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);

	return true;
}

bool ReleaseResourceDX10(void)
{
	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	// 
	float aspect = (float)width/(float)height;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// 取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	// 清除畫面
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	//
	if ( g_Particle.m_iNumParticles )
	{
		g_pDevice->VSSetShader(g_pVertexShader);
		g_pDevice->PSSetShader(g_pPixelShader);
		//
		Matrix4x4 wvp_matrix = g_Control.GetObjectMatrix() * g_Control.GetViewMatrix() * g_proj_matrix;
		sMatrixConstants *pConstant;
		g_pMatrixConstant->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **)&pConstant);
		pConstant->wvp_matrix = wvp_matrix;
		pConstant->texture_matrix[0].Identity();
		pConstant->texture_matrix[1].Identity();
		g_pMatrixConstant->Unmap();
		//
		ID3D10Buffer *buffers[2] = {g_pMatrixConstant, g_pMaterialConstant};
		g_pDevice->VSSetConstantBuffers(0, 2, buffers);
		g_pDevice->PSSetConstantBuffers(0, 2, buffers);
		//
		g_pDevice->PSSetShaderResources(0, 1, &g_pTexture);
		//
		UINT stride = sizeof(sParticleVertex);
		UINT offset = 0;
		//
		sParticleVertex *pVertices;
		g_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **)&pVertices);
		memcpy(pVertices, g_Particle.m_pVertexArray, sizeof(sParticleVertex) * g_Particle.m_iNumParticles * 4);
		g_pVertexBuffer->Unmap();

		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
		g_pDevice->IASetIndexBuffer(g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		g_pDevice->IASetInputLayout(g_pVertexLayout);

		float dummy[4];
		g_pDevice->OMSetBlendState(g_pBlendState, dummy, 0xffffffff);
		g_pDevice->OMSetDepthStencilState(g_pZWriteOff, 0);

		if ( !g_bPointSprite )
			g_pDevice->DrawIndexed(g_Particle.m_iNumParticles*6, 0, 0);
	}
	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_