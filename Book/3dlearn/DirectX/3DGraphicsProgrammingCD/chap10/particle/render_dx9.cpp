#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutFont_DX9.h"
#include "GutModel_DX9.h"
#include "GutTexture_DX9.h"
#include "GutHeightmap_DX9.h"

#include "render_data.h"

static CGutFontDX9 g_Font;
static LPDIRECT3DTEXTURE9 g_pTexture = NULL;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_Font.SetFontSize(32, 32);
	g_Font.SetFontAdvance(20, 30);
	g_Font.SetConsoleSize(32, 16);
	// 載入字型
	g_Font.LoadTexture("../../textures/ascii_couriernew.tga");

	g_pTexture = GutLoadTexture_DX9("../../textures/particle.tga");

	Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&proj_matrix);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 	
	g_Font.SetConsoleResolution(width, height);
	g_Font.BuildMesh();

	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &proj_matrix);
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// 開始下繪圖指令
	device->BeginScene(); 

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix; world_matrix.Identity();

	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&world_matrix);

	//
	sModelMaterial_DX9 mtl;

	mtl.m_bBlend = true;
	mtl.m_SrcBlend = D3DBLEND_ONE;
	mtl.m_DestBlend = D3DBLEND_ONE;
	mtl.m_pTextures[0] = g_pTexture;
	mtl.Submit();

	device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	if ( g_Particle.m_iNumParticles )
	{
		if ( g_bPointSprite )
		{
			int w,h; GutGetWindowSize(w, h);
			float fAspect = (float)h/(float)w;
			float fovH = FastMath::DegToRad(fAspect * g_fFovW);
			float tanH = FastMath::Tan(fovH * 0.5f);

			device->SetFVF(D3DFVF_XYZ|D3DFVF_PSIZE|D3DFVF_DIFFUSE);

			float pScaleA = 0.0f;
			float pScaleB = 0.0f;
			float pScaleC = 4.0f * tanH * tanH;

			device->SetRenderState(D3DRS_POINTSPRITEENABLE, TRUE);
			device->SetRenderState(D3DRS_POINTSCALEENABLE, TRUE);

			device->SetRenderState(D3DRS_POINTSCALE_A, *((DWORD*)&pScaleA));
			device->SetRenderState(D3DRS_POINTSCALE_B, *((DWORD*)&pScaleB));
			device->SetRenderState(D3DRS_POINTSCALE_C, *((DWORD*)&pScaleC));

			device->DrawPrimitiveUP(
				D3DPT_POINTLIST, g_Particle.m_iNumParticles, 
				g_Particle.m_pPointSpriteArray, sizeof(sPointSpriteVertex));

			device->SetRenderState(D3DRS_POINTSCALEENABLE, FALSE);
			device->SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
		}
		else
		{
			device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE|D3DFVF_TEX1);

			device->DrawIndexedPrimitiveUP(
				D3DPT_TRIANGLELIST, 0,
				g_Particle.m_iNumParticles*4,
				g_Particle.m_iNumParticles*2,
				g_Particle.m_pIndexArray, D3DFMT_INDEX16,
				g_Particle.m_pVertexArray, sizeof(sParticleVertex));
		}
	}
	//
	device->EndScene(); 
	// 把背景backbuffer呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
