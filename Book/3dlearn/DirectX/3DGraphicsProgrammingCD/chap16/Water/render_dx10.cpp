#ifdef _ENABLE_DX10_

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_DX10.h"
#include "GutTexture_DX10.h"

enum BufferType
{
	TEX_HEIGHT0,
	TEX_HEIGHT1,
	TEX_HEIGHT2,
	TEX_NORMAL,
	TEX_TEXTURES
};

static ID3D10Device	*g_pDevice = NULL;

static ID3D10Effect	*g_pWaterFX = NULL;
static ID3D10Effect *g_pPostFX = NULL;

static ID3D10ShaderResourceView *g_pWaterTexture = NULL;

static ID3D10Texture2D *g_Textures[TEX_TEXTURES];
static ID3D10ShaderResourceView *g_RSView[TEX_TEXTURES];
static ID3D10RenderTargetView *g_RTView[TEX_TEXTURES];

static ID3D10InputLayout *g_pImpulseLayout = NULL;
static ID3D10InputLayout *g_pVertexLayout = NULL;

static ID3D10Buffer *g_pVertexBuffer = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_orient_matrix;

static CGutModel_DX10 g_Model_DX10;

static Vector4 g_vTextureSize;

bool ReInitResourceDX10(void)
{
	GutGetWindowSize(g_iWidth, g_iHeight);

	g_proj_matrix = GutMatrixOrthoRH_DirectX(g_iWidth, g_iHeight, 1.0f, 100.0f);
	// g_proj_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, 1.0f, 1000.0f);

	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		GutCreateRenderTarget_DX10(g_iWidth, g_iHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 
			&g_Textures[i], &g_RSView[i], &g_RTView[i]);
	}

	g_vTextureSize[0] = 1.0f/(float)g_iWidth;
	g_vTextureSize[1] = 1.0f/(float)g_iHeight;
	g_vTextureSize[2] = (float)g_iWidth;
	g_vTextureSize[3] = (float)g_iHeight;

	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	g_orient_matrix.Identity();

	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		g_Textures[i] = NULL;
		g_RSView[i] = NULL;
		g_RTView[i] = NULL;
	}

	g_pWaterFX = GutLoadFXShaderDX10("../../shaders/Watereffect_dx10.fx");
	if ( NULL==g_pWaterFX )
		return false;

	g_pPostFX = GutLoadFXShaderDX10("../../shaders/Posteffect_dx10.fx");
	if ( NULL==g_pPostFX )
		return false;

	{
		const sVertexDecl *pVertexDecl = g_Model.GetVertexFormat();

		ID3D10EffectTechnique *pShader = g_pWaterFX->GetTechniqueByName("AddImpulse");

		D3D10_PASS_DESC PassDesc;
		pShader->GetPassByIndex(0)->GetDesc(&PassDesc);

		g_pImpulseLayout = GutCreateInputLayoutDX10(pVertexDecl, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize);
	}

	// 建立一片矩形, 水波模擬時會用到.
	{
		g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(Vertex_VT)*4, g_FullScreenQuad);

		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		ID3D10EffectTechnique *pShader = g_pWaterFX->GetTechniqueByName("WaterSimulation");

		D3D10_PASS_DESC PassDesc;
		pShader->GetPassByIndex(0)->GetDesc(&PassDesc);

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout ) )
			return false;
	}

	if ( !CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl") )
		return false;
	g_Model_DX10.ConvertToDX10Model(&g_Model);

	g_pWaterTexture = GutLoadTexture_DX10("../../textures/bark_loop.tga");

	ReInitResourceDX10();

	return true;
}

bool ReleaseResourceDX10(void)
{
	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		SAFE_RELEASE(g_Textures[i]);
		SAFE_RELEASE(g_RSView[i]);
		SAFE_RELEASE(g_RTView[i]);
	}

	SAFE_RELEASE(g_pImpulseLayout);

	SAFE_RELEASE(g_pWaterFX);
	SAFE_RELEASE(g_pPostFX);

	SAFE_RELEASE(g_pWaterTexture);

	return true;
}

