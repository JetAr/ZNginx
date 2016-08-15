#ifdef _ENABLE_DX10_

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_DX10.h"

static ID3D10Device	*g_pDevice = NULL;

static ID3D10Effect	*g_pDrawCurveFX = NULL;

bool ReInitResourceDX10(void)
{
	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	g_pDrawCurveFX = GutLoadFXShaderDX10("DrawCurve_DX10.fx");
	if ( NULL==g_pDrawCurveFX )
		return false;

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pDrawCurveFX);

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

	// 取得主畫面
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 

	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);

	ID3D10EffectTechnique *g_pShader = NULL;

	switch(g_iMode)
	{
	default:
	case 1:
		g_pShader = g_pDrawCurveFX->GetTechniqueByName("SinCurve");
		break;
	case 2:
		{
			g_pShader = g_pDrawCurveFX->GetTechniqueByName("ZCurve");

			Vector4 vNearFar(1.0f, 100.0f, 0.0f, 0.0f);
			Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_DirectX(45.0f, 1.0f, vNearFar[0], vNearFar[1]);
			//Matrix4x4 proj_matrix = GutMatrixOrthoRH_DirectX(10.0f, 10.0f, vNearFar[0], vNearFar[1]);
			//Matrix4x4 proj_matrix = GutMatrixOrthoRH_OpenGL(10.0f, 10.0f, vNearFar[0], vNearFar[1]);
			//Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_OpenGL(45.0f, 1.0f, vNearFar[0], vNearFar[1]);

			ID3D10EffectMatrixVariable *pMatrix = 
				g_pDrawCurveFX->GetVariableByName("proj_matrix")->AsMatrix();
			ID3D10EffectVectorVariable *pNearFarPlane = 
				g_pDrawCurveFX->GetVariableByName("NearFarPlane")->AsVector();

			pMatrix->SetMatrix(&proj_matrix[0][0]);
			pNearFarPlane->SetFloatVector(&vNearFar[0]);

			break;
		}
	}

	g_pShader->GetPassByIndex(0)->Apply(0);
	g_pDevice->Draw(2, 0);

	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_