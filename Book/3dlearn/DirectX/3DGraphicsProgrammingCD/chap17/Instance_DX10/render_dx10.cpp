

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_DX10.h"

static ID3D10Device	*g_pDevice = NULL;
static ID3D10Effect	*g_pInstanceFX = NULL;
static ID3D10InputLayout *g_pInputLayout = NULL;

static CGutModel g_Model;
static CGutModel_DX10 g_Model_DX10;
static Matrix4x4 g_proj_matrix;

bool ReInitResourceDX10(void)
{
	int w,h; GutGetWindowSize(w, h);
	float fAspect = (float)h/(float)w;

	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(72, fAspect, 0.1f, 100.0f);

	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	g_pInstanceFX = GutLoadFXShaderDX10("InstanceID.fx");
	if ( NULL==g_pInstanceFX )
		return false;

	g_Model.SetTexturePath("../../textures/");
	g_Model.Load_ASCII("../../models/earth.gma");
	g_Model_DX10.ConvertToDX10Model(&g_Model);

	ID3D10EffectTechnique *pShader = g_pInstanceFX->GetTechniqueByName("RenderInstance");
	const sVertexDecl *pVertexDecl = g_Model.GetVertexFormat();
	D3D10_PASS_DESC PassDesc;

	pShader->GetPassByIndex(0)->GetDesc(&PassDesc);
	g_pInputLayout = GutCreateInputLayoutDX10(pVertexDecl, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize);

	ReInitResourceDX10();

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pInstanceFX);
	g_Model_DX10.Release();

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	ReInitResourceDX10();
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	const int num_instances = 4;

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 vp_matrix = view_matrix * g_proj_matrix;
	Matrix4x4 wvp_matrix[num_instances];
	Vector4 instance_pos[num_instances];

	instance_pos[0].Set(-1.0f, -1.0f, 0.0f, 0.0f);
	instance_pos[1].Set( 1.0f, -1.0f, 0.0f, 0.0f);
	instance_pos[2].Set( 1.0f,  1.0f, 0.0f, 0.0f);
	instance_pos[3].Set(-1.0f,  1.0f, 0.0f, 0.0f);

	// 取得主畫面
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 

	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	ID3D10EffectTechnique *pShader = g_pInstanceFX->GetTechniqueByName("RenderInstance");
	ID3D10EffectMatrixVariable *pWVPMatrixVar = g_pInstanceFX->GetVariableByName("wvp_matrices")->AsMatrix();

	for ( int i=0; i<num_instances; i++ )
	{
		Matrix4x4 instance_matrix = world_matrix;
		instance_matrix.Translate(instance_pos[i]);
		wvp_matrix[i] = instance_matrix * vp_matrix;
	}

	pWVPMatrixVar->SetMatrixArray((float*)&wvp_matrix[0], 0, num_instances);

	g_pDevice->IASetInputLayout(g_pInputLayout);

	pShader->GetPassByIndex(0)->Apply(0);
	g_Model_DX10.RenderInstance(num_instances, 0, 0);

	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 
	pSwapChain->Present(1, 0);
}

