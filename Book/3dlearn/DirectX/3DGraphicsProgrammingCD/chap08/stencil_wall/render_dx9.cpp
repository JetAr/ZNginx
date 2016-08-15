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
	// 取得Direct3D9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// 載入貼圖
	g_pTexture0 = GutLoadTexture_DX9("../../textures/brickwall_broken.tga");
	//g_pTexture1 = GutLoadTexture_DX9("../../textures/checkerboard.tga");
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

// 使用Direct3D9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, 0x00000000, 1.0f, 0);
	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	// 開始下繪圖指令
	device->BeginScene(); 
	// 設定資料格式
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 

	{
		Matrix4x4 IdentityMatrix; 
		IdentityMatrix.Identity();

		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &IdentityMatrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &IdentityMatrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &IdentityMatrix);
		device->SetTexture(0, g_pTexture0);

		if ( g_bStencil )
		{
			// 啟動 Stencil Buffer Test
			device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
			// 把 Stencil 參考值設定 1
			device->SetRenderState(D3DRS_STENCILREF, 1);
			// Stencil Test 判斷條件設定成永遠成立
			device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
			// 直接把 Stencil 參考值填入 Stencil Buffer 里
			device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		}
		else
		{
			// 關閉Stencil Buffer Test
			device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
		}

		// 設定 Alpha Test 條件
		device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
		device->SetRenderState(D3DRS_ALPHAREF, 128);

		// 畫出墻壁, 會同時更新Framebuffer上的顏色跟Stencil Buffer.
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
	}

	{
		if ( g_bStencil )
		{
			// 下面 3 行程式碼會設定成只更新畫面上 Stencil 值為 1 的像素
			// 把 Stencil 參考值設定 1
			device->SetRenderState(D3DRS_STENCILREF, 1);
			// Stencil Test 判斷條件設定成等於 Stencil 參考值才成立
			device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
			// Stencil Test 判斷成立時，保留 Stencil Buffer 值，不去更新它。
			device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		}

		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		// 鏡頭座標系轉換矩陣
		Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
		device->SetTexture(0, g_pTexture1);
		// 開啟混色功能
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		// 混色公式 = source_color * 1 + dest_color * 1
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		// 畫出光柱
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
		// 關閉Alpha Test功能
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	}

	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
