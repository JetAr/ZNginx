#include "render_data.h"

#include "Gut.h"
#include "GutDX9.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"
#include "GutModel_DX9.h"

LPDIRECT3DTEXTURE9 g_pTexture = NULL;

LPD3DXEFFECT g_pEffect = NULL;
LPD3DXEFFECT g_pRefractionFX = NULL;

static sImageInfo g_ImageInfo;
static Matrix4x4 g_proj_matrix;

static CGutModel_DX9 g_Model_DX9;

bool ReInitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	int w, h;
	GutGetWindowSize(w, h);
	
	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNearZ, g_fFarZ);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&g_proj_matrix);

	return true;
}

bool InitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	CGutModel::SetTexturePath("../../textures/");

	g_pTexture = GutLoadTexture_DX9("../../textures/lena.tga", &g_ImageInfo);

	g_pEffect = GutLoadFXShaderDX9("../../shaders/Posteffect.fx");
	if ( NULL==g_pEffect )
		return false;

	g_pRefractionFX = GutLoadFXShaderDX9("../../shaders/Refraction.fx");
	if ( NULL==g_pRefractionFX )
		return false;

	g_Model_DX9.ConvertToDX9Model(&g_Model);

	if ( !ReInitResourceDX9() )
		return false;

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pRefractionFX);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// 取得Direct3D 9裝置
	ReInitResourceDX9();
}

void DrawImage(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// 計算貼圖像素的貼圖座標間距
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

	// Direct3D9會偏移半個像素, 從貼圖座標來把它修正回來
	Vector4 vTexoffset(fTexelW*0.5f, fTexelH*0.5f, 0.0f, 1.0f);

	Vertex_VT quad[4];
	memcpy(quad, g_FullScreenQuad, sizeof(quad));
	for ( int i=0; i<4; i++ )
	{
		quad[i].m_Texcoord[0] += vTexoffset[0];
		quad[i].m_Texcoord[1] += vTexoffset[1];
	}

	D3DXHANDLE shader = g_pEffect->GetTechniqueByName("DrawImage");
	D3DXHANDLE pTextureVar = g_pEffect->GetParameterByName(NULL, "Image");

	g_pEffect->SetTechnique(shader);
	g_pEffect->SetTexture(pTextureVar, pSource);

	// 設定頂點資料格式	
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);

	g_pEffect->Begin(NULL, 0);
	g_pEffect->BeginPass(0);
	
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex_VT));
	
	g_pEffect->EndPass();
	g_pEffect->End();
}

void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->BeginScene(); 
	device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.0f, 0);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 wv_matrix = world_matrix * view_matrix;
	Matrix4x4 wvp_matrix = wv_matrix * g_proj_matrix;

	DrawImage(g_pTexture, &g_ImageInfo);

	D3DXHANDLE shader = g_pRefractionFX->GetTechniqueByName("Refraction");
	D3DXHANDLE texture = g_pRefractionFX->GetParameterByName(NULL, "BackgroundImage");
	D3DXHANDLE wv_matrix_h = g_pRefractionFX->GetParameterByName(NULL, "wv_matrix");
	D3DXHANDLE wvp_matrix_h = g_pRefractionFX->GetParameterByName(NULL, "wvp_matrix");
	D3DXHANDLE objectcolor_h = g_pRefractionFX->GetParameterByName(NULL, "object_color");

	g_pRefractionFX->SetTechnique(shader);
	g_pRefractionFX->SetTexture(texture, g_pTexture);
	g_pRefractionFX->SetMatrix(wv_matrix_h, (D3DXMATRIX *)&wv_matrix);
	g_pRefractionFX->SetMatrix(wvp_matrix_h, (D3DXMATRIX *)&wvp_matrix);
	g_pRefractionFX->SetVector(objectcolor_h, (D3DXVECTOR4 *)&g_vObjectColor);

	g_pRefractionFX->Begin(NULL, 0);
	g_pRefractionFX->BeginPass(0);

	g_Model_DX9.Render(0);

	g_pRefractionFX->EndPass();
	g_pRefractionFX->End();

	device->EndScene();
	device->Present( NULL, NULL, NULL, NULL );
}
