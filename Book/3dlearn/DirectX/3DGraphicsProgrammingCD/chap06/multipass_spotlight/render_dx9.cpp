#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"

#include "render_data.h"

static LPDIRECT3DTEXTURE9 g_pTexture0 = NULL;
static LPDIRECT3DTEXTURE9 g_pTexture1 = NULL;

static Matrix4x4 g_projection_matrix;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 載入貼圖
	g_pTexture0 = GutLoadTexture_DX9("../../textures/brickwall.tga");
	g_pTexture1 = GutLoadTexture_DX9("../../textures/spotlight_effect.tga");

	// trilinear
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// trilinear
	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture0);
	SAFE_RELEASE(g_pTexture1);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	GutResetGraphicsDeviceDX9();
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_fOrthoWidth = g_fOrthoSize;
	g_fOrthoHeight = g_fOrthoSize;
	if ( aspect > 1.0f )
		g_fOrthoHeight *= aspect;
	else
		g_fOrthoWidth /= aspect;

	g_projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	//device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// trilinear
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// trilinear
	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 開始下繪圖指令
	device->BeginScene(); 
	// 設定資料格式
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 
	//device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER, 0x0, 1.0f, 0);

	{
		Matrix4x4 IdentityMatrix; 
		IdentityMatrix.Identity();

		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &IdentityMatrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &IdentityMatrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &IdentityMatrix);
		device->SetTexture(0, g_pTexture0);
		// ZBuffer測試條件設為永遠成立
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		// 畫出矩形, 同時會清除ZBuffer
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_FullScreenQuad, sizeof(Vertex_VT));
		// ZBuffer測試條件設為小於
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);
	}

	{
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		// 鏡頭座標系轉換矩陣
		Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
		device->SetTexture(0, g_pTexture1);
		// 開啟混色功能
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		// source_blend_factor = 1
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		// dest_blend_factor = 1
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		// 混色公式 = source_color * 1 + dest_color * 1
		// 畫出矩形
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
		// 關閉Alpha Test功能
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
