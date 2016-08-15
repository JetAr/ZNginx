#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `計算出一個可以轉換到鏡頭座標系的矩陣`
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

	device->Clear(
		0, NULL, // `清除整個畫面 `
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // `清除顏色跟Z Buffer`
		D3DCOLOR_ARGB(0, 0, 0, 0), // `定要把顏色清成黑色`
		1.0f, // `設定要把Z值清為1, 也就是離鏡頭最遠.`
		0 // `設定要把Stencil buffer清為0, 在這沒差.`
		);

	// `開始下繪圖指令`
	device->BeginScene(); 
	// `設定資料格式`
	device->SetFVF(D3DFVF_XYZ); 

	// `4個金字塔的基本位置`
	Vector4 pos[4] = 
	{
		Vector4(-1.0f, -1.0f, 0.0f),
		Vector4( 1.0f, -1.0f, 0.0f),
		Vector4(-1.0f,  1.0f, 0.0f),
		Vector4( 1.0f,  1.0f, 0.0f),
	};

	Matrix4x4 world_matrix;

	for ( int i=0; i<4; i++ )
	{
		// `設定轉換矩陣`
		world_matrix.Translate_Replace(pos[i]);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);
		// `畫出金字塔的8條邊線`
		device->DrawPrimitiveUP(D3DPT_LINELIST, 8, g_vertices, sizeof(Vector4)); 
	}

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 

	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
