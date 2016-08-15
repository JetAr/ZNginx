#include "Gut.h"
#include "render_data.h"
#include "GutModel_DX9.h"

CGutModel_DX9 g_Model_DX9;

LPDIRECT3DTEXTURE9 g_pTexture = NULL;
LPDIRECT3DSURFACE9 g_pDepthStencil = NULL;

static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_mirror_view_matrix;

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
	// 使用自動normalize功能
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	device->CreateTexture(512, 512, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexture, NULL);
	device->CreateDepthStencilSurface(512, 512, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &g_pDepthStencil, NULL);

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model);

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pDepthStencil);
	g_Model_DX9.Release();

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
	// 使用自動normalize功能
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);
}

static void RenderModelDX9(bool mirror, Vector4 *pPlane)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 view_matrix;
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	if ( mirror )
	{
		Vector4 vEye = g_Control.GetCameraPosition();
		Vector4 vUp = g_Control.m_vUp;
		Vector4 vLookAt = g_Control.m_vLookAt;

		Vector4 mirror_eye = MirrorPoint(vEye, *pPlane);
		Vector4 mirror_lookat = MirrorPoint(vLookAt, *pPlane);
		Vector4 mirror_up = MirrorPoint(vUp, *pPlane);

		view_matrix = GutMatrixLookAtRH(mirror_eye, mirror_lookat, mirror_up);
		g_mirror_view_matrix = view_matrix;
	}
	else
	{
		view_matrix = g_Control.GetViewMatrix();
	}

	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

	g_Model_DX9.Render();
}

// `使用Direct3D9來繪圖`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	Vector4 vPlane(0.0f, 0.0f, 1.0f, -g_mirror_z);

	// `開始下繪圖指令`
	device->BeginScene(); 

	{
		LPDIRECT3DSURFACE9 pFrameBufferBackup, pDepthBufferBackup;
		device->GetRenderTarget(0, &pFrameBufferBackup); pFrameBufferBackup->Release();
		device->GetDepthStencilSurface(&pDepthBufferBackup); pDepthBufferBackup->Release();

		LPDIRECT3DSURFACE9 pSurface;
		g_pTexture->GetSurfaceLevel(0, &pSurface); 

		device->SetRenderTarget(0, pSurface);
		device->SetDepthStencilSurface(g_pDepthStencil);

		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 200, 255), 1.0f, 0);

		RenderModelDX9(true, &vPlane);

		pSurface->Release();
		device->SetRenderTarget(0, pFrameBufferBackup);
		device->SetDepthStencilSurface(pDepthBufferBackup);
	}

	// `把上一個步驟的結果當成貼圖來使用`
	{
		// `消除畫面`
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 150, 255), 1.0f, 0);

		RenderModelDX9(false, NULL);

		Matrix4x4 identMat; identMat.Identity();
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &identMat);

		sModelMaterial_DX9 material;
		material.m_pTextures[0] = g_pTexture;
		material.Submit();

		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);
		// `計算`texture matrix
		Matrix4x4 uv_offset_matrix;
		uv_offset_matrix.Scale_Replace(0.5f, -0.5f, 1.0f);
		uv_offset_matrix[3].Set(0.5f, 0.5f, 0.0f, 1.0f);
		Matrix4x4 texture_matrix = g_mirror_view_matrix * g_projection_matrix * uv_offset_matrix;

		struct VertexDX9
		{
			float m_Position[3];
			float m_Tex[4];
		};

		VertexDX9 vertex[4];

		// `計算貼圖座標, 也可以經由設定texture matrix來做.`
		for ( int i=0; i<4; i++ )
		{
			Vector4 vPosition(g_Quad[i].m_Position[0], g_Quad[i].m_Position[1], g_Quad[i].m_Position[2]);
			g_Quad[i].m_Texcoord = vPosition * texture_matrix;

			g_Quad[i].m_Position.StoreXYZ(vertex[i].m_Position);
			g_Quad[i].m_Texcoord.Store(vertex[i].m_Tex);
		}
		// `D3DTTFF_PROJECTED告知direct3d裝置texcoord需要除以w`
		device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4|D3DTTFF_PROJECTED);
		// `畫出矩形`
		device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1 | D3DFVF_TEXCOORDSIZE4(0) );
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex, sizeof(VertexDX9));

	}

	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 

	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
