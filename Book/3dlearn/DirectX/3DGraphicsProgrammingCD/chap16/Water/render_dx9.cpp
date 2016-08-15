#include "render_data.h"

#include "Gut.h"
#include "GutImage.h"
#include "GutWin32.h"

#include "GutModel_DX9.h"
#include "GutTexture_DX9.h"

enum
{
	TEX_HEIGHT0 = 0,
	TEX_HEIGHT1,
	TEX_HEIGHT2,
	TEX_NORMAL,
	TEX_TEXTURES
};

LPDIRECT3DTEXTURE9 g_pWaterTexture = NULL;

LPDIRECT3DTEXTURE9 g_pTextures[TEX_TEXTURES];
LPDIRECT3DSURFACE9 g_pSurfaces[TEX_TEXTURES];

LPDIRECT3DSURFACE9 g_pMainFramebuffer = NULL;
LPDIRECT3DSURFACE9 g_pMainDepthbuffer = NULL;

LPD3DXEFFECT g_pWaterEffect = NULL;
LPD3DXEFFECT g_pPostEffect = NULL;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_orient_matrix;

sImageInfo g_ImageInfo;

CGutModel_DX9 g_Model_DX9;

bool ReInitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	GutGetWindowSize(g_iWidth, g_iHeight);

	g_proj_matrix = GutMatrixOrthoRH_DirectX(g_iWidth, g_iHeight, 0.0f, 100.0f);

	int w = g_iWidth;
	int h = g_iHeight;

	g_ImageInfo.m_iWidth = g_iWidth;
	g_ImageInfo.m_iHeight = g_iHeight;
	g_ImageInfo.m_bProcedure = true;

	//D3DFORMAT fmt = D3DFMT_G16R16;
	D3DFORMAT fmt = D3DFMT_A8R8G8B8;

	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	device->CreateTexture(
		w, h, 
		1, D3DUSAGE_RENDERTARGET, 
		fmt, D3DPOOL_DEFAULT, 
		&g_pTextures[TEX_HEIGHT0], NULL);
	if ( NULL==g_pTextures[TEX_HEIGHT0] ) return false;

	g_pTextures[TEX_HEIGHT0]->GetSurfaceLevel(0, &g_pSurfaces[TEX_HEIGHT0]);
	if ( NULL==g_pSurfaces[TEX_HEIGHT0] ) return false;

	device->CreateTexture(
		w, h, 
		1, D3DUSAGE_RENDERTARGET, 
		fmt, D3DPOOL_DEFAULT, 
		&g_pTextures[TEX_HEIGHT1], NULL);
	if ( NULL==g_pTextures[TEX_HEIGHT1] ) return false;

	if ( NULL==g_pTextures[TEX_HEIGHT1] ) return false;
	g_pTextures[TEX_HEIGHT1]->GetSurfaceLevel(0, &g_pSurfaces[TEX_HEIGHT1]);

	device->CreateTexture(
		w, h, 
		1, D3DUSAGE_RENDERTARGET, 
		fmt, D3DPOOL_DEFAULT, 
		&g_pTextures[TEX_HEIGHT2], NULL);
	if ( NULL==g_pTextures[TEX_HEIGHT2] ) return false;

	g_pTextures[TEX_HEIGHT2]->GetSurfaceLevel(0, &g_pSurfaces[TEX_HEIGHT2]);
	if ( NULL==g_pSurfaces[TEX_HEIGHT2] ) return false;

	device->CreateTexture(
		w, h, 
		1, D3DUSAGE_RENDERTARGET, 
		D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, 
		&g_pTextures[TEX_NORMAL], NULL);

	if ( NULL==g_pTextures[TEX_NORMAL] ) return false;

	g_pTextures[TEX_NORMAL]->GetSurfaceLevel(0, &g_pSurfaces[TEX_NORMAL]);

	if ( NULL==g_pSurfaces[TEX_NORMAL] ) return false;

	device->GetRenderTarget(0, &g_pMainFramebuffer);
	device->GetDepthStencilSurface(&g_pMainDepthbuffer);

	device->SetRenderTarget(0, g_pSurfaces[0]);
	device->SetRenderTarget(1, g_pSurfaces[1]);
	device->SetRenderTarget(2, g_pSurfaces[2]);
	
	device->Clear(0, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0);

	device->SetRenderTarget(0, g_pMainFramebuffer);
	device->SetRenderTarget(1, NULL);
	device->SetRenderTarget(2, NULL);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);

	return true;
}

bool InitResourceDX9(void)
{
	g_orient_matrix.Identity();

	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	if ( !ReInitResourceDX9() )
		return false;

	g_pPostEffect = GutLoadFXShaderDX9("../../shaders/Posteffect.fx");
	if ( NULL==g_pPostEffect )
		return false;

	g_pWaterEffect = GutLoadFXShaderDX9("../../shaders/Watereffect.fx");
	if ( NULL==g_pWaterEffect )
		return false;

	g_pWaterTexture = GutLoadTexture_DX9("../../textures/lena.tga");
	if ( NULL==g_pWaterTexture )
		return false;

	g_Model_DX9.ConvertToDX9Model(&g_Model);

	return true;
}

