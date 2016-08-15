#include "render_data.h"

#include "Gut.h"
#include "GutModel_DX9.h"
#include "GutWin32.h"
#include "GutImage.h"

CGutModel_DX9 g_Model_DX9;
CGutModel_DX9 g_Sphere_DX9;

enum
{
	TEX_POS,
	TEX_NORMAL,
	TEX_DIFFUSE,
	TEX_TEXTURES
};

LPDIRECT3DTEXTURE9 g_pTextures[TEX_TEXTURES];
LPDIRECT3DSURFACE9 g_pSurfaces[TEX_TEXTURES];

LPD3DXEFFECT g_pPrepare = NULL;
LPD3DXEFFECT g_pLighting = NULL;
LPD3DXEFFECT g_pPostEffect = NULL;

LPDIRECT3DSURFACE9 g_pMainFramebuffer = NULL;
LPDIRECT3DSURFACE9 g_pMainDepthbuffer = NULL;

LPDIRECT3DVERTEXDECLARATION9 g_pVertexDecl = NULL;

static Matrix4x4 g_proj_matrix;

sImageInfo g_ImageInfo;

bool ReInitResourceDX9(void)
{
	// ¨ú±oDirect3D 9¸Ë¸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// ³]©wµø¨¤Âà´«¯x°}
	int w, h; GutGetWindowSize(w, h);
	float fAspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, fAspect, 0.1f, 100.0f);

	g_ImageInfo.m_iWidth = w;
	g_ImageInfo.m_iHeight = h;
	g_ImageInfo.m_bProcedure = true;

	D3DFORMAT fmt = D3DFMT_A16B16G16R16F;
	//D3DFORMAT fmt = D3DFMT_A32B32G32R32F;

	for ( int t=0; t<TEX_TEXTURES; t++ )
	{
		device->CreateTexture(
			w, h, 
			1, D3DUSAGE_RENDERTARGET, 
			fmt, D3DPOOL_DEFAULT, 
			&g_pTextures[t], NULL);

		if ( NULL==g_pTextures[t] )
			return false;

		g_pTextures[t]->GetSurfaceLevel(0, &g_pSurfaces[t]);
		if ( NULL==g_pSurfaces[t] )
			return false;
	}

	device->GetRenderTarget(0, &g_pMainFramebuffer);
	device->GetDepthStencilSurface(&g_pMainDepthbuffer);

	return true;
}

bool InitResourceDX9(void)
{
	// ¨ú±oDirect3D 9¸Ë¸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	if ( !ReInitResourceDX9() )
		return false;

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model);
	g_Sphere_DX9.ConvertToDX9Model(&g_Sphere);

	g_pPrepare = GutLoadFXShaderDX9("../../shaders/DeferredLighting_Prepare.fx");
	if ( NULL==g_pPrepare )
		return false;
	g_pLighting = GutLoadFXShaderDX9("../../shaders/DeferredLighting_Final.fx");
	if ( NULL==g_pLighting )
		return false;
	g_pPostEffect = GutLoadFXShaderDX9("../../shaders/Posteffect.fx");
	if ( NULL==g_pPostEffect )
		return false;

	D3DVERTEXELEMENT9 elems[] =
	{
		{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		D3DDECL_END()
	};

	if ( D3D_OK!=device->CreateVertexDeclaration(elems, &g_pVertexDecl) )
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

	SAFE_RELEASE(g_pPrepare);
	SAFE_RELEASE(g_pLighting);
	SAFE_RELEASE(g_pPostEffect);
	
	SAFE_RELEASE(g_pVertexDecl);

	g_Model_DX9.Release();

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

	// Reset Device
	GutResetGraphicsDeviceDX9();
	// ¨ú±oDirect3D 9¸Ë¸m
	ReInitResourceDX9();
}

void DrawScreenQuad(sImageInfo *pInfo, float x0, float y0, float width, float height)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	
	Matrix4x4 ident_mat; ident_mat.Identity();

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&ident_mat);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&ident_mat);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&ident_mat);

	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

	Vertex_V3T2 quad[4];

	quad[0].m_Position[0] = x0;
	quad[0].m_Position[1] = y0;
	quad[0].m_Position[2] = 0.0f;
	quad[0].m_Texcoord[0] = 0.0f + fTexelW*0.5f;
	quad[0].m_Texcoord[1] = 1.0f + fTexelH*0.5f;

	quad[1].m_Position[0] = x0 + width;
	quad[1].m_Position[1] = y0;
	quad[1].m_Position[2] = 0.0f;
	quad[1].m_Texcoord[0] = 1.0f + fTexelW*0.5f;
	quad[1].m_Texcoord[1] = 1.0f + fTexelH*0.5f;

	quad[2].m_Position[0] = x0;
	quad[2].m_Position[1] = y0 + height;
	quad[2].m_Position[2] = 0.0f;
	quad[2].m_Texcoord[0] = 0.0f + fTexelW*0.5f;
	quad[2].m_Texcoord[1] = 0.0f + fTexelH*0.5f;

	quad[3].m_Position[0] = x0 + width;
	quad[3].m_Position[1] = y0 + height;
	quad[3].m_Position[2] = 0.0f;
	quad[3].m_Texcoord[0] = 1.0f + fTexelW*0.5f;
	quad[3].m_Texcoord[1] = 0.0f + fTexelH*0.5f;

	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex_V3T2));
}

