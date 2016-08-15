#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"
#include "GutModel_DX9.h"

#include "render_data.h"

static Matrix4x4 g_projection_matrix;

CGutModel_DX9 g_Model_DX9;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNear, g_fFar);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	for ( int t=0; t<MAX_NUM_TEXTURES; t++ )
	{
		// trilinear filter
		device->SetSamplerState(t, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(t, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(t, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}

	g_Model_DX9.ConvertToDX9Model(&g_Model);
	//g_Model_DX9.SetCullMode(D3DCULL_NONE);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	device->SetRenderState(D3DRS_COLORVERTEX, FALSE);
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

	device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return true;
}

bool ReleaseResourceDX9(void)
{

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNear, g_fFar);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	device->SetRenderState(D3DRS_COLORVERTEX, FALSE);
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, D3DMCS_MATERIAL);

}

D3DCOLOR ConvertToD3DCOLOR(Vector4 &vColor)
{
	Vector4 vColor_0_255 = vColor;
	vColor_0_255.Saturate();
	vColor_0_255 *= 255.0f;

	int r = (int) vColor_0_255.GetX();
	int g = (int) vColor_0_255.GetY();
	int b = (int) vColor_0_255.GetZ();
	int a = (int) vColor_0_255.GetW();

	D3DCOLOR d3dcolor = D3DCOLOR_ARGB(a, r, g, b);
	return d3dcolor;
}

void SetupLightingDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);

	GutSetupLightDX9(0, g_Lights[0]);
	GutSetupLightDX9(1, g_Lights[1]);
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	device->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 設定轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

	// 開始下繪圖指令
	device->BeginScene(); 

	SetupLightingDX9();

	g_Model_DX9.Render();

	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 

	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
