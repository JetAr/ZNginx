#include "Gut.h"
#include "GutModel_DX9.h"
#include "render_data.h"

LPDIRECT3DCUBETEXTURE9 g_pTexture = NULL;
LPDIRECT3DSURFACE9 g_pDepthStencil = NULL;

static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_mirror_view_matrix;
static CGutModel_DX9 g_Models_DX9[4];

bool InitResourceDX9(void)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `設定視角轉換矩陣`

	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// `關閉打光`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// `改變三角形正面的面向`
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
	device->CreateCubeTexture(512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexture, NULL);
	device->CreateDepthStencilSurface(512, 512, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &g_pDepthStencil, NULL);

	if ( g_pTexture==NULL || g_pDepthStencil==NULL )
		return false;

	CGutModel::SetTexturePath("../../textures/");

	for ( int i=0; i<4; i++ )
	{
		g_Models_DX9[i].ConvertToDX9Model(&g_Models[i]);
	}

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
	// 地球
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_earth_matrix);
	g_Models_DX9[1].Render();
	// 月亮
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_moon_matrix);
	g_Models_DX9[2].Render();
	// 火星
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_mars_matrix);
	g_Models_DX9[3].Render();
}
// 使用Direct3D9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// `開始下繪圖指令`
	device->BeginScene(); 

	// `更新cubemap 6個面`
	{
		// `記錄主`framebuffer
		LPDIRECT3DSURFACE9 pFrameBufferBackup, pDepthBufferBackup;
		device->GetRenderTarget(0, &pFrameBufferBackup);
		pFrameBufferBackup->Release();
		device->GetDepthStencilSurface(&pDepthBufferBackup);
		pDepthBufferBackup->Release();

		Matrix4x4 mirror_x; mirror_x.Scale_Replace(-1.0f, 1.0f, 1.0f);
		Matrix4x4 cubemap_projection_matrix = mirror_x * GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 0.1f, 100.0f);

		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &cubemap_projection_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);

		for ( int i=0; i<6; i++ )
		{
			// `取出cubemap的其中一個面`
			LPDIRECT3DSURFACE9 pSurface;
			D3DCUBEMAP_FACES face = (D3DCUBEMAP_FACES) (D3DCUBEMAP_FACE_POSITIVE_X + i);
			g_pTexture->GetCubeMapSurface(face, 0, &pSurface); 
			// `把畫面畫到cubemap其中一個面`
			device->SetRenderTarget(0, pSurface);
			device->SetDepthStencilSurface(g_pDepthStencil);
			// `清除畫面`
			device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
			// `更新鏡頭面向`
			Matrix4x4 cubemap_view_matrix = GutMatrixLookAtRH(g_vCubemap_Eye, g_vCubemap_Lookat[i], g_vCubemap_Up[i]);
			device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &cubemap_view_matrix);
			// `畫行星`
			RenderSolarSystemDX9();
			// `告知d3d裝置這個surface使用完畢, 減少surface的reference count.`
			pSurface->Release();
		}

		// `還原成使用主framebuffer`
		device->SetRenderTarget(0, pFrameBufferBackup);
		device->SetDepthStencilSurface(pDepthBufferBackup);
	}

	// `把上一個步驟的結果當成貼圖來使用`
	{
		// `消除畫面`
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
		// `轉換矩陣`
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_sun_matrix);
		// `套用矩陣把反射轉換到世界座標系`
		Matrix4x4 inv_view = g_Control.GetCameraMatrix();
		inv_view.NoTranslate();
		//inv_view.FastInvert();
		device->SetTransform(D3DTS_TEXTURE1, (D3DMATRIX *)&inv_view);
		// `把Model的第3層貼圖強迫改成cubemap`
		CGutModel_DX9::SetTextureOverwrite(2, g_pTexture);
		CGutModel_DX9::SetUVMapOverwrite(2, MAP_CUBEMAP);
		// `太陽`
		g_Models_DX9[0].Render();
		// `把Model的第3層貼圖還原成模型原始設定`
		CGutModel_DX9::SetTextureOverwrite(2, NULL);
		CGutModel_DX9::SetUVMapOverwrite(2, 0xff);
		// `行星`
		RenderSolarSystemDX9();
	}

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 

	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