bool ReleaseResourceDX9(void)
{
	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		SAFE_RELEASE(g_pSurfaces[i]);
		SAFE_RELEASE(g_pTextures[i]);
	}

	SAFE_RELEASE(g_pWaterTexture);

	SAFE_RELEASE(g_pMainFramebuffer);
	SAFE_RELEASE(g_pMainDepthbuffer);

	SAFE_RELEASE(g_pWaterEffect);
	SAFE_RELEASE(g_pPostEffect);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		SAFE_RELEASE(g_pSurfaces[i]);
		SAFE_RELEASE(g_pTextures[i]);
	}

	SAFE_RELEASE(g_pMainFramebuffer);
	SAFE_RELEASE(g_pMainDepthbuffer);

	GutResetGraphicsDeviceDX9();

	ReInitResourceDX9();
}

void DrawImage(LPDIRECT3DTEXTURE9 pSource, float x0, float y0, float w, float h, sImageInfo *pInfo, bool bRedOnly = false)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DXHANDLE shader = g_pPostEffect->GetTechniqueByName(bRedOnly ? "DrawImageRedChannel" : "DrawImage");
	D3DXHANDLE pTextureVar = g_pPostEffect->GetParameterByName(NULL, "Image");

	g_pPostEffect->SetTechnique(shader);
	g_pPostEffect->SetTexture(pTextureVar, pSource);

	// 設定頂點資料格式	
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);

	g_pPostEffect->Begin(NULL, 0);
	g_pPostEffect->BeginPass(0);

	GutDrawScreenQuad_DX9(x0, y0, w, h, pInfo);

	g_pPostEffect->EndPass();
	g_pPostEffect->End();
}

void DrawImage(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo, bool bRedOnly = false)
{
	DrawImage(pSource, -1.0f, -1.0f, 2.0f, 2.0f, pInfo, bRedOnly);
}

static void AddImpulse(void)
{
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
	Vector4 vVec2_new = Vector3CrossProduct(vVec0_new, vVec1_new);

	g_orient_matrix.Identity();
	g_orient_matrix[0] = vVec0_new;
	g_orient_matrix[1] = vVec1_new;
	g_orient_matrix[2] = vVec2_new;

	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderTarget(0, g_pSurfaces[TEX_HEIGHT1]);
	device->SetDepthStencilSurface(NULL);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	
	Matrix4x4 world_matrix; 
	world_matrix.Scale_Replace(g_fRippleSize, g_fRippleSize, 1.0f);
	world_matrix[3] = g_vPosition;

	Matrix4x4 wvp_matrix = g_orient_matrix * world_matrix * view_matrix * g_proj_matrix;

	D3DXHANDLE shader = g_pWaterEffect->GetTechniqueByName("AddImpulse");
	D3DXHANDLE wvp_matrix_var = g_pWaterEffect->GetParameterByName(NULL, "wvp_matrix");
	D3DXHANDLE force_var = g_pWaterEffect->GetParameterByName(NULL, "fForce");

	g_pWaterEffect->SetTechnique(shader);
	g_pWaterEffect->SetMatrix(wvp_matrix_var, (D3DXMATRIX *)&wvp_matrix);
	g_pWaterEffect->SetFloat(force_var, 0.05f);

	g_pWaterEffect->Begin(NULL, 0);
	g_pWaterEffect->BeginPass(0);
		g_Model_DX9.Render(0);
	g_pWaterEffect->EndPass();
	g_pWaterEffect->End();

	vPosition = g_vPosition;
}

static void WaterSimulation(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderTarget(0, g_pSurfaces[TEX_HEIGHT2]);
	device->SetDepthStencilSurface(NULL);

	D3DXHANDLE shader = g_pWaterEffect->GetTechniqueByName("WaterSimulation");
	D3DXHANDLE heightmap_current_var = g_pWaterEffect->GetParameterByName(NULL, "heightmap_current");
	D3DXHANDLE heightmap_prev_var = g_pWaterEffect->GetParameterByName(NULL, "heightmap_prev");
	D3DXHANDLE texturesize_var = g_pWaterEffect->GetParameterByName(NULL, "texture_size");
	D3DXHANDLE damping_var = g_pWaterEffect->GetParameterByName(NULL, "fDamping");
	
	Vector4 texturesize;
	texturesize[0] = 1.0f/(float)g_ImageInfo.m_iWidth;
	texturesize[1] = 1.0f/(float)g_ImageInfo.m_iHeight;
	texturesize[2] = (float)g_ImageInfo.m_iWidth;
	texturesize[3] = (float)g_ImageInfo.m_iHeight;

	g_pWaterEffect->SetTechnique(shader);
	g_pWaterEffect->SetTexture(heightmap_prev_var, g_pTextures[TEX_HEIGHT0]);
	g_pWaterEffect->SetTexture(heightmap_current_var, g_pTextures[TEX_HEIGHT1]);
	g_pWaterEffect->SetFloat(damping_var, 0.99f);
	g_pWaterEffect->SetVector(texturesize_var, (D3DXVECTOR4*)&texturesize);

	g_pWaterEffect->Begin(NULL,0);
	g_pWaterEffect->BeginPass(0);
		GutDrawFullScreenQuad_DX9(&g_ImageInfo);
	g_pWaterEffect->EndPass();
	g_pWaterEffect->End();
}

