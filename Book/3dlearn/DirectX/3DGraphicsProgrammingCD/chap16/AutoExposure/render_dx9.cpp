#include "render_data.h"

#include "Gut.h"
#include "GutDX9.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"
#include "GutModel_DX9.h"

LPDIRECT3DTEXTURE9 g_pTexture = NULL;

LPD3DXEFFECT g_pEffect = NULL;
D3DXHANDLE g_pAddImageShader = NULL;
D3DXHANDLE g_pBlurShader = NULL;
D3DXHANDLE g_pBrightnessShader = NULL;

LPD3DXEFFECT g_pExposureEffect = NULL;
LPD3DXEFFECT g_pLuminanceShader = NULL;
LPD3DXEFFECT g_pAverageShade = NULL;
LPD3DXEFFECT g_pAutoExposure = NULL;
LPD3DXEFFECT g_pToneMapShader = NULL;

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

LPDIRECT3DTEXTURE9 g_pFrameBuffer[NUM_FRAMEBUFFERS];
LPDIRECT3DSURFACE9 g_pFrameSurface[NUM_FRAMEBUFFERS];

LPDIRECT3DSURFACE9 g_pMainSurface = NULL;
LPDIRECT3DSURFACE9 g_pDepthStencilSurface = NULL;

static sImageInfo g_ImageInfo;
static sImageInfo g_Image256x256(256,256,true);
static sImageInfo g_Image64x64(64,64,true);
static sImageInfo g_Image16x16(16,16,true);
static sImageInfo g_Image4x4(4,4,true);
static sImageInfo g_Image1x1(1,1,true);

static Matrix4x4 g_proj_matrix;

static CGutModel_DX9 g_Model_DX9;

static void SwapRenderTarget(int ra, int rb)
{
	LPDIRECT3DTEXTURE9 texture = g_pFrameBuffer[ra];
	LPDIRECT3DSURFACE9 surface = g_pFrameSurface[ra];

	g_pFrameBuffer[ra] = g_pFrameBuffer[rb];
	g_pFrameSurface[ra] = g_pFrameSurface[rb];

	g_pFrameBuffer[rb] = texture;
	g_pFrameSurface[rb] = surface;
}

bool ReInitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	int w, h;
	GutGetWindowSize(w, h);
	
	g_ImageInfo.m_iWidth = w;
	g_ImageInfo.m_iHeight = h;
	
	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNearZ, g_fFarZ);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&g_proj_matrix);

	int width = w;
	int height = h;
	
	D3DFORMAT fmt = D3DFMT_A16B16G16R16F;

	device->CreateTexture(width, height, 1, 
		D3DUSAGE_RENDERTARGET, fmt, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[FULLSIZE], NULL);

	device->CreateTexture(256, 256, 1, 
		D3DUSAGE_RENDERTARGET, fmt, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[DOWNSAMPLED_256x256], NULL);

	device->CreateTexture(64, 64, 1, 
		D3DUSAGE_RENDERTARGET, fmt, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[DOWNSAMPLED_64x64], NULL);

	device->CreateTexture(16, 16, 1, 
		D3DUSAGE_RENDERTARGET, fmt, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[DOWNSAMPLED_16x16], NULL);

	device->CreateTexture(4, 4, 1, 
		D3DUSAGE_RENDERTARGET, fmt, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[DOWNSAMPLED_4x4], NULL);

	device->CreateTexture(1, 1, 1, 
		D3DUSAGE_RENDERTARGET, fmt, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[LUMINANCE_PREVIOUS], NULL);

	device->CreateTexture(1, 1, 1, 
		D3DUSAGE_RENDERTARGET, fmt, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[LUMINANCE_CURRENT], NULL);

	device->CreateTexture(1, 1, 1, 
		D3DUSAGE_RENDERTARGET, fmt, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[LUMINANCE_TEMP], NULL);

	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		if ( g_pFrameBuffer[i]==NULL )
			return false;
		g_pFrameBuffer[i]->GetSurfaceLevel(0, &g_pFrameSurface[i]);
	}

	for ( int i=0; i<4; i++ )
	{
		device->SetSamplerState(i,  D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(i,  D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(i,  D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		device->SetSamplerState(i,  D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(i,  D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}

	device->GetRenderTarget(0, &g_pMainSurface); g_pMainSurface->Release();
	device->GetDepthStencilSurface(&g_pDepthStencilSurface); g_pDepthStencilSurface->Release();

	return true;
}

bool InitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		g_pFrameBuffer[i] = NULL;
		g_pFrameSurface[i] = NULL;
	}

	CGutModel::SetTexturePath("../../textures/");

	if ( !g_Model_DX9.ConvertToDX9Model(&g_Model) )
		return false;

	g_pEffect = GutLoadFXShaderDX9("../../shaders/Posteffect.fx");
	if ( NULL==g_pEffect )
		return false;

	g_pAddImageShader = g_pEffect->GetTechniqueByName("DrawImage");
	g_pBlurShader = g_pEffect->GetTechniqueByName("BlurImage");
	g_pBrightnessShader = g_pEffect->GetTechniqueByName("Brightness");

	if ( NULL==g_pAddImageShader || NULL==g_pBlurShader || 
		 NULL==g_pBrightnessShader )
		return false;

	g_pExposureEffect = GutLoadFXShaderDX9("../../shaders/Exposure.fx");
	if ( NULL==g_pExposureEffect )
		return false;

	if ( !ReInitResourceDX9() )
		return false;

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pExposureEffect);

	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		SAFE_RELEASE(g_pFrameSurface[i]);
		SAFE_RELEASE(g_pFrameBuffer[i]);
	}

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	for ( int i=0; i<NUM_FRAMEBUFFERS; i++ )
	{
		SAFE_RELEASE(g_pFrameSurface[i]);
		SAFE_RELEASE(g_pFrameBuffer[i]);
	}

	// Reset Device
	GutResetGraphicsDeviceDX9();
	// 取得Direct3D9裝置
	ReInitResourceDX9();
}

