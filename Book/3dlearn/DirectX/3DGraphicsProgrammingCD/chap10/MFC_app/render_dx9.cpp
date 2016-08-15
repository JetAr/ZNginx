#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 1.0f, 100.0f);

	// 設定視角轉換矩陣
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 設定鏡頭轉換矩陣
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//device->SetRenderState(D3DRS_SHADEMODE, D3DSHADE_FLAT); // 關閉內插
	//device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME); // 使用畫邊線模式

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int w, int h)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// Reset Device
	GutResetGraphicsDeviceDX9();

	// 設定視角轉換矩陣
	float aspect = (float) h / (float) w;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 設定鏡頭轉換矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->Clear(
		0, NULL, // 清除整個畫面 
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // 清除顏色跟Z buffer 
		D3DCOLOR_ARGB(0, 0, 0, 0), // 設定要把顏色清成黑色
		1.0f, // 設定要把Z值清為1, 也就是離鏡頭最遠
		0 // 設定要把Stencil buffer清為0, 在這沒差.
		);

	static float angle = 0.0f;
	angle += 0.01f;

	// 開始下繪圖指令
	device->BeginScene(); 

	// 設定資料格式
	// D3DFVF_XYZ = 使用4個浮點數來記錄位置
	// D3DFVF_DIFFUSE = 使用32bits整數型態來記錄BGRA顏色
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	// 設定轉換矩陣
	Matrix4x4 world_matrix;
	world_matrix.RotateZ_Replace(angle); // 產生旋轉矩陣
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

	// 畫出金字塔的8條邊線
	device->DrawIndexedPrimitiveUP(
		D3DPT_TRIANGLELIST, // 指定所要畫的基本圖形種類 
		0, // 會使用的最小頂點編號, 事實上沒太大用處
		5, // 頂點陣列里有幾個頂點
		6, // 要畫出幾個基本圖形
		g_indices, // 索引陣列
		D3DFMT_INDEX16, // 索引陣列的型態
		g_vertices, // 頂點陣列
		sizeof(Vertex_VC) // 頂點陣列里每個頂點的記憶體間距
		); 

	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 

	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
