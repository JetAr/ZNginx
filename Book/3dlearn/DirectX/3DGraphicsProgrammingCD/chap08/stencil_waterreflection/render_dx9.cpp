#include "Gut.h"
#include "render_data.h"
#include "GutModel_DX9.h"
#include "GutWin32.h"

CGutModel_DX9 g_Model_DX9;

static Matrix4x4 g_projection_matrix;

void InitStateDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
}

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

	InitStateDX9();

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model);

	return true;
}

bool ReleaseResourceDX9(void)
{
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

	InitStateDX9();
}

static void RenderModelDX9(bool mirror, Vector4 *pPlane)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	if ( mirror )
	{
		if ( g_bClip )
		{
			device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
			float clipplane[] = {0.0f, 0.0f, 1.0f, -g_mirror_z};
			device->SetClipPlane(0, clipplane);
		}

		Vector4 vEye = g_Control.GetCameraPosition();
		Vector4 vLookAt = g_Control.m_vLookAt;
		Vector4 vUp = g_Control.m_vUp;

		Vector4 mirror_eye = MirrorPoint(vEye, *pPlane);
		Vector4 mirror_lookat = MirrorPoint(vLookAt, *pPlane);
		Vector4 mirror_up = MirrorVector(vUp, *pPlane);

		Matrix4x4 temp_matrix = GutMatrixLookAtRH(mirror_eye, mirror_lookat, mirror_up);

		// 因為是鏡射, 在轉換到鏡頭座標系後要做個左右對調的動作.
		Matrix4x4 mirror_x;
		mirror_x.Identity();
		mirror_x.Scale(-1.0f, 1.0f, 1.0f);

		view_matrix = temp_matrix * mirror_x;

		// 右左對調後, 3角形的頂點排列順序會被反過來.
		GutSetDX9BackFace(D3DCULL_CCW);
	}
	else
	{
		if ( g_bClip )
		{
			// 啟動ClipPlane
			device->SetRenderState(D3DRS_CLIPPLANEENABLE, D3DCLIPPLANE0 );
			float clipplane[] = {0.0f, 0.0f, 1.0f, -g_mirror_z};
			device->SetClipPlane(0, clipplane);
		}

		GutSetDX9BackFace(D3DCULL_CW);
	}

	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

	g_Model_DX9.Render();

	GutSetDX9BackFace(D3DCULL_CW);
	device->SetRenderState(D3DRS_CLIPPLANEENABLE, FALSE);
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	Vector4 vPlane(0.0f, 0.0f, 1.0f, -g_mirror_z);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	device->BeginScene(); 
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 150, 255), 1.0f, 0);
	{
		// 畫出空間中的茶壼
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);
		RenderModelDX9(false, NULL);
	}
	// 畫出鏡面, 同時把stencil值設為1
	{	
		sModelMaterial_DX9 material;
		material.m_Material.Diffuse.r = material.m_Material.Diffuse.g = material.m_Material.Diffuse.b = material.m_Material.Diffuse.a = 0.0f;
		material.Submit();

		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_REPLACE);
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_STENCILREF, 1);

		Matrix4x4 identity_matrix = Matrix4x4::IdentityMatrix();
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &identity_matrix);
		// 畫出矩形
		device->SetFVF(D3DFVF_XYZ);
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
	}
	// 把鏡面范圍的ZBuffer清為1.0
	{
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0);
		int w, h; 
		GutGetWindowSize(w, h);
		D3DVIEWPORT9 viewport;
		viewport.X = viewport.Y = 0;
		viewport.Width = w; viewport.Height = h;
		viewport.MinZ = 1.0f; viewport.MaxZ = 1.0f;
		device->SetViewport(&viewport);
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));
		viewport.MinZ = 0.0f; viewport.MaxZ = 1.0f;
		device->SetViewport(&viewport);
		device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 
			D3DCOLORWRITEENABLE_RED | 
			D3DCOLORWRITEENABLE_GREEN |
			D3DCOLORWRITEENABLE_BLUE |
			D3DCOLORWRITEENABLE_ALPHA);
	}
	// 畫出反射的茶壼
	{
		RenderModelDX9(true, &vPlane);
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