void DrawScreenQuad(sImageInfo *pInfo, float x0, float y0, float width, float height)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

	Vertex_VT quad[4];
	memcpy(quad, g_FullScreenQuad, sizeof(quad));

	quad[0].m_Position[0] = x0;
	quad[0].m_Position[1] = y0;

	quad[1].m_Position[0] = x0 + width;
	quad[1].m_Position[1] = y0;

	quad[2].m_Position[0] = x0;
	quad[2].m_Position[1] = y0 + height;

	quad[3].m_Position[0] = x0 + width;
	quad[3].m_Position[1] = y0 + height;

	for ( int i=0; i<4; i++ )
	{
		quad[i].m_Texcoord[0] += fTexelW*0.5f;
		quad[i].m_Texcoord[1] += fTexelH*0.5f;
	}

	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex_VT));
}

void DrawFullScreenQuad(sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

	Vertex_VT quad[4];
	memcpy(quad, g_FullScreenQuad, sizeof(quad));

	for ( int i=0; i<4; i++ )
	{
		quad[i].m_Texcoord[0] += fTexelW*0.5f;
		quad[i].m_Texcoord[1] += fTexelH*0.5f;
	}

	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex_VT));
}

void AutoExposure(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DXHANDLE shader = g_pExposureEffect->GetTechniqueByName("AutoExposure");
	g_pExposureEffect->SetTechnique(shader);

	Vector4 vMiddleGray(0.5f);
	Vector4 vMultiplierClamp(0.2f, 3.0f, 0.0f, 0.0f);

	D3DXHANDLE middlegray_var = g_pExposureEffect->GetParameterByName(NULL, "vMiddleGray");
	D3DXHANDLE clamp_var = g_pExposureEffect->GetParameterByName(NULL, "vMultiplierClamp");
	D3DXHANDLE image0_var = g_pExposureEffect->GetParameterByName(NULL, "Image");
	D3DXHANDLE image1_var = g_pExposureEffect->GetParameterByName(NULL, "Image2");

	g_pExposureEffect->SetVector(middlegray_var, (D3DXVECTOR4*)&vMiddleGray);
	g_pExposureEffect->SetVector(clamp_var, (D3DXVECTOR4*)&vMultiplierClamp);

	g_pExposureEffect->SetTexture(image0_var, g_pFrameBuffer[FULLSIZE]);
	g_pExposureEffect->SetTexture(image1_var, g_pFrameBuffer[LUMINANCE_TEMP]);

	g_pExposureEffect->Begin(NULL, 0);
	g_pExposureEffect->BeginPass(0);

	DrawFullScreenQuad(&g_ImageInfo);

	g_pExposureEffect->EndPass();
	g_pExposureEffect->End();
}

