#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

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

	D3DPRESENT_PARAMETERS d3dpresent;

	ZeroMemory( &d3dpresent, sizeof(d3dpresent) );
	d3dpresent.Windowed = TRUE; // 使用視窗模式
	d3dpresent.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpresent.BackBufferFormat = D3DFMT_UNKNOWN; // 使用視窗模式可以不去設定
	d3dpresent.BackBufferCount = 1; // 提供一塊backbuffer
	d3dpresent.EnableAutoDepthStencil = TRUE; // 自動開啟DepthStencil Buffer
	d3dpresent.AutoDepthStencilFormat = D3DFMT_D24S8; // DepthStencil Buffer模式

	device->Reset(&d3dpresent);

	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
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
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	// `設定資料格式`
	// D3DFVF_XYZ = `使用3個浮點數來記錄位置`
	// D3DFVF_DIFFUSE = `使用32bits整數型態來記錄BGRA顏色`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	// `太陽`
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_sun_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
		g_pSphereIndices, D3DFMT_INDEX16, g_pSunVertices, sizeof(Vertex_VC) );
	// `地球`
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_earth_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
		g_pSphereIndices, D3DFMT_INDEX16, g_pEarthVertices, sizeof(Vertex_VC) );
	// `月亮`
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_moon_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
		g_pSphereIndices, D3DFMT_INDEX16, g_pMoonVertices, sizeof(Vertex_VC) );

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 

	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
