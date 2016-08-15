#include "Gut.h"
#include "render_data.h"

static LPDIRECT3DVERTEXSHADER9 g_pVertexShaderDX9 = NULL;
static LPDIRECT3DPIXELSHADER9  g_pPixelShaderDX9 = NULL;

static Matrix4x4 g_proj_matrix;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 載入Vertex Shader
	g_pVertexShaderDX9 = GutLoadVertexShaderDX9_HLSL("../../shaders/vertex_color.shader", "VS", "vs_1_1");
	if ( g_pVertexShaderDX9==NULL )
		return false;
	// 載入Pixel Shader
	g_pPixelShaderDX9 = GutLoadPixelShaderDX9_HLSL("../../shaders/vertex_color.shader", "PS", "ps_2_0");
	if ( g_pPixelShaderDX9==NULL )
		return false;
	// 計算投影轉換矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pVertexShaderDX9);
	SAFE_RELEASE(g_pPixelShaderDX9);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	GutResetGraphicsDeviceDX9();

	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
}

// `使用Direct3D9來繪圖`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `消除畫面`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// `開始下繪圖指令`
	device->BeginScene(); 
	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 view_proj_matrix = view_matrix * g_proj_matrix;
	Matrix4x4 world_view_proj_matrix;
	// `設定資料格式`
	// `D3DFVF_XYZ = 使用3個浮點數來記錄位置`
	// `D3DFVF_DIFFUSE = 使用32bits整數型態來記錄BGRA顏色`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 
	// `設定Shader`
	device->SetVertexShader(g_pVertexShaderDX9);
	device->SetPixelShader(g_pPixelShaderDX9);
	// `太陽`
	world_view_proj_matrix = g_sun_matrix * view_proj_matrix;
	device->SetVertexShaderConstantF(0, (const float *)&world_view_proj_matrix, 4);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pSunVertices, sizeof(Vertex_VC) );
	// `地球`
	world_view_proj_matrix = g_earth_matrix * view_proj_matrix;
	device->SetVertexShaderConstantF(0, (const float *)&world_view_proj_matrix, 4);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pEarthVertices, sizeof(Vertex_VC) );
	// `月亮`
	world_view_proj_matrix = g_moon_matrix * view_proj_matrix;
	device->SetVertexShaderConstantF(0, (const float *)&world_view_proj_matrix, 4);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pMoonVertices, sizeof(Vertex_VC) );
	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 
	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