void AdaptiveLuminance(void)
{
	static int count = 0;
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	if ( count )
	{
		device->SetRenderTarget(0, g_pFrameSurface[LUMINANCE_TEMP]);

		D3DXHANDLE shader = g_pExposureEffect->GetTechniqueByName("AdaptiveLuminance");
		g_pExposureEffect->SetTechnique(shader);

		Vector4 vSpeed(0.03f);

		D3DXHANDLE speed_var = g_pExposureEffect->GetParameterByName(NULL, "vAdaptiveSpeed");
		D3DXHANDLE image0_var = g_pExposureEffect->GetParameterByName(NULL, "Image");
		D3DXHANDLE image1_var = g_pExposureEffect->GetParameterByName(NULL, "Image2");

		g_pExposureEffect->SetVector(speed_var, (D3DXVECTOR4*)&vSpeed);
		g_pExposureEffect->SetTexture(image0_var, g_pFrameBuffer[LUMINANCE_PREVIOUS]);
		g_pExposureEffect->SetTexture(image1_var, g_pFrameBuffer[LUMINANCE_CURRENT]);

		g_pExposureEffect->Begin(NULL, 0);
		g_pExposureEffect->BeginPass(0);

		DrawFullScreenQuad(&g_Image1x1);

		g_pExposureEffect->EndPass();
		g_pExposureEffect->End();
	}
	else
	{
		device->StretchRect(g_pFrameSurface[LUMINANCE_CURRENT], NULL, g_pFrameSurface[LUMINANCE_TEMP], NULL, D3DTEXF_POINT);
	}
	// copy
	//device->StretchRect(g_pFrameSurface[LUMINANCE_TEMP], NULL, g_pFrameSurface[LUMINANCE_PREVIOUS], NULL, D3DTEXF_POINT);
	SwapRenderTarget(LUMINANCE_TEMP, LUMINANCE_PREVIOUS);
	count++;
}

void AverageLuminance(LPDIRECT3DTEXTURE9 pSource)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DXHANDLE shader = g_pExposureEffect->GetTechniqueByName("Average16Samples");
	g_pExposureEffect->SetTechnique(shader);

	D3DXHANDLE texoffset_var = g_pExposureEffect->GetParameterByName(NULL, "vTexOffset");
	D3DXHANDLE image_var = g_pExposureEffect->GetParameterByName(NULL, "Image");

	float fTexX, fTexY;
	Vector4 vTexOffset_256x256[16];
	Vector4 vTexOffset_64x64[16];
	Vector4 vTexOffset_16x16[16];
	Vector4 vTexOffset_4x4[16];
	int index = 0;

	index=0;
	fTexX = 1.0f/256.0f;
	fTexY = 1.0f/256.0f;

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_256x256[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/64.0f;
	fTexY = 1.0f/64.0f;
	
	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_64x64[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/16.0f;
	fTexY = 1.0f/16.0f;

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_16x16[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			index++;
		}
	}

	index=0;
	fTexX = 1.0f/4.0f;
	fTexY = 1.0f/4.0f;

	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++ )
		{
			vTexOffset_4x4[index].Set(x*fTexX, y*fTexY, 0.0f, 0.0f);
			index++;
		}
	}

	// 256x256 -> 64x164
	{
		device->SetRenderTarget(0, g_pFrameSurface[DOWNSAMPLED_64x64]);
		device->SetDepthStencilSurface(NULL);

		g_pExposureEffect->SetVectorArray(texoffset_var, (D3DXVECTOR4*)vTexOffset_256x256, 16);
		g_pExposureEffect->SetTexture(image_var, pSource);

		g_pExposureEffect->Begin(NULL, 0);
		g_pExposureEffect->BeginPass(0);

		DrawFullScreenQuad(&g_Image64x64);

		g_pExposureEffect->EndPass();
		g_pExposureEffect->End();
	}
	// 64x64 -> 16x16
	{
		device->SetRenderTarget(0, g_pFrameSurface[DOWNSAMPLED_16x16]);
		device->SetDepthStencilSurface(NULL);

		g_pExposureEffect->SetVectorArray(texoffset_var, (D3DXVECTOR4*)vTexOffset_64x64, 16);
		g_pExposureEffect->SetTexture(image_var, pSource);

		g_pExposureEffect->Begin(NULL, 0);
		g_pExposureEffect->BeginPass(0);

		DrawFullScreenQuad(&g_Image64x64);

		g_pExposureEffect->EndPass();
		g_pExposureEffect->End();
	}
	// 16x16 -> 4x4
	{
		device->SetRenderTarget(0, g_pFrameSurface[DOWNSAMPLED_4x4]);

		g_pExposureEffect->SetVectorArray(texoffset_var, (D3DXVECTOR4*)vTexOffset_16x16, 16);
		g_pExposureEffect->SetTexture(image_var, g_pFrameBuffer[DOWNSAMPLED_16x16]);

		g_pExposureEffect->Begin(NULL, 0);
		g_pExposureEffect->BeginPass(0);

		DrawFullScreenQuad(&g_Image16x16);

		g_pExposureEffect->EndPass();
		g_pExposureEffect->End();

	}
	// 4x4 -> 1x1
	{
		device->SetRenderTarget(0, g_pFrameSurface[LUMINANCE_CURRENT]);

		g_pExposureEffect->SetVectorArray(texoffset_var, (D3DXVECTOR4*)vTexOffset_4x4, 16);
		g_pExposureEffect->SetTexture(image_var, g_pFrameBuffer[DOWNSAMPLED_4x4]);

		g_pExposureEffect->Begin(NULL, 0);
		g_pExposureEffect->BeginPass(0);

		DrawFullScreenQuad(&g_Image4x4);

		g_pExposureEffect->EndPass();
		g_pExposureEffect->End();
	}
}

