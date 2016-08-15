#include "render_data.h"

#include "Gut.h"
#include "GutImage.h"
#include "GutWin32.h"

#include "GutModel_DX9.h"
#include "GutTexture_DX9.h"

enum
{
	TEX_POSITION0 = 0,
	TEX_POSITION1,
	TEX_VELOCITY0,
	TEX_VELOCITY1,
	TEX_TEXTURES
};

LPDIRECT3DTEXTURE9 g_pParticleTexture = NULL;
LPDIRECT3DTEXTURE9 g_pNoiseTexture = NULL;

LPDIRECT3DTEXTURE9 g_pTextures[TEX_TEXTURES];
LPDIRECT3DSURFACE9 g_pSurfaces[TEX_TEXTURES];

LPDIRECT3DSURFACE9 g_pMainFramebuffer = NULL;
LPDIRECT3DSURFACE9 g_pMainDepthbuffer = NULL;

LPD3DXEFFECT g_pParticleFX = NULL;
LPD3DXEFFECT g_pParticleRenderFX = NULL;
LPD3DXEFFECT g_pPostFX = NULL;

static Matrix4x4 g_proj_matrix;

sImageInfo g_ImageInfo;

bool ReInitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	int w,h;
	GutGetWindowSize(w, h);

	float fAspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, fAspect, g_fCameraNearZ, g_fCameraFarZ);

	g_ImageInfo.m_iWidth = g_texture_width;
	g_ImageInfo.m_iHeight = g_texture_height;
	g_ImageInfo.m_bProcedure = true;

	//D3DFORMAT fmt = D3DFMT_A16B16G16R16F;
	D3DFORMAT fmt = D3DFMT_A32B32G32R32F;

	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		device->CreateTexture(
			g_texture_width, g_texture_height, 
			1, D3DUSAGE_RENDERTARGET, 
			fmt, D3DPOOL_DEFAULT, 
			&g_pTextures[i], NULL);

		if ( NULL==g_pTextures[i] ) return false;

		g_pTextures[i]->GetSurfaceLevel(0, &g_pSurfaces[i]);

		if ( NULL==g_pSurfaces[i] ) return false;
	}

	device->GetRenderTarget(0, &g_pMainFramebuffer);
	device->GetDepthStencilSurface(&g_pMainDepthbuffer);

	device->SetRenderTarget(0, g_pSurfaces[0]);
	device->SetRenderTarget(1, g_pSurfaces[1]);
	
	device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 100, 0, 100), 1.0f, 0);

	device->SetRenderTarget(0, g_pMainFramebuffer);
	device->SetRenderTarget(1, NULL);

	return true;
}

bool InitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		g_pTextures[i] = NULL;
		g_pSurfaces[i] = NULL;
	}

	if ( !ReInitResourceDX9() )
		return false;

	g_pParticleFX = GutLoadFXShaderDX9("../../shaders/GPUParticle.fx");
	if ( NULL==g_pParticleFX )
		return false;

	g_pParticleRenderFX = GutLoadFXShaderDX9("../../shaders/GPUParticle_render.fx");
	if ( NULL==g_pParticleRenderFX )
		return false;

	g_pPostFX = GutLoadFXShaderDX9("../../shaders/Posteffect.fx");
	if ( NULL==g_pPostFX )
		return false;

	g_pNoiseTexture = GutLoadTexture_DX9("../../textures/noise_512x512.tga");
	if ( NULL==g_pNoiseTexture )
		return false;

	g_pParticleTexture = GutLoadTexture_DX9("../../textures/particle.tga");
	if ( NULL==g_pParticleTexture )
		return false;

	return true;
}

bool ReleaseResourceDX9(void)
{
	for ( int i=0; i<TEX_TEXTURES; i++ )
	{
		SAFE_RELEASE(g_pSurfaces[i]);
		SAFE_RELEASE(g_pTextures[i]);
	}

	SAFE_RELEASE(g_pMainFramebuffer);
	SAFE_RELEASE(g_pMainDepthbuffer);

	SAFE_RELEASE(g_pParticleFX);
	SAFE_RELEASE(g_pParticleRenderFX);
	SAFE_RELEASE(g_pPostFX);

	SAFE_RELEASE(g_pNoiseTexture);
	SAFE_RELEASE(g_pParticleTexture);

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

void DrawImage(LPDIRECT3DTEXTURE9 pSource, float x0, float y0, float w, float h, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DXHANDLE shader = g_pPostFX->GetTechniqueByName("DrawImage");
	D3DXHANDLE pTextureVar = g_pPostFX->GetParameterByName(NULL, "Image");

	g_pPostFX->SetTechnique(shader);
	g_pPostFX->SetTexture(pTextureVar, pSource);

	// 設定頂點資料格式	
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);

	g_pPostFX->Begin(NULL, 0);
	g_pPostFX->BeginPass(0);

	GutDrawScreenQuad_DX9(x0, y0, w, h, pInfo);

	g_pPostFX->EndPass();
	g_pPostFX->End();
}