void DrawImage(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo, float x0, float y0, float w, float h, const char *tech = NULL)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	const char *szTech = tech ? tech : "DrawImage";
	D3DXHANDLE shader = g_pPostEffect->GetTechniqueByName(szTech);
	D3DXHANDLE pTextureVar = g_pPostEffect->GetParameterByName(NULL, "Image");

	g_pPostEffect->SetTechnique(shader);
	g_pPostEffect->SetTexture(pTextureVar, pSource);

	// 設定頂點資料格式	
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);

	g_pPostEffect->Begin(NULL, 0);
	g_pPostEffect->BeginPass(0);

	DrawScreenQuad(pInfo, x0, y0, w, h);

	g_pPostEffect->EndPass();
	g_pPostEffect->End();
}

void DrawImage(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo)
{
	DrawImage(pSource, pInfo, -1.0f, -1.0f, 2.0f, 2.0f);
}

static void DeferredLighting_Prepare(void)
{
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 wvp_matrix = world_matrix * view_matrix * g_proj_matrix;

	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderTarget(0, g_pSurfaces[TEX_POS]);
	device->SetRenderTarget(1, g_pSurfaces[TEX_NORMAL]);
	device->SetRenderTarget(2, g_pSurfaces[TEX_DIFFUSE]);
	device->SetDepthStencilSurface(g_pMainDepthbuffer);

	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	D3DXHANDLE shader = g_pPrepare->GetTechniqueByName("Prepare");
	D3DXHANDLE wvp_matrix_var = g_pPrepare->GetParameterByName(NULL, "wvp_matrix");
	D3DXHANDLE world_matrix_var = g_pPrepare->GetParameterByName(NULL, "world_matrix");

	g_pPrepare->SetTechnique(shader);
	g_pPrepare->SetMatrix(wvp_matrix_var, (D3DXMATRIX*)&wvp_matrix);
	g_pPrepare->SetMatrix(world_matrix_var, (D3DXMATRIX*)&world_matrix);

	g_pPrepare->Begin(NULL, 0);
	g_pPrepare->BeginPass(0);
		g_Model_DX9.Render(SUBMIT_TEXTURES);
	g_pPrepare->EndPass();
	g_pPrepare->End();

	device->SetRenderTarget(0, g_pMainFramebuffer);
	device->SetRenderTarget(1, NULL);
	device->SetRenderTarget(2, NULL);

	device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
}

static void DeferredLighting_Ambient(void)
{	
	float texel_w = 1.0f/(float)g_ImageInfo.m_iWidth;
	float texel_h = 1.0f/(float)g_ImageInfo.m_iHeight;

	Matrix4x4 ident_matrix; ident_matrix.Identity();
	Matrix4x4 tex_matrix; tex_matrix.Identity();
	tex_matrix.Scale_Replace(0.5f, -0.5f, 1.0f);
	tex_matrix[3].Set(0.5f + texel_w*0.5f, 0.5f + texel_h*0.5f, 0.0f, 1.0f);

	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	D3DXHANDLE shader = g_pLighting->GetTechniqueByName("AmbientLight");
	D3DXHANDLE wvp_matrix_var = g_pLighting->GetParameterByName(NULL, "wvp_matrix");
	D3DXHANDLE tex_matrix_var = g_pLighting->GetParameterByName(NULL, "texspace_matrix");

	D3DXHANDLE lightpos_var = g_pLighting->GetParameterByName(NULL, "vLightPos");
	D3DXHANDLE lightdiffuse_var = g_pLighting->GetParameterByName(NULL, "vLightColor");

	D3DXHANDLE worldpos_var = g_pLighting->GetParameterByName(NULL, "worldposTexture");
	D3DXHANDLE worldnormal_var = g_pLighting->GetParameterByName(NULL, "worldnormalTexture");
	D3DXHANDLE diffuse_var = g_pLighting->GetParameterByName(NULL, "diffuseTexture");

	g_pLighting->SetTechnique(shader);
	g_pLighting->SetMatrix(wvp_matrix_var, (D3DXMATRIX*)&ident_matrix);
	g_pLighting->SetMatrix(tex_matrix_var, (D3DXMATRIX*)&tex_matrix);

	g_pLighting->SetVector(lightpos_var, (D3DXVECTOR4 *)&g_Lights[0].m_vPosition[0]);
	g_pLighting->SetVector(lightdiffuse_var, (D3DXVECTOR4 *)&g_Lights[0].m_vAmbient[0]);

	g_pLighting->SetTexture(worldpos_var, g_pTextures[TEX_POS]);
	g_pLighting->SetTexture(worldnormal_var, g_pTextures[TEX_NORMAL]);
	g_pLighting->SetTexture(diffuse_var, g_pTextures[TEX_DIFFUSE]);

	// 用一個佔滿螢幕的矩形來打 ambient 光.
	g_pLighting->Begin(NULL, 0);
	g_pLighting->BeginPass(0);
		DrawScreenQuad(&g_ImageInfo, -1.0f, -1.0f, 2.0f, 2.0f);
	g_pLighting->EndPass();
	g_pLighting->End();
}

