#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutWin32.h"
#include "GutDX10.h"
#include "GutTexture_DX10.h"
#include "GutModel_DX10.h"

#include "render_data.h"

static ID3D10Device			*g_pDevice			= NULL;
static ID3D10InputLayout	*g_pVertexLayout	= NULL;
static ID3D10Buffer			*g_pVertexBuffer	= NULL;

enum ShaderType
{
	SHADER_LUMINANCE,
	SHADER_AVERAGE,
	SHADER_ADAPTIVE,
	SHADER_AUTOEXPOSURE,
	NUM_SHADERS
};

enum FrameType
{
	FULLSIZE,
	DOWNSAMPLED_256x256,
	DOWNSAMPLED_64x64,
	DOWNSAMPLED_16x16,
	DOWNSAMPLED_4x4,
	LUMINANCE_PREVIOUS,
	LUMINANCE_CURRENT,
	LUMINANCE_TEMP,
	NUM_FRAMEBUFFERS
};

struct sViewportSize
{
	int width;
	int height;
};

static sViewportSize g_ViewportSize[] =
{
	{512,512},
	{256,256},
	{64,64},
	{16,16},
	{4,4},
	{1,1},
	{1,1},
	{1,1},
	{0,0}
};

static ID3D10Effect	*g_pPostFX = NULL;
static ID3D10Effect	*g_pExposureFX = NULL;

static ID3D10Texture2D *g_pTextures[NUM_FRAMEBUFFERS];
static ID3D10ShaderResourceView *g_pSRView[NUM_FRAMEBUFFERS];
static ID3D10RenderTargetView *g_pRTView[NUM_FRAMEBUFFERS];

static CGutModel_DX10 g_Model_DX10;

static Matrix4x4 g_proj_matrix;

static void SwapRenderTarget(int ra, int rb)
{
	ID3D10Texture2D *texture = g_pTextures[rb];
	ID3D10ShaderResourceView *srview = g_pSRView[rb];
	ID3D10RenderTargetView *rtview = g_pRTView[rb];

	g_pTextures[rb] = g_pTextures[ra];
	g_pSRView[rb] = g_pSRView[ra];
	g_pRTView[rb] = g_pRTView[ra];

	g_pTextures[ra] = texture;
	g_pSRView[ra] = srview;
	g_pRTView[ra] = rtview;
}

bool ReInitResourceDX10(void)
{
	int w, h;
	GutGetWindowSize(w, h);

	g_ViewportSize[0].width = w;
	g_ViewportSize[0].height = h;

	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		SAFE_RELEASE(g_pTextures[i]);
		SAFE_RELEASE(g_pSRView[i]);
		SAFE_RELEASE(g_pRTView[i]);
	}

	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		if ( !GutCreateRenderTarget_DX10(
			g_ViewportSize[i].width, g_ViewportSize[i].height, 
			DXGI_FORMAT_R16G16B16A16_FLOAT, &g_pTextures[i], &g_pSRView[i], &g_pRTView[i]) )
			return false;
	}

	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNearZ, g_fFarZ);

	return true;
}

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		g_pTextures[i] = NULL;
		g_pSRView[i] = NULL;
		g_pRTView[i] = NULL;
	}

	// 載入Shader
	{
		g_pPostFX = GutLoadFXShaderDX10("../../shaders/Posteffect_dx10.fx");
		if ( NULL==g_pPostFX )
			return false;

		g_pExposureFX = GutLoadFXShaderDX10("../../shaders/Exposure_dx10.fx");
		if ( NULL==g_pExposureFX )
			return false;
	}

	// 設定Vertex資料格式
	{
		ID3D10EffectTechnique *pShader = g_pExposureFX->GetTechniqueByName("AutoExposure");

		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};

		D3D10_PASS_DESC PassDesc;
		pShader->GetPassByIndex(0)->GetDesc(&PassDesc);

		if ( D3D_OK != g_pDevice->CreateInputLayout( layout, 2, PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize, &g_pVertexLayout ) )
			return false;

		SAFE_RELEASE(pVSCode);
	}

	if ( !ReInitResourceDX10() )
		return false;

	CGutModel_DX10::LoadDefaultShader("../../shaders/gmodel_dx10.hlsl");
	g_Model_DX10.ConvertToDX10Model(&g_Model);

	g_pVertexBuffer = GutCreateVertexBuffer_DX10(sizeof(Vertex_VT)*4, g_FullScreenQuad);

	return true;
}

