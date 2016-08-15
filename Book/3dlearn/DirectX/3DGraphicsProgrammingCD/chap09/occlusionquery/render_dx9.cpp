#include "Gut.h"
#include "render_data.h"

#include "GutWin32.h"

Matrix4x4 g_projection_matrix;
LPDIRECT3DQUERY9 g_pOcclusionQuery[2];

bool InitResourceDX9(void)
{
	// 取得Direct3D9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//
	device->CreateQuery(D3DQUERYTYPE_OCCLUSION, &g_pOcclusionQuery[0]);
	device->CreateQuery(D3DQUERYTYPE_OCCLUSION, &g_pOcclusionQuery[1]);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pOcclusionQuery[0]);
	SAFE_RELEASE(g_pOcclusionQuery[1]);

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
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(90.0f, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

static int g_FrameCount = 0;

void RenderSolarSystemDX9(void)
{
	int index = g_FrameCount % 2;
	LPDIRECT3DQUERY9 pQuery = g_pOcclusionQuery[index];

	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定資料格式
	// D3DFVF_XYZ = 使用3個浮點數來記錄位置
	// D3DFVF_DIFFUSE = 使用32bits整數型態來記錄BGRA顏色
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 
	// 太陽
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_sun_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
		g_pSphereIndices, D3DFMT_INDEX16, g_pSunVertices, sizeof(Vertex_VC) );
	// 地球
	pQuery->Issue(D3DISSUE_BEGIN);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_earth_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
		g_pSphereIndices, D3DFMT_INDEX16, g_pEarthVertices, sizeof(Vertex_VC) );
	pQuery->Issue(D3DISSUE_END);
	// 月亮
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_moon_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, g_iNumSphereTriangles, 
		g_pSphereIndices, D3DFMT_INDEX16, g_pMoonVertices, sizeof(Vertex_VC) );

	if ( g_FrameCount )
	{
		int num_loops = 0;
		int num_samples_passed = 0;
		// 去檢查前一個畫面的Occlusion Query結果
		pQuery = g_pOcclusionQuery[(index + 1) % 2];
		while( pQuery->GetData(&num_samples_passed, 4, D3DGETDATA_FLUSH)==S_FALSE  )
		{
			// 結果可能還沒出來, 要再查詢一次
			num_loops++;
		}

		printf("Earth %s\t\r", num_samples_passed ? "visible" : "disappear");
	}

	g_FrameCount++;
}
// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	// 取得視窗大小
	int w, h;
	GutGetWindowSize(w, h);
	// 清除畫面
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 100, 100, 255), 1.0f, 0);
	// 開始下繪圖指令
	device->BeginScene(); 
	// view matrix
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	g_view_matrix = object_matrix * view_matrix;
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&g_view_matrix);
	// projection matrix
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&g_projection_matrix);
	// render objects
	RenderSolarSystemDX9();

	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