static void DeferredLighting_PointLights(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	float texel_w = 1.0f/(float)g_ImageInfo.m_iWidth;
	float texel_h = 1.0f/(float)g_ImageInfo.m_iHeight;

	Matrix4x4 ident_matrix; 
	ident_matrix.Identity();

	Matrix4x4 tex_matrix;
	tex_matrix.Scale_Replace(0.5f, -0.5f, 1.0f);
	tex_matrix[3].Set(0.5f + texel_w*0.5f, 0.5f + texel_h*0.5f, 0.0f, 1.0f);

	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 camera_matrix = g_Control.GetCameraMatrix();
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 vp_matrix = view_matrix * g_proj_matrix;

	D3DXHANDLE shader = g_pLighting->GetTechniqueByName("PointLight");
	g_pLighting->SetTechnique(shader);

	D3DXHANDLE wvp_matrix_var = g_pLighting->GetParameterByName(NULL, "wvp_matrix");
	D3DXHANDLE cam_pos_var = g_pLighting->GetParameterByName(NULL, "vCameraPos");
	D3DXHANDLE tex_matrix_var = g_pLighting->GetParameterByName(NULL, "texspace_matrix");

	D3DXHANDLE lightpos_var = g_pLighting->GetParameterByName(NULL, "vLightPos");
	D3DXHANDLE lightdiffuse_var = g_pLighting->GetParameterByName(NULL, "vLightColor");

	D3DXHANDLE worldpos_var = g_pLighting->GetParameterByName(NULL, "worldposTexture");
	D3DXHANDLE worldnormal_var = g_pLighting->GetParameterByName(NULL, "worldnormalTexture");
	D3DXHANDLE diffuse_var = g_pLighting->GetParameterByName(NULL, "diffuseTexture");

	g_pLighting->SetTexture(worldpos_var, g_pTextures[TEX_POS]);
	g_pLighting->SetTexture(worldnormal_var, g_pTextures[TEX_NORMAL]);
	g_pLighting->SetTexture(diffuse_var, g_pTextures[TEX_DIFFUSE]);
	g_pLighting->SetMatrix(tex_matrix_var, (D3DXMATRIX*)&tex_matrix);
	g_pLighting->SetVector(cam_pos_var, (D3DXVECTOR4*)&camera_matrix[3]);

	// 對每個光源畫一顆球來打光
	for ( int i=1; i<NUM_LIGHTS; i++ )
	{
		sGutLight *pLight = &g_Lights[i];

		Matrix4x4 world;
		world.Scale_Replace(pLight->m_vPosition.GetWWWW());
		world[3] = pLight->m_vPosition;
		world[3][3] = 1.0f;

		Matrix4x4 wvp = world * vp_matrix;
		g_pLighting->SetMatrix(wvp_matrix_var, (D3DXMATRIX*)&wvp);

		g_pLighting->SetVector(lightpos_var, (D3DXVECTOR4*) &pLight->m_vPosition);
		g_pLighting->SetVector(lightdiffuse_var, (D3DXVECTOR4*) &pLight->m_vDiffuse);

		g_pLighting->Begin(NULL, 0);
		g_pLighting->BeginPass(0);
			g_Sphere_DX9.Render(0);
		g_pLighting->EndPass();
		g_pLighting->End();
	}

	// 用點來畫出光源位置
	shader = g_pLighting->GetTechniqueByName("DisplayLight");
	Matrix4x4 wvp = view_matrix * g_proj_matrix;
	g_pLighting->SetTechnique(shader);
	g_pLighting->SetMatrix(wvp_matrix_var, (D3DXMATRIX*)&wvp);

	g_pLighting->Begin(NULL, 0);
	g_pLighting->BeginPass(0);
		device->SetVertexDeclaration(g_pVertexDecl);
		device->DrawPrimitiveUP(D3DPT_POINTLIST , NUM_LIGHTS, &g_LightsVC, sizeof(Vertex_VC));
	g_pLighting->EndPass();
	g_pLighting->End();
}

void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	
	device->BeginScene(); 

	DeferredLighting_Prepare();
	DeferredLighting_Ambient();
	DeferredLighting_PointLights();

	float x = -1.0f;
	float y = -1.0f;
	float w = 0.4f;
	float h = 0.4f;

	if ( g_iMode & 0x01 )
	{
		DrawImage(g_pTextures[TEX_POS], &g_ImageInfo, x, y, w, h);
		x+=w;
	}

	if ( g_iMode & 0x02 )
	{
		DrawImage(g_pTextures[TEX_NORMAL], &g_ImageInfo, x, y, w, h, "DrawNormal");
		x+=w;
	}

	if ( g_iMode & 0x04 )
	{
		DrawImage(g_pTextures[TEX_DIFFUSE], &g_ImageInfo, x, y, w, h);
		x+=w;
	}

	device->EndScene(); 
    device->Present( NULL, NULL, NULL, NULL );
}
