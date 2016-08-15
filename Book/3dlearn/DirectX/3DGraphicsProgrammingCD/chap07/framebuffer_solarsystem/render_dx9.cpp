#include "Gut.h"
#include "render_data.h"

LPDIRECT3DTEXTURE9 g_pTexture = NULL;
LPDIRECT3DSURFACE9 g_pDepthStencil = NULL;

Matrix4x4 g_projection_matrix;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	device->CreateTexture(512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexture, NULL);
	device->CreateDepthStencilSurface(512, 512, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &g_pDepthStencil, NULL);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pDepthStencil);

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
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 改變三角形正面的面向
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
}

void RenderSolarSystemDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Vector4 eye(0.0f, 0.0f, 15.0f); 
	Vector4 lookat(0.0f, 0.0f, 0.0f); 
	Vector4 up(0.0f, 1.0f, 0.0f); 

	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);

	Matrix4x4 view_matrix = GutMatrixLookAtRH(eye, lookat, up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);

	// `設定資料格式`
	// D3DFVF_XYZ = `使用3個浮點數來記錄位置`
	// D3DFVF_DIFFUSE = `使用32bits整數型態來記錄BGRA顏色`
	device->SetFVF(D3DFVF_XYZ|D3DFVF_DIFFUSE); 

	// `太陽`
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_sun_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, 
		g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pSunVertices, sizeof(Vertex_VC) );
	// `地球`
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_earth_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, 
		g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pEarthVertices, sizeof(Vertex_VC) );
	// `月亮`
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_moon_matrix);
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, g_iNumSphereVertices, 
		g_iNumSphereTriangles, g_pSphereIndices, D3DFMT_INDEX16, g_pMoonVertices, sizeof(Vertex_VC) );
}
// 使用Direct3D9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// `開始下繪圖指令`
	device->BeginScene(); 

	// `把太陽系的運動畫到另一個render target中`
	{
		LPDIRECT3DSURFACE9 pFrameBufferBackup, pDepthBufferBackup;
		device->GetRenderTarget(0, &pFrameBufferBackup); pFrameBufferBackup->Release();
		device->GetDepthStencilSurface(&pDepthBufferBackup); pDepthBufferBackup->Release();

		LPDIRECT3DSURFACE9 pSurface;
		g_pTexture->GetSurfaceLevel(0, &pSurface); 

		device->SetRenderTarget(0, pSurface);
		device->SetDepthStencilSurface(g_pDepthStencil);

		device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(100, 100, 100, 255), 1.0f, 0);

		RenderSolarSystemDX9();

		pSurface->Release();
		device->SetRenderTarget(0, pFrameBufferBackup);
		device->SetDepthStencilSurface(pDepthBufferBackup);
	}

	// `把上一個步驟的結果當成貼圖來使用`
	{
		// `消除畫面`
		device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 128, 255), 1.0f, 0);
		// `投影矩陣`
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		// `鏡頭座標系轉換矩陣`
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);
		// `套用動態貼圖`
		device->SetTexture(0, g_pTexture);
		// `畫出矩形`
		device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
	}

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 
	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
