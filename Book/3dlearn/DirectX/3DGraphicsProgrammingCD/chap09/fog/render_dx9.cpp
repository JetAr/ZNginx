#include "Gut.h"
#include "render_data.h"
#include "GutModel_DX9.h"
#include "GutWin32.h"

CGutModel_DX9 g_Model_DX9;

static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_mirror_view_matrix;

void InitStateDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 使用trilinear
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
}

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

	InitStateDX9();

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, aspect, 0.1f, 100.0f);

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	InitStateDX9();
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	DWORD FogColor = D3DCOLOR_RGBA(128, 128, 128, 255);

	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->BeginScene(); 
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, FogColor, 1.0f, 0);
	// 設定轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &object_matrix);
	// 設定霧
	device->SetRenderState(D3DRS_FOGENABLE, TRUE);
	device->SetRenderState(D3DRS_FOGCOLOR, FogColor);

	switch(g_iFogMode)
	{
	case 0:
		device->SetRenderState(D3DRS_FOGENABLE, FALSE);
		break;
	case 1:
		{
			// 隨距離線性變濃的霧
			float fStart = 0.0f;
			float fEnd = 10.0f;
			device->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
			device->SetRenderState(D3DRS_FOGSTART, *(DWORD *)(&fStart));
			device->SetRenderState(D3DRS_FOGEND, *(DWORD *)(&fEnd));
			// 計算公式為
			// (fog_end - distance_to_camera) / (fog_end - fog_start)
			break;
		}
	case 2:
		{
			// `套用指數函式來變化的霧 `
			float fFogDensity = 0.5f;
			device->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_EXP);
			device->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)&fFogDensity);
			// 計算公式為
			// power(e, -(fog_density * distance_to_camera))
			break;
		}
	case 3:
		{
			// `套用指數函式來變化的霧 `
			float fFogDensity = 0.5f;
			device->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_EXP2);
			device->SetRenderState(D3DRS_FOGDENSITY, *(DWORD *)&fFogDensity);
			// 計算公式為
			// power(e, -(fog_density * distance_to_camera)^2)
			break;
		}
	}
	// 畫出模型
	g_Model_DX9.Render();
	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
