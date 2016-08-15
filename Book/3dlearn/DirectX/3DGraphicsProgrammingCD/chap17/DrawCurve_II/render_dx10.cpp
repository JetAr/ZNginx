
#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_DX10.h"

static ID3D10Device	*g_pDevice = NULL;
static ID3D10Effect	*g_pDrawCurveFX = NULL;
static ID3D10Buffer *g_pStreamOutBuffer = NULL;
static ID3D10InputLayout *g_pLayout = NULL;

static int g_iOldMode = -1;

bool ReInitResourceDX10(void)
{
	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	g_pDrawCurveFX = GutLoadFXShaderDX10("DrawCurve2_DX10.fx");
	if ( NULL==g_pDrawCurveFX )
		return false;

	g_pStreamOutBuffer = GutCreateBufferObject_DX10(D3D10_BIND_VERTEX_BUFFER | D3D10_BIND_STREAM_OUTPUT, 
		D3D10_USAGE_DEFAULT, sizeof(Vector4)*256, NULL);

	const D3D10_INPUT_ELEMENT_DESC layout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D10_INPUT_PER_VERTEX_DATA, 0 },
	};

	ID3D10EffectTechnique *pShader = g_pDrawCurveFX->GetTechniqueByName("DrawCurve");
	D3D10_PASS_DESC PassDesc;
	pShader->GetPassByIndex( 0 )->GetDesc( &PassDesc );

	g_pDevice->CreateInputLayout(layout, 1, 
		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pLayout);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pDrawCurveFX);
	SAFE_RELEASE(g_pStreamOutBuffer);
	SAFE_RELEASE(g_pLayout);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	ReInitResourceDX10();
}

void BuildCurve(void)
{
	UINT offset[1] = { 0 };
	UINT stride_zero[1] = {0};
	ID3D10Buffer *pDummy[1] = {NULL};

	ID3D10EffectTechnique *pShader = NULL;

	switch(g_iMode)
	{
	default:
	case 1:
		{
			pShader = g_pDrawCurveFX->GetTechniqueByName("SinCurve");
			break;
		}
	case 2:
		{
			pShader = g_pDrawCurveFX->GetTechniqueByName("ZCurve");

			Vector4 vNearFar(1.0f, 100.0f, 0.0f, 0.0f);
			Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_DirectX(45.0f, 1.0f, vNearFar[0], vNearFar[1]);
			//Matrix4x4 proj_matrix = GutMatrixOrthoRH_DirectX(10.0f, 10.0f, vNearFar[0], vNearFar[1]);

			ID3D10EffectMatrixVariable *pMatrix = 
				g_pDrawCurveFX->GetVariableByName("proj_matrix")->AsMatrix();
			ID3D10EffectVectorVariable *pNearFarPlane = 
				g_pDrawCurveFX->GetVariableByName("NearFarPlane")->AsVector();

			pMatrix->SetMatrix(&proj_matrix[0][0]);
			pNearFarPlane->SetFloatVector(&vNearFar[0]);

			break;
		}
	}

	g_pDevice->SOSetTargets(1, &g_pStreamOutBuffer, offset);
	g_pDevice->IASetVertexBuffers(0, 1, pDummy, stride_zero, offset);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_POINTLIST);

	pShader->GetPassByIndex(0)->Apply(0);
	g_pDevice->Draw(1, 0);

	g_iOldMode = g_iMode;
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// 取得主畫面
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 

	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);

	UINT offset[1] = { 0 };
	UINT stride[1] = { sizeof(Vector4) };
	ID3D10Buffer *pDummy[1] = {NULL};

	if ( g_iOldMode != g_iMode )
	{
		BuildCurve();
	}

	ID3D10EffectTechnique *pShader = g_pDrawCurveFX->GetTechniqueByName("DrawCurve");
	pShader->GetPassByIndex(0)->Apply(0);

	{
		g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
		g_pDevice->SOSetTargets(1, pDummy, offset);
		g_pDevice->IASetVertexBuffers(0, 1, &g_pStreamOutBuffer, stride, offset);
		g_pDevice->IASetInputLayout(g_pLayout);

		g_pDevice->DrawAuto();
	}

	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); 
	pSwapChain->Present(1, 0);
}