/*
void ExpLuminance(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->StretchRect(g_pFrameSurface[LUMINANCE_CURRENT], NULL, g_pFrameSurface[LUMINANCE_TEMP], NULL, D3DTEXF_POINT);

	device->SetRenderTarget(0, g_pFrameSurface[LUMINANCE_CURRENT]);

	D3DXHANDLE shader = g_pExposureEffect->GetTechniqueByName("ExpLuminance");
	D3DXHANDLE image_var = g_pExposureEffect->GetParameterByName(NULL, "Image");
	
	g_pExposureEffect->SetTechnique(shader);
	g_pExposureEffect->SetTexture(image_var, g_pFrameBuffer[LUMINANCE_TEMP]);

	g_pExposureEffect->Begin(NULL, 0);
	g_pExposureEffect->BeginPass(0);

	DrawFullScreenQuad(&g_Image1x1);

	g_pExposureEffect->EndPass();
	g_pExposureEffect->End();
}
*/

void ExpLuminance(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderTarget(0, g_pFrameSurface[LUMINANCE_TEMP]);

	D3DXHANDLE shader = g_pExposureEffect->GetTechniqueByName("ExpLuminance");
	D3DXHANDLE image_var = g_pExposureEffect->GetParameterByName(NULL, "Image");
	
	g_pExposureEffect->SetTechnique(shader);
	g_pExposureEffect->SetTexture(image_var, g_pFrameBuffer[LUMINANCE_CURRENT]);

	g_pExposureEffect->Begin(NULL, 0);
	g_pExposureEffect->BeginPass(0);

	DrawFullScreenQuad(&g_Image1x1);

	g_pExposureEffect->EndPass();
	g_pExposureEffect->End();

	SwapRenderTarget(LUMINANCE_TEMP, LUMINANCE_CURRENT);
}

void ExpLuminance(LPDIRECT3DSURFACE9 pSurace, LPDIRECT3DTEXTURE9 pTexture, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderTarget(0, pSurace);

	D3DXHANDLE shader = g_pExposureEffect->GetTechniqueByName("ExpLuminance");
	D3DXHANDLE image_var = g_pExposureEffect->GetParameterByName(NULL, "Image");

	g_pExposureEffect->SetTechnique(shader);
	g_pExposureEffect->SetTexture(image_var, pTexture);

	g_pExposureEffect->Begin(NULL, 0);
	g_pExposureEffect->BeginPass(0);

	DrawFullScreenQuad(pInfo);

	g_pExposureEffect->EndPass();
	g_pExposureEffect->End();
}

