#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "Gut.h"
#include "render_data.h"
#include "GutModel_DX10.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;

static ID3D10Buffer			*g_pVertexBuffer	= NULL;

static ID3D10Buffer			*g_pConstantBuffer	= NULL;
static ID3D10VertexShader	*g_pVertexShader	= NULL;
static ID3D10PixelShader	*g_pPixelShader		= NULL;
static ID3D10RasterizerState *g_pRasterizerState= NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;

CGutModel_DX10 g_Model_DX10;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_view_matrix;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;


	// 計算投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, g_fNear, g_fFar);

	if ( !CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl") )
		return false;
	g_Model_DX10.ConvertToDX10Model(&g_Model);

	return true;
}

bool ReleaseResourceDX10(void)
{
	g_Model_DX10.Release();

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNear, g_fFar);
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// 取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer

	g_pDevice->ClearRenderTargetView(pRenderTargetView, &vClearColor[0]);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);

	Matrix4x4 camera_matrix = g_Control.GetCameraMatrix();
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	g_Model_DX10.SetLight(0, g_Lights[0]);
	g_Model_DX10.SetLight(1, g_Lights[1]);
	g_Model_DX10.EnableLighting(true);

	g_Model_DX10.SetProjectionMatrix(g_proj_matrix);
	g_Model_DX10.SetWorldMatrix(world_matrix);
	g_Model_DX10.SetViewMatrix(view_matrix);
	g_Model_DX10.SetInvViewMatrix(camera_matrix);
	g_Model_DX10.UpdateMatrix();

	g_Model_DX10.Render();

	// 等待硬體掃結束, 然後才更新畫面
	pSwapChain->Present(1, 0);
}

#else

void ResizeWindowDX10(int width, int height) {}
void RenderFrameDX10(void) {}
bool ReleaseResourceDX10(void) { return false; }
bool InitResourceDX10(void) { return false; }

#endif // _ENABLE_DX10_