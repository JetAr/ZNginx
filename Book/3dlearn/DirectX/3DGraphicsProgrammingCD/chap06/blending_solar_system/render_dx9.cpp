#include "Gut.h"
#include "render_data.h"
#include <d3dx9.h>
#include <d3dx9math.h>

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

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 50, 50, 50), 1.0f, 0);

	// 開始下繪圖指令
	device->BeginScene(); 

	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	// 設定資料格式
	// D3DFVF_XYZ = 使用3個浮點數來記錄位置
	// D3DFVF_DIFFUSE = 使用32bits整數型態來記錄BGRA顏色
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	device->SetRenderState(D3DRS_ZWRITEENABLE, g_bZWrite ? TRUE : FALSE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);

	switch(g_iBlendMode)
	{
	case 0:
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		break;
	case 1:
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		break;
	}

	Matrix4x4 world_view_matrix;

	for ( int i=0; i<2; i++ )
	{
		switch( g_iOrder[i] )
		{
		case 0:
			// 太陽
			world_view_matrix = g_sun_matrix * g_rot_matrix;
			device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_view_matrix);
			device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
				g_pSphereIndices, D3DFMT_INDEX16, g_pSunVertices, sizeof(Vertex_VC) );
			break;
		case 1:
			// 地球
			world_view_matrix = g_earth_matrix * g_rot_matrix;
			device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_view_matrix);
			device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
				g_pSphereIndices, D3DFMT_INDEX16, g_pEarthVertices, sizeof(Vertex_VC) );
			break;
		}
	}

	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 

	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