void ConverToLuminance(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	
	device->SetRenderTarget(0, g_pFrameSurface[DOWNSAMPLED_256x256]);
	device->SetDepthStencilSurface(NULL);

	D3DXHANDLE shader = g_pExposureEffect->GetTechniqueByName("Luminance");
	g_pExposureEffect->SetTechnique(shader);

	D3DXHANDLE tablevar = g_pExposureEffect->GetParameterByName(NULL, "vLuminanceTable");
	D3DXHANDLE imagevar = g_pExposureEffect->GetParameterByName(NULL, "Image");

	Vector4 vTable(0.21f, 0.71f, 0.072f);
	g_pExposureEffect->SetVector(tablevar, (D3DXVECTOR4*)&vTable);
	g_pExposureEffect->SetTexture(imagevar, pSource);

	g_pExposureEffect->Begin(NULL, 0);
	g_pExposureEffect->BeginPass(0);

	DrawFullScreenQuad(pInfo);

	g_pExposureEffect->EndPass();
	g_pExposureEffect->End();
}

void ConverToLogLuminance(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderTarget(0, g_pFrameSurface[DOWNSAMPLED_256x256]);
	device->SetDepthStencilSurface(NULL);

	D3DXHANDLE shader = g_pExposureEffect->GetTechniqueByName("LogLuminance");
	g_pExposureEffect->SetTechnique(shader);

	D3DXHANDLE tablevar = g_pExposureEffect->GetParameterByName(NULL, "vLuminanceTable");
	D3DXHANDLE imagevar = g_pExposureEffect->GetParameterByName(NULL, "Image");

	Vector4 vTable(0.21f, 0.71f, 0.072f);
	g_pExposureEffect->SetVector(tablevar, (D3DXVECTOR4*)&vTable);
	g_pExposureEffect->SetTexture(imagevar, pSource);

	g_pExposureEffect->Begin(NULL, 0);
	g_pExposureEffect->BeginPass(0);

	DrawFullScreenQuad(pInfo);

	g_pExposureEffect->EndPass();
	g_pExposureEffect->End();
}

void DrawImage(LPDIRECT3DTEXTURE9 pSource1, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	g_pEffect->SetTechnique(g_pAddImageShader);
	D3DXHANDLE pTexture1Var = g_pEffect->GetParameterByName(NULL, "Image");
	
	g_pEffect->SetTexture(pTexture1Var, pSource1);

	g_pEffect->Begin(NULL, 0);
	g_pEffect->BeginPass(0);

	DrawFullScreenQuad(pInfo);
	
	g_pEffect->EndPass();
	g_pEffect->End();
}

void DrawImage(LPDIRECT3DTEXTURE9 pSource1, sImageInfo *pInfo, float x0, float y0, float width, float height)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	g_pEffect->SetTechnique(g_pAddImageShader);
	D3DXHANDLE pTexture1Var = g_pEffect->GetParameterByName(NULL, "Image");

	g_pEffect->SetTexture(pTexture1Var, pSource1);

	g_pEffect->Begin(NULL, 0);
	g_pEffect->BeginPass(0);

	DrawScreenQuad(pInfo, x0, y0, width, height);

	g_pEffect->EndPass();
	g_pEffect->End();
}

void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->BeginScene(); 

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	// normal pass
	if ( g_bPosteffect )
	{
		device->SetRenderTarget(0, g_pFrameSurface[FULLSIZE]);
		device->SetDepthStencilSurface(NULL);
	}
	else
	{
		device->SetRenderTarget(0, g_pMainSurface);
		device->SetDepthStencilSurface(NULL);
	}

	device->Clear(0, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0);

	device->SetVertexShader(NULL);
	device->SetPixelShader(NULL);

	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&g_proj_matrix);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&world_matrix);

	g_Model_DX9.Render();

	if ( g_bPosteffect )
	{
		ConverToLogLuminance(g_pFrameBuffer[FULLSIZE], &g_ImageInfo);
		// ExpLuminance(g_pFrameSurface[FULLSIZE], g_pFrameBuffer[DOWNSAMPLED_256x256], &g_Image256x256);

		AverageLuminance(g_pFrameBuffer[DOWNSAMPLED_256x256]);
		ExpLuminance();
		AdaptiveLuminance();

		device->SetRenderTarget(0, g_pMainSurface);
		device->SetDepthStencilSurface(NULL);
		//device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.0f, 0);
		
		AutoExposure();

		if ( g_iMode & 0x01 )
		{
			DrawImage(g_pFrameBuffer[LUMINANCE_TEMP], &g_Image1x1, -1.0f, -1.0f, 0.1f, 0.1f);
			DrawImage(g_pFrameBuffer[LUMINANCE_CURRENT], &g_Image1x1, -0.9f, -1.0f, 0.1f, 0.1f);
		}
	}

	device->EndScene();
	device->Present( NULL, NULL, NULL, NULL );
}