void DrawImage(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo)
{
	DrawImage(pSource, -1.0f, -1.0f, 2.0f, 2.0f, pInfo);
}

static void ParticleSimulation(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderTarget(0, g_pSurfaces[TEX_POSITION1]);
	device->SetRenderTarget(1, g_pSurfaces[TEX_VELOCITY1]);
	device->SetDepthStencilSurface(NULL);

	D3DXHANDLE shader = g_pParticleFX->GetTechniqueByName("Simulate");
	D3DXHANDLE positiontex_var = g_pParticleFX->GetParameterByName(NULL, "PositionTex");
	D3DXHANDLE velocitytex_var = g_pParticleFX->GetParameterByName(NULL, "VelocityTex");
	D3DXHANDLE noisetex_var = g_pParticleFX->GetParameterByName(NULL, "NoiseTex");
	D3DXHANDLE force_var = g_pParticleFX->GetParameterByName(NULL, "Force");
	D3DXHANDLE time_var = g_pParticleFX->GetParameterByName(NULL, "fTimeAdvance");
	D3DXHANDLE seed_var = g_pParticleFX->GetParameterByName(NULL, "Seed");
	D3DXHANDLE tan_var = g_pParticleFX->GetParameterByName(NULL, "fTan");
	D3DXHANDLE liferange_var = g_pParticleFX->GetParameterByName(NULL, "LifeRange");
	D3DXHANDLE speedrange_var = g_pParticleFX->GetParameterByName(NULL, "SpeedRange");
	D3DXHANDLE sizerange_var = g_pParticleFX->GetParameterByName(NULL, "SizeRange");

	Vector4 vForce = g_vForce * g_fTimeAdvance;
	Vector4 vSeed;
	Vector4 vLifeRange(1.0f, 2.0f, 0.0f, 0.0f);
	Vector4 vSpeedRange(1.0f, 2.0f, 0.0f, 0.0f);
	Vector4 vSizeRange(0.01f, 0.02f, 0.0f, 0.0f);
	//Vector4 vSizeRange(1.0f, 10.0f, 0.0f, 0.0f);

	vSeed[0] = float(rand()%1024)/1024.0f;
	vSeed[1] = float(rand()%1024)/1024.0f;
	vSeed[2] = float(rand()%1024)/1024.0f;
	vSeed[3] = float(rand()%1024)/1024.0f;

	g_pParticleFX->SetTechnique(shader);
	g_pParticleFX->SetTexture(positiontex_var, g_pTextures[TEX_POSITION0]);
	g_pParticleFX->SetTexture(velocitytex_var, g_pTextures[TEX_VELOCITY0]);
	g_pParticleFX->SetTexture(noisetex_var, g_pNoiseTexture);
	g_pParticleFX->SetVector(force_var, (D3DXVECTOR4*)&vForce);
	g_pParticleFX->SetVector(seed_var, (D3DXVECTOR4*)&vSeed);
	g_pParticleFX->SetVector(liferange_var, (D3DXVECTOR4*)&vLifeRange);
	g_pParticleFX->SetVector(speedrange_var, (D3DXVECTOR4*)&vSpeedRange);
	g_pParticleFX->SetVector(sizerange_var, (D3DXVECTOR4*)&vSizeRange);
	g_pParticleFX->SetFloat(time_var, g_fTimeAdvance);
	g_pParticleFX->SetFloat(tan_var, FastMath::Tan(FastMath::DegToRad(g_fEmitTheta)));

	g_pParticleFX->Begin(NULL,0);
	g_pParticleFX->BeginPass(0);
		GutDrawFullScreenQuad_DX9(&g_ImageInfo);
	g_pParticleFX->EndPass();
	g_pParticleFX->End();

	device->SetRenderTarget(1, NULL);
}