void ResizeWindowDX10(int width, int height)
{
	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		SAFE_RELEASE(g_Textures[i]);
		SAFE_RELEASE(g_RSView[i]);
		SAFE_RELEASE(g_RTView[i]);
	}

	GutResetGraphicsDeviceDX10();

	ReInitResourceDX10();
}

static void SwapHeightmaps(void)
{
	ID3D10Texture2D *texture = g_Textures[TEX_HEIGHT0];
	ID3D10ShaderResourceView *rsview = g_RSView[TEX_HEIGHT0];
	ID3D10RenderTargetView *rtview = g_RTView[TEX_HEIGHT0];

	g_Textures[TEX_HEIGHT0] = g_Textures[TEX_HEIGHT1];
	g_RSView[TEX_HEIGHT0] = g_RSView[TEX_HEIGHT1];
	g_RTView[TEX_HEIGHT0] = g_RTView[TEX_HEIGHT1];

	g_Textures[TEX_HEIGHT1] = g_Textures[TEX_HEIGHT2];
	g_RSView[TEX_HEIGHT1] = g_RSView[TEX_HEIGHT2];
	g_RTView[TEX_HEIGHT1] = g_RTView[TEX_HEIGHT2];

	g_Textures[TEX_HEIGHT2] = texture;
	g_RSView[TEX_HEIGHT2] = rsview;
	g_RTView[TEX_HEIGHT2] = rtview;
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

void DrawImage(ID3D10ShaderResourceView *pTexture, float x, float y, float w, float h)
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

static void AddImpulse(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	static Vector4 vPosition(0.0f, 0.0f, 0.0f, 0.0f);
	//Vector4 vDiff = vPosition - g_vPosition;
	Vector4 vDiff = g_vPosition - vPosition;
	Vector4 vLength = vDiff.Length();

	if ( vLength[0]<2.0f )
		return;

	Vector4 vDir = vDiff / vLength;
	Vector4 vVec0(vDir[1],-vDir[0], 0.0f, 0.0f);
	Vector4 vVec1(vDir[0], vDir[1], 0.0f, 0.0f);

	vPosition = g_vPosition;

	Vector4 vVec0_old = g_orient_matrix[0];
	Vector4 vVec1_old = g_orient_matrix[1];

	Vector4 vVec0_new = VectorLerp(vVec0_old, vVec0, 0.2f);
	Vector4 vVec1_new = VectorLerp(vVec1_old, vVec1, 0.2f);
	vVec0_new.Normalize();
	vVec1_new.Normalize();

	g_orient_matrix.Identity();
	g_orient_matrix[0] = vVec0_new;
	g_orient_matrix[1] = vVec1_new;

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();

	Matrix4x4 scale_matrix; 
	scale_matrix.Scale_Replace(g_fRippleSize, g_fRippleSize, 1.0f);

	Matrix4x4 world_matrix = g_orient_matrix * scale_matrix;
	world_matrix[3] = g_vPosition;

	Matrix4x4 wvp_matrix = world_matrix * view_matrix * g_proj_matrix;

	ID3D10EffectTechnique *pShader = g_pWaterFX->GetTechniqueByName("AddImpulse");
	ID3D10EffectMatrixVariable *wvp_matrix_param = g_pWaterFX->GetVariableByName("wvp_matrix")->AsMatrix();
	ID3D10EffectScalarVariable *force_param = g_pWaterFX->GetVariableByName("fForce")->AsScalar();

	wvp_matrix_param->SetMatrix( (float *)&wvp_matrix );
	force_param->SetFloat(0.5f);

	g_pDevice->OMSetRenderTargets(1, &g_RTView[TEX_HEIGHT1], NULL);

	pShader->GetPassByIndex(0)->Apply(0);
	g_pDevice->IASetInputLayout(g_pImpulseLayout);

	g_Model_DX10.Render(0);
}

static void WaterSimulation(void)
{
	g_pDevice->OMSetRenderTargets(1, &g_RTView[TEX_HEIGHT2], NULL);

	ID3D10EffectTechnique *pShader = g_pWaterFX->GetTechniqueByName("WaterSimulation");

	ID3D10EffectShaderResourceVariable *heightmap_current_param = g_pWaterFX->GetVariableByName("heightmap_current")->AsShaderResource();
	ID3D10EffectShaderResourceVariable *heightmap_prev_param = g_pWaterFX->GetVariableByName("heightmap_prev")->AsShaderResource();

	ID3D10EffectVectorVariable *texturesize_param = g_pWaterFX->GetVariableByName("vTextureSize")->AsVector();
	ID3D10EffectScalarVariable *damping_param = g_pWaterFX->GetVariableByName("fDamping")->AsScalar();

	heightmap_prev_param->SetResource( g_RSView[TEX_HEIGHT0] );
	heightmap_current_param->SetResource( g_RSView[TEX_HEIGHT1] );

	texturesize_param->SetFloatVector(&g_vTextureSize[0]);
	damping_param->SetFloat(0.99f);

	pShader->GetPassByIndex(0)->Apply(0);

	DrawFullScreenQuad();
}

static void HeightmapToNormal(void)
{
	g_pDevice->OMSetRenderTargets(1, &g_RTView[TEX_NORMAL], NULL);

	ID3D10EffectTechnique *pShader = g_pWaterFX->GetTechniqueByName("ConvertNormal");

	ID3D10EffectShaderResourceVariable *heightmap_current_param = g_pWaterFX->GetVariableByName("heightmap_current")->AsShaderResource();
	ID3D10EffectScalarVariable *normalscale_param = g_pWaterFX->GetVariableByName("fNormalScale")->AsScalar();

	heightmap_current_param->SetResource( g_RSView[TEX_HEIGHT2] );
	normalscale_param->SetFloat(1.0f);

	pShader->GetPassByIndex(0)->Apply(0);

	DrawFullScreenQuad();
}

static void RenderWater(void)
{
	ID3D10EffectTechnique *pShader = g_pWaterFX->GetTechniqueByName("Water");

	ID3D10EffectShaderResourceVariable *normalmap_param = g_pWaterFX->GetVariableByName("NormalmapTex")->AsShaderResource();
	ID3D10EffectShaderResourceVariable *water_param = g_pWaterFX->GetVariableByName("WaterTex")->AsShaderResource();
	ID3D10EffectScalarVariable *texcoordscale_param = g_pWaterFX->GetVariableByName("fTexcoordScale")->AsScalar();

	normalmap_param->SetResource( g_RSView[TEX_NORMAL] );
	water_param->SetResource( g_pWaterTexture );
	texcoordscale_param->SetFloat(0.2f);

	pShader->GetPassByIndex(0)->Apply(0);

	DrawFullScreenQuad();
}

static void DrawObject(void)
{
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();

	Matrix4x4 scale_matrix; 
	scale_matrix.Scale_Replace(g_fRippleSize, g_fRippleSize, 1.0f);

	Matrix4x4 world_matrix = g_orient_matrix * scale_matrix;
	world_matrix[3] = g_vPosition;

	g_Model_DX10.SetProjectionMatrix(g_proj_matrix);
	g_Model_DX10.SetWorldMatrix(world_matrix);
	g_Model_DX10.SetViewMatrix(view_matrix);
	g_Model_DX10.UpdateMatrix();

	// set depth stencil states to default settings
	g_pDevice->OMSetDepthStencilState(NULL, 0);

	g_Model_DX10.Render();
}

void RenderFrameDX10(void)
{
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ID3D10ShaderResourceView *dummy_views[3] = {NULL, NULL, NULL};

	AddImpulse();
	WaterSimulation();
	HeightmapToNormal();

	// 回到主畫面
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView();
	g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);

	RenderWater();
	DrawObject();

	SwapHeightmaps();

	float x = -1.0f;
	float y = -1.0f;
	float w = 0.3f;
	float h = 0.3f;

	if ( g_iMode & 0x01 )
	{
		DrawImage(g_RSView[TEX_HEIGHT2], x, y, w, h);
		x+=w;
	}

	if ( g_iMode & 0x02 )
	{
		DrawImage(g_RSView[TEX_NORMAL], x, y, w, h);
		x+=w;
	}

	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_