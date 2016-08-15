#include "render_data.h"

#include "Gut.h"
#include "GutModel_DX9.h"
#include "GutWin32.h"

CGutModel_DX9 g_Model_DX9;
CGutModel_DX9 g_SpotLightModel_DX9;

static sModelMaterial_DX9 g_material_stencilpass;
static sModelMaterial_DX9 g_material_spotlightpass;

static Matrix4x4 g_projection_matrix;

void InitStateDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
}

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);

	InitStateDX9();

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model);
	g_SpotLightModel_DX9.ConvertToDX9Model(&g_SpotLightModel);

	g_material_stencilpass.m_bCullFace = false;

	g_material_spotlightpass.m_bBlend = true;
	g_material_spotlightpass.m_SrcBlend = D3DBLEND_ONE;
	g_material_spotlightpass.m_DestBlend = D3DBLEND_ONE;

	g_material_spotlightpass.m_Material.Diffuse.r = 
		g_material_spotlightpass.m_Material.Diffuse.g = 
		g_material_spotlightpass.m_Material.Diffuse.b = 
		g_material_spotlightpass.m_Material.Diffuse.a = 0.3f;

	return true;
}

bool ReleaseResourceDX9(void)
{
	g_Model_DX9.Release();
	g_SpotLightModel_DX9.Release();
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
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	device->BeginScene(); 
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
	// 轉換矩陣
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// 畫出茶壼
	{
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);
		g_Model_DX9.Render();
	}
	// stencil pass
	{
		Matrix4x4 ident_matrix = Matrix4x4::IdentityMatrix();
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &ident_matrix);

		device->SetRenderState(D3DRS_STENCILREF, 1);
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
		device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

		// 畫出面對鏡頭的面，同時遞增 Stencil Buffer。
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		// 呼叫 Render 時傳入 0 代表不需要套用任何材質設定
		g_SpotLightModel_DX9.Render(0);

		// 畫出背對鏡頭的面，同時遞減 Stencil Buffer。
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		// 呼叫 Render 時傳入 0 代表不需要套用任何材質設定
		g_SpotLightModel_DX9.Render(0);

		// 還原設定
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 
			D3DCOLORWRITEENABLE_RED |
			D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE |
			D3DCOLORWRITEENABLE_ALPHA);
	}
	// 畫出光柱照射到的區域
	{
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILREF, 1);

		// 用 g_material_spotlightpass 來取代模型中的材質設定
		CGutModel_DX9::SetMaterialOverwrite(&g_material_spotlightpass);
		g_SpotLightModel_DX9.Render();
		// 還原，讓模型使用它原本的材質設定。
		CGutModel_DX9::SetMaterialOverwrite(NULL);
	}
	// 用正常的方法畫出光柱
	{
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		g_SpotLightModel_DX9.Render();
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}
	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
