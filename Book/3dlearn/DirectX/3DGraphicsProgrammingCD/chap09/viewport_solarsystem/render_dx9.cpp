#include "Gut.h"
#include "GutModel_DX9.h"
#include "render_data.h"

#include "GutWin32.h"

static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_view_matrix;
static CGutModel_DX9 g_Models_DX9[3];

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(70.0f, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	for ( int i=0; i<3; i++ )
	{
		g_Models_DX9[i].ConvertToDX9Model(&g_Models[i]);
	}

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
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(70.0f, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
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

void SetupLightingDX9(Light *pLightArray, int num_lights)
{
	// 取得Direct3D9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// 設定環境光
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));

	int i;

	for ( i=0; i<num_lights; i++ )
	{
		// 設定光源
		device->LightEnable(i, TRUE);

		D3DLIGHT9 light;
		ZeroMemory( &light, sizeof(light) );

		light.Type = D3DLIGHT_POINT;

		light.Position = *(D3DVECTOR *) &pLightArray[i].m_Position;
		light.Diffuse = *(D3DCOLORVALUE*) &pLightArray[i].m_Diffuse;
		light.Specular = *(D3DCOLORVALUE*) &pLightArray[i].m_Specular;
		light.Range = 1000.0f;

		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;

		device->SetLight(i, &light);
	}
}


void RenderSolarSystemDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 太陽
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_sun_matrix);
	g_Models_DX9[0].Render();
	//device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// 地球
	//SetupLightingDX9(&g_Light, 1);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_earth_matrix);
	g_Models_DX9[1].Render();
	// 月亮
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_moon_matrix);
	g_Models_DX9[2].Render();
}
// 使用Direct3D9來繪圖
void RenderFrameDX9(void)
{
	// `取得視窗大小`
	int w, h;
	GutGetWindowSize(w, h);
	// `清除畫面`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 100, 100, 255), 1.0f, 0);
	// `開始下繪圖指令`
	device->BeginScene(); 

	Vector4 camera_lookat(0.0f, 0.0f, 0.0f);
	Matrix4x4 ortho_proj = GutMatrixOrthoRH_DirectX(20.0f, 20.0f, 0.1f, 100.0f);

	{
		// `前視圖`
		D3DVIEWPORT9 viewport = {0, 0, w/2, h/2, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Vector4 camera_pos(0.0f, -20.0f, 0.0f);
		Vector4 camera_up(0.0f, 0.0f, 1.0f);
		g_view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
		// projection matrix
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&ortho_proj);
		// render objects
		RenderSolarSystemDX9();
	}
	{
		// `上視圖`
		D3DVIEWPORT9 viewport = {w/2, 0, w/2, h/2, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Vector4 camera_pos(0.0f, 0.0f, 20.0f);
		Vector4 camera_up(0.0f, 1.0f, 0.0f);
		g_view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
		// projection matrix
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&ortho_proj);
		// render objects
		RenderSolarSystemDX9();
	}
	{
		// `右視圖`
		D3DVIEWPORT9 viewport = {0, h/2, w/2, h/2, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Vector4 camera_pos(20.0f, 0.0f, 0.0f);
		Vector4 camera_up(0.0f, 0.0f, 1.0f);
		g_view_matrix = GutMatrixLookAtRH(camera_pos, camera_lookat, camera_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
		// projection matrix
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&ortho_proj);
		// render objects
		RenderSolarSystemDX9();
	}
	{
		// `使用者視角`
		D3DVIEWPORT9 viewport = {w/2, h/2, w/2, h/2, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
		g_view_matrix = object_matrix * view_matrix;
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
		// projection matrix
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&g_projection_matrix);
		// render objects
		RenderSolarSystemDX9();
	}
	{
		D3DVIEWPORT9 viewport = {0, 0, w, h, 0.0f, 1.0f};
		device->SetViewport(&viewport);
		// view matrix
		Matrix4x4 ident_mat; ident_mat.Identity();
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&ident_mat);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&ident_mat);
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&ident_mat);
		// render border
		device->SetFVF(D3DFVF_XYZ);
		device->DrawPrimitiveUP(D3DPT_LINELIST, 2, g_Border, sizeof(Vertex_VC));
	}
	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 
	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