bool ReleaseResourceDX10(void)
{

	SAFE_RELEASE(g_pVertexLayout);
	SAFE_RELEASE(g_pVertexBuffer);

	SAFE_RELEASE(g_pPostFX);
	SAFE_RELEASE(g_pExposureFX);

	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		SAFE_RELEASE(g_pTextures[i]);
		SAFE_RELEASE(g_pSRView[i]);
		SAFE_RELEASE(g_pRTView[i]);
	}

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

static Vector4 vLuminanceTable(0.21f, 0.71f, 0.072f);

void SetViewport(int target)
{
	D3D10_VIEWPORT vp;

	vp.TopLeftX = vp.TopLeftY = 0;
	vp.Width = g_ViewportSize[target].width;
	vp.Height = g_ViewportSize[target].height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	g_pDevice->RSSetViewports(1, &vp);
}

static void ConvertToLuminance(void)
{
	g_pDevice->OMSetRenderTargets(1, &g_pRTView[DOWNSAMPLED_256x256], NULL);

	SetViewport(DOWNSAMPLED_256x256);

	ID3D10EffectTechnique *pShader = g_pExposureFX->GetTechniqueByName("Luminance");
	ID3D10EffectShaderResourceVariable *pImage_var = g_pExposureFX->GetVariableByName("Image0")->AsShaderResource();
	ID3D10EffectVectorVariable *pLuminanceTable_var = g_pExposureFX->GetVariableByName("vLuminanceTable")->AsVector();

	pImage_var->SetResource(g_pSRView[FULLSIZE]);
	pLuminanceTable_var->SetFloatVector( &vLuminanceTable[0] );

	pShader->GetPassByIndex(0)->Apply(0);

	DrawFullScreenQuad();
}

static void ConvertToLogLuminance(void)
{
	g_pDevice->OMSetRenderTargets(1, &g_pRTView[DOWNSAMPLED_256x256], NULL);

	SetViewport(DOWNSAMPLED_256x256);

	ID3D10EffectTechnique *pShader = g_pExposureFX->GetTechniqueByName("LogLuminance");
	ID3D10EffectShaderResourceVariable *pImage_var = g_pExposureFX->GetVariableByName("Image0")->AsShaderResource();
	ID3D10EffectVectorVariable *pLuminanceTable_var = g_pExposureFX->GetVariableByName("vLuminanceTable")->AsVector();

	pImage_var->SetResource(g_pSRView[FULLSIZE]);
	pLuminanceTable_var->SetFloatVector( &vLuminanceTable[0] );

	pShader->GetPassByIndex(0)->Apply(0);

	DrawFullScreenQuad();
}