static void RenderParticle(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	int w, h; GutGetWindowSize(w, h);

	Matrix4x4 wvp_matrix = g_Control.GetObjectMatrix() * g_Control.GetViewMatrix() * g_proj_matrix;
	Vector4 ScreenSize( (float)w, (float)h, 0.0f, 0.0f);
	float fTanW = FastMath::Tan( FastMath::DegToRad(g_fFOV) );

	device->SetRenderTarget(0, g_pMainFramebuffer);
	device->SetDepthStencilSurface(g_pMainDepthbuffer);
	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.0f, 0);

	D3DXHANDLE shader = g_pParticleRenderFX->GetTechniqueByName("RenderParticle");
	D3DXHANDLE positiontex_var = g_pParticleRenderFX->GetParameterByName(NULL, "PositionTex");
	D3DXHANDLE velocitytex_var = g_pParticleRenderFX->GetParameterByName(NULL, "VelocityTex");
	D3DXHANDLE wvp_matrix_var = g_pParticleRenderFX->GetParameterByName(NULL, "wvp_matrix");
	D3DXHANDLE tan_fov_var = g_pParticleRenderFX->GetParameterByName(NULL, "fTanW");
	D3DXHANDLE screensize_var = g_pParticleRenderFX->GetParameterByName(NULL, "ScreenSize");
	D3DXHANDLE diffusetex_var = g_pParticleRenderFX->GetParameterByName(NULL, "DiffuseTex");

	g_pParticleRenderFX->SetTechnique(shader);
	g_pParticleRenderFX->SetTexture(positiontex_var, g_pTextures[TEX_POSITION1]);
	g_pParticleRenderFX->SetTexture(velocitytex_var, g_pTextures[TEX_VELOCITY1]);
	g_pParticleRenderFX->SetMatrix(wvp_matrix_var, (D3DXMATRIX*)&wvp_matrix);
	g_pParticleRenderFX->SetVector(screensize_var, (D3DXVECTOR4*)&ScreenSize);
	g_pParticleRenderFX->SetFloat(tan_fov_var, fTanW);
	g_pParticleRenderFX->SetTexture(diffusetex_var, g_pParticleTexture);
	
	device->SetFVF(D3DFVF_TEX1);

	g_pParticleRenderFX->Begin(NULL,0);
	g_pParticleRenderFX->BeginPass(0);
		//device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
		device->DrawPrimitiveUP(D3DPT_POINTLIST, g_num_particles, g_pParticleArray, sizeof(sParticle));
		//device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
	g_pParticleRenderFX->EndPass();
	g_pParticleRenderFX->End();
}

static void SwapTextures(void)
{
	LPDIRECT3DSURFACE9 backup_surface;
	LPDIRECT3DTEXTURE9 backup_texture;

	backup_surface = g_pSurfaces[TEX_POSITION0];
	backup_texture = g_pTextures[TEX_POSITION0];

	g_pSurfaces[TEX_POSITION0] = g_pSurfaces[TEX_POSITION1];
	g_pTextures[TEX_POSITION0] = g_pTextures[TEX_POSITION1];
	g_pSurfaces[TEX_POSITION1] = backup_surface;
	g_pTextures[TEX_POSITION1] = backup_texture;

	backup_surface = g_pSurfaces[TEX_VELOCITY0];
	backup_texture = g_pTextures[TEX_VELOCITY0];

	g_pSurfaces[TEX_VELOCITY0] = g_pSurfaces[TEX_VELOCITY1];
	g_pTextures[TEX_VELOCITY0] = g_pTextures[TEX_VELOCITY1];
	g_pSurfaces[TEX_VELOCITY1] = backup_surface;
	g_pTextures[TEX_VELOCITY1] = backup_texture;
}

void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	
	device->BeginScene(); 

	ParticleSimulation();
	RenderParticle();
	SwapTextures();

	float x = -1.0f;
	float y = -1.0f;
	float w = 0.4f;
	float h = 0.4f;

	device->SetRenderTarget(0, g_pMainFramebuffer);
	device->SetDepthStencilSurface(g_pMainDepthbuffer);

	if ( g_iMode & 0x01 )
	{
		DrawImage(g_pTextures[TEX_POSITION0], x, y, w, h, &g_ImageInfo);
		x+=w;
	}

	if ( g_iMode & 0x02 )
	{
		DrawImage(g_pTextures[TEX_VELOCITY1], x, y, w, h, &g_ImageInfo);
		x+=w;
	}

	device->EndScene(); 
    device->Present( NULL, NULL, NULL, NULL );
}
