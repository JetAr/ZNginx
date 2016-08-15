#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutWin32.h"
#include "GutDX10.h"
#include "GutTexture_DX10.h"
#include "GutModel_DX10.h"

#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;
static ID3D10InputLayout	*g_pRefractionLayout = NULL;

static ID3D10Effect			*g_pRefractionFX = NULL;
static ID3D10Effect			*g_pPostFX = NULL;
static ID3D10Buffer			*g_pVertexBuffer = NULL;

static ID3D10ShaderResourceView *g_pTexture = NULL;

static CGutModel_DX10 g_Model_DX10;

static Matrix4x4 g_proj_matrix;

bool ReInitResourceDX10(void)
{
	int w, h;
	GutGetWindowSize(w, h);

	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNearZ, g_fFarZ);

	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	// 載入Shader
	{
		g_pRefractionFX = GutLoadFXShaderDX10("../../shaders/Refraction_dx10.fx");
		if ( NULL==g_pRefractionFX )
			return false;

		ID3D10EffectTechnique *pShader = g_pRefractionFX->GetTechniqueByName("Refraction");
		const sVertexDecl *pVertexDecl = g_Model.GetVertexFormat();

		D3D10_PASS_DESC PassDesc;
		pShader->GetPassByIndex(0)->GetDesc(&PassDesc);

		g_pRefractionLayout = GutCreateInputLayoutDX10(pVertexDecl, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize);
	}

	g_pPostFX = GutLoadFXShaderDX10("../../shaders/Posteffect_dx10.fx");
	if ( NULL==g_pPostFX )
		return false;

	{
		g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(Vertex_VT)*4, g_FullScreenQuad);

		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		ID3D10EffectTechnique *pShader = g_pPostFX->GetTechniqueByName("DrawIcon");

		D3D10_PASS_DESC PassDesc;
		pShader->GetPassByIndex(0)->GetDesc(&PassDesc);

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout ) )
			return false;
	}

	if ( !ReInitResourceDX10() )
		return false;

	CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl");
	g_Model_DX10.ConvertToDX10Model(&g_Model);

	g_pTexture = GutLoadTexture_DX10("../../textures/lena.dds");

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pRefractionFX);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	ReInitResourceDX10();
}

void DrawFullScreenQuad(void)
{
	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	g_pDevice->IASetInputLayout(g_pVertexLayout);
	g_pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	g_pDevice->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);
	g_pDevice->Draw(4, 0);
}

void DrawImage(ID3D10ShaderResourceView *pTexture, float x=-1, float y=-1, float w=2, float h=2)
{
	Vector4 vtable[4];

	vtable[0].Set(x, y, 0.0f, 1.0f);
	vtable[1].Set(x+w, y, 0.0f, 1.0f);
	vtable[2].Set(x, y+h, 0.0f, 1.0f);
	vtable[3].Set(x+w, y+h, 0.0f, 1.0f);

	Vector4 ttable[4] =
	{
		Vector4(0.0f, 1.0f, 0.0f, 1.0f),
		Vector4(1.0f, 1.0f, 0.0f, 1.0f),
		Vector4(0.0f, 0.0f, 0.0f, 1.0f),
		Vector4(1.0f, 0.0f, 0.0f, 1.0f)
	};

	ID3D10EffectTechnique *pShader = g_pPostFX->GetTechniqueByName("DrawIcon");

	ID3D10EffectShaderResourceVariable *pInputTexture = g_pPostFX->GetVariableByName("Image")->AsShaderResource();
	ID3D10EffectVectorVariable *vertex_table = g_pPostFX->GetVariableByName("vertex_table")->AsVector();
	ID3D10EffectVectorVariable *texcoord_table = g_pPostFX->GetVariableByName("texcoord_table")->AsVector();

	pInputTexture->SetResource(pTexture);
	vertex_table->SetFloatVectorArray( (float*)vtable, 0, 4);
	texcoord_table->SetFloatVectorArray( (float*)ttable, 0, 4);

	pShader->GetPassByIndex(0)->Apply(0);

	DrawFullScreenQuad();
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f);
	Vector4 vObjectColor(0.0f, 1.0f, 0.0f);

	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); //frame buffer
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); //depth/stencil buffer
	g_pDevice->ClearRenderTargetView(pRenderTargetView, &vClearColor[0]);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);

	// background image
	DrawImage(g_pTexture);

	// refraction

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wvp_matrix = world_matrix * view_matrix * g_proj_matrix;
	Matrix4x4 wv_matrix = world_matrix * view_matrix;

	ID3D10EffectTechnique *pShader = g_pRefractionFX->GetTechniqueByName("Refraction");
	ID3D10EffectShaderResourceVariable *texture_param = g_pRefractionFX->GetVariableByName("BackgroundImage")->AsShaderResource();
	ID3D10EffectMatrixVariable *wvpmatrix_param = g_pRefractionFX->GetVariableByName("wvp_matrix")->AsMatrix();
	ID3D10EffectMatrixVariable *wvmatrix_param = g_pRefractionFX->GetVariableByName("wv_matrix")->AsMatrix();
	ID3D10EffectVectorVariable *color_param = g_pRefractionFX->GetVariableByName("object_color")->AsVector();

	texture_param->SetResource(g_pTexture);
	wvpmatrix_param->SetMatrix( (float *)&wvp_matrix );
	wvmatrix_param->SetMatrix( (float *)&wv_matrix );
	color_param->SetFloatVector( (float *)&vObjectColor );

	pShader->GetPassByIndex(0)->Apply(0);
	g_pDevice->IASetInputLayout(g_pRefractionLayout);

	g_Model_DX10.Render(SUBMIT_CULLFACE);

	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain();
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_