static void Average(void)
{
	float fTexX, fTexY;
	Vector4 vOffset;
	Vector4 vTexOffset_256x256[16];
	Vector4 vTexOffset_64x64[16];
	Vector4 vTexOffset_16x16[16];
	Vector4 vTexOffset_4x4[16];
	int index = 0;

	index=0;
	fTexX = 1.0f/256.0f;
	fTexY = 1.0f/256.0f;
	vOffset.Set(-0.5f/64.0f, -0.5f/64.0f, 0.0f, 0.0f);

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_256x256[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			vTexOffset_256x256[index] += vOffset;
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/64.0f;
	fTexY = 1.0f/64.0f;
	vOffset.Set(-0.5f/16.0f, -0.5f/16.0f, 0.0f, 0.0f);

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_64x64[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			vTexOffset_64x64[index] += vOffset;
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/16.0f;
	fTexY = 1.0f/16.0f;
	vOffset.Set(-0.5f/4.0f, -0.5f/4.0f, 0.0f, 0.0f);

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_16x16[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			vTexOffset_16x16[index] += vOffset;
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/4.0f;
	fTexY = 1.0f/4.0f;
	vOffset.Set(-0.5f, -0.5f, 0.0f, 0.0f);

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_4x4[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			vTexOffset_4x4[index] += vOffset;
			index++;
		}
	}


	ID3D10EffectTechnique *pShader = g_pExposureFX->GetTechniqueByName("Average16Samples");
	ID3D10EffectShaderResourceVariable *pImage_var = g_pExposureFX->GetVariableByName("Image0")->AsShaderResource();
	ID3D10EffectVectorVariable *pTexOffset_var = g_pExposureFX->GetVariableByName("vTexOffset")->AsVector();

	// pShader->GetPassByIndex(0)->Apply(0);

	// 256x256 -> 64x64
	{
		g_pDevice->OMSetRenderTargets(1, &g_pRTView[DOWNSAMPLED_64x64], NULL);
		SetViewport(DOWNSAMPLED_64x64);

		pImage_var->SetResource(g_pSRView[DOWNSAMPLED_256x256]);
		pTexOffset_var->SetFloatVectorArray( (float *)vTexOffset_256x256, 0, 16);

		pShader->GetPassByIndex(0)->Apply(0);
		DrawFullScreenQuad();
	}

	// 64x64 -> 16x16
	{
		g_pDevice->OMSetRenderTargets(1, &g_pRTView[DOWNSAMPLED_16x16], NULL);
		SetViewport(DOWNSAMPLED_16x16);

		pImage_var->SetResource(g_pSRView[DOWNSAMPLED_64x64]);
		pTexOffset_var->SetFloatVectorArray( (float *)vTexOffset_64x64, 0, 16);

		pShader->GetPassByIndex(0)->Apply(0);
		DrawFullScreenQuad();
	}

	// 16x16 -> 4x4
	{
		g_pDevice->OMSetRenderTargets(1, &g_pRTView[DOWNSAMPLED_4x4], NULL);
		SetViewport(DOWNSAMPLED_4x4);

		pImage_var->SetResource(g_pSRView[DOWNSAMPLED_16x16]);
		pTexOffset_var->SetFloatVectorArray( (float *)vTexOffset_16x16, 0, 16);

		pShader->GetPassByIndex(0)->Apply(0);
		DrawFullScreenQuad();
	}

	// 4x4 -> 1x1
	{
		g_pDevice->OMSetRenderTargets(1, &g_pRTView[LUMINANCE_CURRENT], NULL);
		SetViewport(LUMINANCE_CURRENT);

		pImage_var->SetResource(g_pSRView[DOWNSAMPLED_4x4]);
		pTexOffset_var->SetFloatVectorArray( (float *)vTexOffset_4x4, 0, 16);

		pShader->GetPassByIndex(0)->Apply(0);
		DrawFullScreenQuad();
	}

	static int count = 0;
	count++;
}

static void ExpLuminance(void)
{
	ID3D10EffectTechnique *pShader = g_pExposureFX->GetTechniqueByName("ExpLuminance");
	ID3D10EffectShaderResourceVariable *pImage0_var = g_pExposureFX->GetVariableByName("Image0")->AsShaderResource();

	g_pDevice->OMSetRenderTargets(1, &g_pRTView[LUMINANCE_TEMP], NULL);

	pImage0_var->SetResource(g_pSRView[LUMINANCE_CURRENT]);
	pShader->GetPassByIndex(0)->Apply(0);

	DrawFullScreenQuad();

	SwapRenderTarget(LUMINANCE_CURRENT, LUMINANCE_TEMP);
}

static void AdaptiveLuminance(void)
{
	ID3D10EffectTechnique *pShader = g_pExposureFX->GetTechniqueByName("AdaptiveLuminance");
	ID3D10EffectShaderResourceVariable *pImage0_var = g_pExposureFX->GetVariableByName("Image0")->AsShaderResource();
	ID3D10EffectShaderResourceVariable *pImage1_var = g_pExposureFX->GetVariableByName("Image1")->AsShaderResource();
	ID3D10EffectVectorVariable *pAdaptiveSpeed_var = g_pExposureFX->GetVariableByName("vAdaptiveSpeed")->AsVector();

	Vector4 vSpeed(0.1f);

	pImage0_var->SetResource(g_pSRView[LUMINANCE_PREVIOUS]);
	pImage1_var->SetResource(g_pSRView[LUMINANCE_CURRENT]);
	pAdaptiveSpeed_var->SetFloatVector( (float*)&vSpeed[0] );

	g_pDevice->OMSetRenderTargets(1, &g_pRTView[LUMINANCE_TEMP], NULL);

	pShader->GetPassByIndex(0)->Apply(0);
	DrawFullScreenQuad();

	SwapRenderTarget(LUMINANCE_TEMP, LUMINANCE_PREVIOUS);
}

static void AutoExposure(void)
{
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();

	ID3D10EffectTechnique *pShader = g_pExposureFX->GetTechniqueByName("AutoExposure");
	ID3D10EffectShaderResourceVariable *pImage0_var = g_pExposureFX->GetVariableByName("Image0")->AsShaderResource();
	ID3D10EffectShaderResourceVariable *pImage1_var = g_pExposureFX->GetVariableByName("Image1")->AsShaderResource();
	ID3D10EffectVectorVariable *pMulClamp_var = g_pExposureFX->GetVariableByName("vMultiplierClamp")->AsVector();
	ID3D10EffectVectorVariable *pMiddle_var = g_pExposureFX->GetVariableByName("vMiddleGray")->AsVector();

	Vector4 vMiddleGray(0.5f);
	Vector4 vMultiplierClamp(0.2f, 3.0f, 0.0f, 0.0f);

	pImage0_var->SetResource(g_pSRView[FULLSIZE]);
	pImage1_var->SetResource(g_pSRView[LUMINANCE_PREVIOUS]);
	pMulClamp_var->SetFloatVector( (float*)&vMultiplierClamp[0] );
	pMiddle_var->SetFloatVector( (float*)&vMiddleGray[0] );

	g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, NULL);
	SetViewport(FULLSIZE);

	pShader->GetPassByIndex(0)->Apply(0);
	DrawFullScreenQuad();
}

void RenderFrameDX10(void)
{
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	UINT stride = sizeof(Vertex_VT);
	UINT offset = 0;

	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView();

	g_pDevice->OMSetRenderTargets(1, &g_pRTView[FULLSIZE], pDepthStencilView);
	g_pDevice->ClearRenderTargetView(g_pRTView[FULLSIZE], &vClearColor[0]);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);

	SetViewport(FULLSIZE);

	CGutModel_DX10::SetViewMatrix(view_matrix);
	CGutModel_DX10::SetWorldMatrix(world_matrix);
	CGutModel_DX10::SetProjectionMatrix(g_proj_matrix);
	CGutModel_DX10::UpdateMatrix();

	g_Model_DX10.Render();

	ConvertToLuminance();
	Average();

	AdaptiveLuminance();
	AutoExposure();

	if ( g_iMode & 0x01 )
	{
		g_pDevice->OMSetRenderTargets(1, &pRenderTargetView, pDepthStencilView);
		SetViewport(FULLSIZE);

		float x = -1.0f;
		float y = -1.0f;
		float w = 0.2f;
		float h = 0.2f;

		DrawImage(g_pSRView[LUMINANCE_CURRENT], x, y, w, h);
		x+=w;
		DrawImage(g_pSRView[LUMINANCE_PREVIOUS], x, y, w, h);
		x+=w;
	}

	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain();
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_