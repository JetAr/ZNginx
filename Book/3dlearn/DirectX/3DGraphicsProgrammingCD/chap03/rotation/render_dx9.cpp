#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `計算出鏡頭座標系的轉換矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, 1.0f, 100.0f);
	// `設定視角轉換矩陣`
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// `設定鏡頭轉換矩陣`
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// `關閉打光`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

// `使用Direct3D9來繪圖`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	// `開始下繪圖指令`
	device->BeginScene(); 
	// `設定資料格式`
	device->SetFVF(D3DFVF_XYZ); 

	// `旋轉角度`
	static float angle = 0.0f;
	angle += 0.01f;

	// `設定旋轉矩陣`
	Matrix4x4 world_matrix;
	world_matrix.RotateZ_Replace(angle);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

	// `畫出金字塔的8條邊線`
	device->DrawIndexedPrimitiveUP(
		D3DPT_LINELIST, // `指定所要畫的基本圖形種類 `
		0, // `會使用的最小頂點編號, 事實上沒太大用處.`
		5, // `頂點陣列里有幾個頂點`
		8, // `要畫出幾個基本圖形`
		g_indices,		// `索引陣列`
		D3DFMT_INDEX16, // `索引陣列的型態`
		g_vertices,		// `頂點陣列`
		sizeof(Vector4)	// `頂點陣列里每個頂點的記憶體間距`
		);

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 

	// `把背景backbuffer呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
