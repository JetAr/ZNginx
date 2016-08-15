#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"

#include "render_data.h"

static LPDIRECT3DTEXTURE9 g_pTexture = NULL;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 鏡頭座標系的轉換矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	// 載入貼圖
	//D3DXCreateTextureFromFile(device, "../../textures/lena.bmp", &g_pTexture);
	D3DXCreateTextureFromFileEx(device, "../../textures/lena.bmp", 
		D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
		D3DPOOL_MANAGED, D3DX_DEFAULT, 
		D3DX_DEFAULT, 0, NULL, NULL,
		&g_pTexture);

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
	// 設定視角轉換矩陣
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 鏡頭座標系的轉換矩陣
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
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	//
	int w, h;
	GutGetWindowSize(w, h);
	RECT rect = {0, 0, w, h/2};
	device->SetScissorRect(&rect);
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, TRUE);

	// 開始下繪圖指令
	device->BeginScene(); 
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
	// 設定資料格式
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 
	// 套用貼圖
	device->SetTexture(0, g_pTexture);
	// 畫出格子
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
	device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
}
