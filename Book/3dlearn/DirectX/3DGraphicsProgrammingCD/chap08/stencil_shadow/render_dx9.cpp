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
	//
	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
}

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFOV, 1.0f, 0.1f, 100.0f);

	InitStateDX9();

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model_textured);

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

// 使用Direct3D9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 ident_matrix; ident_matrix.Identity();
	device->BeginScene(); 
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);
	// light position & orientation
	Vector4 light_pos(5.0f, 0.0f, 5.0f);
	Vector4 light_lookat(0.0f, 0.0f, 0.0f);
	Vector4 light_up(0.0f, 1.0f, 0.0f);
	// light matrix
	Matrix4x4 light_view = GutMatrixLookAtRH(light_pos, light_lookat, light_up);
	Matrix4x4 light_world_view = world_matrix * light_view;
	Matrix4x4 shadow_matrix;
	// 建立shadow volume
	if ( g_bDirectionalLight )
	{
		g_ShadowVolume.BuildShadowVolume(light_world_view, ident_matrix, 20.0f);
		shadow_matrix = light_view;
		shadow_matrix.FastInvert();
	}
	else
	{
		Matrix4x4 light_proj = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 1.0f, 100.0f);
		g_ShadowVolume.BuildShadowVolume(light_world_view, light_proj, 20.0f);
		shadow_matrix = light_view;
		shadow_matrix.FastInvert();
	}
	// 畫出箱子
	{
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

		g_Model_DX9.Render();
	}
	// 畫出墻壁
	{
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &ident_matrix);

		sModelMaterial_DX9 material;

		material.m_Material.Diffuse.r = 0.0f;
		material.m_Material.Diffuse.g = 0.0f;
		material.m_Material.Diffuse.b = 1.0f;
		material.m_Material.Diffuse.a = 1.0f;

		material.Submit();

		device->SetFVF(D3DFVF_XYZ);
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_V3T2));
	}
	device->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
	// 在Stencil Buffer上標示出陰影區域
	{
		sModelMaterial_DX9 material;
		material.Submit();
		// 套用矩陣
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &shadow_matrix);
		// 設定頂點資料格式
		device->SetFVF(D3DFVF_XYZ);
		// 不更新zbuffer跟framebuffer
		device->SetRenderState(D3DRS_COLORWRITEENABLE, FALSE);
		// 只更新Stenil Buffer
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
		// 畫出背對鏡頭的面, 同時遞減Stencil Buffer.
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_DECR);
		device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, g_ShadowVolume.m_iNumFaces, g_ShadowVolume.m_pShadowVolume, sizeof(Vector4));
		// 畫出面對鏡頭的面, 同時遞增Stencil Buffer.
		device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
		device->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCR);
		device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, g_ShadowVolume.m_iNumFaces, g_ShadowVolume.m_pShadowVolume, sizeof(Vector4));
		// 恢復更新framebuffer
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xff);
	}
	// 畫出陰影
	{
		sModelMaterial_DX9 material;
		material.m_bCullFace = false;
		material.Submit();

		device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
		device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		// 套用矩陣
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &ident_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &ident_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &ident_matrix);
		// 只更新stencil buffer上值為非 0 的像素
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_NOTEQUAL);
		device->SetRenderState(D3DRS_STENCILREF, 0x0);
		// 使用黑色再畫一次墻壁
		device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(0, 0, 0,255) );
		device->SetFVF(D3DFVF_XYZ);
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_FullScreenQuad, sizeof(Vertex_V3T2));

		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
	// 觀察shadow volume, 除錯用.
	if ( g_bDrawShadowVolume )
	{
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &shadow_matrix);

		sModelMaterial_DX9 material;
		material.m_bCullFace = false;
		material.Submit();

		device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(255, 255, 255, 255) );
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);
		device->DrawPrimitiveUP(D3DPT_TRIANGLELIST, g_ShadowVolume.m_iNumFaces, g_ShadowVolume.m_pShadowVolume, sizeof(Vector4));
		device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
	// 恢復更新zbuffer
	device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	// 把顏色來源還原為 diffuse * texture
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