static void HeightmapToNormal(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderTarget(0, g_pSurfaces[TEX_NORMAL]);
	device->SetDepthStencilSurface(NULL);

	D3DXHANDLE shader = g_pWaterEffect->GetTechniqueByName("ConvertNormal");
	D3DXHANDLE heightmap_current_var = g_pWaterEffect->GetParameterByName(NULL, "heightmap_current");
	D3DXHANDLE normalscale_var = g_pWaterEffect->GetParameterByName(NULL, "fNormalScale");
	
	g_pWaterEffect->SetTechnique(shader);
	g_pWaterEffect->SetTexture(heightmap_current_var, g_pTextures[TEX_HEIGHT2]);
	g_pWaterEffect->SetFloat(normalscale_var, 1.0f);

	g_pWaterEffect->Begin(NULL,0);
	g_pWaterEffect->BeginPass(0);
		GutDrawFullScreenQuad_DX9(&g_ImageInfo);
	g_pWaterEffect->EndPass();
	g_pWaterEffect->End();
}

static void RenderWater(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DXHANDLE shader = g_pWaterEffect->GetTechniqueByName("Water");
	D3DXHANDLE normalmap_var = g_pWaterEffect->GetParameterByName(NULL, "NormalmapTex");
	D3DXHANDLE watermap_var = g_pWaterEffect->GetParameterByName(NULL, "WaterTex");
	D3DXHANDLE texcoordscale_var = g_pWaterEffect->GetParameterByName(NULL, "fTexcoordScale");
	
	g_pWaterEffect->SetTechnique(shader);
	g_pWaterEffect->SetTexture(normalmap_var, g_pTextures[TEX_NORMAL]);
	g_pWaterEffect->SetTexture(watermap_var, g_pWaterTexture);
	g_pWaterEffect->SetFloat(texcoordscale_var, 0.2f);

	g_pWaterEffect->Begin(NULL,0);
	g_pWaterEffect->BeginPass(0);
		GutDrawFullScreenQuad_DX9(&g_ImageInfo);
	g_pWaterEffect->EndPass();
	g_pWaterEffect->End();
}

void DrawObject(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	
	Matrix4x4 scale_matrix; 
	scale_matrix.Scale_Replace(g_fRippleSize, g_fRippleSize, 1.0f);
	
	Matrix4x4 world_matrix = g_orient_matrix * scale_matrix;
	world_matrix[3] = g_vPosition;

	device->SetVertexShader(NULL);
	device->SetPixelShader(NULL);

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*) &g_proj_matrix);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX*) &view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*) &world_matrix);

	device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	g_Model_DX9.Render();

	device->SetRenderState(D3DRS_LIGHTING, FALSE);
}

void SwapHeightmaps(void)
{
	LPDIRECT3DSURFACE9 pSurface = g_pSurfaces[0];
	LPDIRECT3DTEXTURE9 pTexture = g_pTextures[0];

	g_pSurfaces[0] = g_pSurfaces[1];
	g_pTextures[0] = g_pTextures[1];

	g_pSurfaces[1] = g_pSurfaces[2];
	g_pTextures[1] = g_pTextures[2];

	g_pSurfaces[2] = pSurface;
	g_pTextures[2] = pTexture;
}

void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	
	device->BeginScene(); 

	AddImpulse();
	WaterSimulation();
	HeightmapToNormal();

	device->SetRenderTarget(0, g_pMainFramebuffer);
	device->SetDepthStencilSurface(g_pMainDepthbuffer);
	device->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0x0, 1.0f, 0);

	RenderWater();
	DrawObject();

	SwapHeightmaps();

	float x = -1.0f;
	float y = -1.0f;
	float w = 0.4f;
	float h = 0.4f;

	if ( g_iMode & 0x01 )
	{
		DrawImage(g_pTextures[TEX_HEIGHT2], x, y, w, h, &g_ImageInfo);
		x+=w;
	}

	if ( g_iMode & 0x02 )
	{
		D3DXHANDLE shader = g_pWaterEffect->GetTechniqueByName("Heightmap");
		D3DXHANDLE heightmap_var = g_pWaterEffect->GetTechniqueByName("heightmap_current");

		g_pWaterEffect->SetTechnique(shader);
		g_pWaterEffect->SetTexture(heightmap_var, g_pTextures[TEX_HEIGHT2]);

		g_pWaterEffect->Begin(NULL, 0);
		g_pWaterEffect->BeginPass(0);
			GutDrawScreenQuad_DX9(x, y, w, h, &g_ImageInfo);
		g_pWaterEffect->EndPass();
		g_pWaterEffect->End();

		x+=w;
	}

	if ( g_iMode & 0x04 )
	{
		DrawImage(g_pTextures[TEX_NORMAL], x, y, w, h, &g_ImageInfo);
		x+=w;
	}

	device->EndScene(); 
    device->Present( NULL, NULL, NULL, NULL );
}
