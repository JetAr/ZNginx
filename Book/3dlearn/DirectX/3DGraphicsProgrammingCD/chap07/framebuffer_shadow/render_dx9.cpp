#include "Gut.h"
#include "render_data.h"
#include "GutModel_DX9.h"

CGutModel_DX9 g_Model_DX9;

LPDIRECT3DTEXTURE9 g_pTexture = NULL;
LPDIRECT3DSURFACE9 g_pDepthStencil = NULL;

static Matrix4x4 g_projection_matrix;

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

	CGutModel::SetTexturePath("../../textures/");
	g_Model_DX9.ConvertToDX9Model(&g_Model);

	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

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
	//device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

}

D3DCOLOR ConvertToD3DCOLOR(Vector4 &vColor)
{
	Vector4 vColor_0_255 = vColor;
	vColor_0_255.Saturate();
	vColor_0_255 *= 255.0f;

	int r = (int) vColor_0_255.GetX();
	int g = (int) vColor_0_255.GetY();
	int b = (int) vColor_0_255.GetZ();
	int a = (int) vColor_0_255.GetW();

	D3DCOLOR d3dcolor = D3DCOLOR_ARGB(a, r, g, b);
	return d3dcolor;
}

void SetupLightingDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// 設定環境光
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));
	// 設定光源
	device->LightEnable(0, TRUE);

	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(light) );

	light.Type = D3DLIGHT_POINT;

	light.Position = *(D3DVECTOR *) &g_Light.m_Position;
	light.Diffuse = *(D3DCOLORVALUE*) &g_Light.m_Diffuse;
	light.Specular = *(D3DCOLORVALUE*) &g_Light.m_Specular;
	light.Range = 100.0f;

	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	device->SetLight(0, &light);

}

// `使用Direct3D9來繪圖`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 light_projection_matrix;
	Matrix4x4 light_view_matrix;

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	// `開始下繪圖指令`
	device->BeginScene(); 

	// `把剪影畫在動態貼圖中`
	if(1)
	{
		//! 设置临时framebuffer
		LPDIRECT3DSURFACE9 pFrameBufferBackup, pDepthBufferBackup;
		device->GetRenderTarget(0, &pFrameBufferBackup); 
		pFrameBufferBackup->Release();
		device->GetDepthStencilSurface(&pDepthBufferBackup); 
		pDepthBufferBackup->Release();

		LPDIRECT3DSURFACE9 pSurface;
		g_pTexture->GetSurfaceLevel(0, &pSurface); 

		device->SetRenderTarget(0, pSurface);
		device->SetDepthStencilSurface(g_pDepthStencil);

		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);

		Vector4 vLightPos = g_Light.m_Position;
		Vector4 vLightUp(0.0f, 1.0f, 0.0f);
		Vector4 vLightLookat(0.0f, 0.0f, 0.0f);
		// `把鏡頭放在光源的`
		light_projection_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, 0.1f, 100.0f);
		light_view_matrix = GutMatrixLookAtRH(vLightPos, vLightLookat, vLightUp);
		// `設定轉換矩陣`
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&light_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&light_view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&world_matrix);

		D3DCOLORVALUE zero = {0.0f, 0.0f, 0.0f, 1.0f};

		sModelMaterial_DX9 material;
		material.m_Material.Ambient = zero;
		material.m_Material.Emissive = zero;
		material.m_Material.Diffuse = zero;
		material.m_Material.Specular = zero;
		material.m_bCullFace = false;

		material.Submit();

		device->SetRenderState(D3DRS_LIGHTING, 0);

		g_Model_DX9.Render(0);

		pSurface->Release();
		device->SetRenderTarget(0, pFrameBufferBackup);
		device->SetDepthStencilSurface(pDepthBufferBackup);
	}

	// `把上一個步驟的結果當成貼圖來使用`
	{
		// `消除畫面`
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 150, 255), 1.0f, 0);
		// `設定轉換矩陣`
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);


#define DRAW_TEAPORT_FIRST 1

		//! 画茶壶
		if (DRAW_TEAPORT_FIRST)
		{
			SetupLightingDX9();
			g_Model_DX9.Render();
		}

		//! 设置矩形的纹理
		sModelMaterial_DX9 material;
		material.m_pTextures[0] = g_pTexture;
		material.Submit();

		Matrix4x4 inv_view_matrix = view_matrix;
		inv_view_matrix.FastInvert();

		Matrix4x4 uv_offset_matrix;
		uv_offset_matrix.Identity();
		uv_offset_matrix.Scale(0.5f, -0.5f, 0.5f);
		uv_offset_matrix[3].Set(0.5f, 0.5f, 0.5f, 1.0f);

		Matrix4x4 texture_matrix = inv_view_matrix * light_view_matrix * light_projection_matrix * uv_offset_matrix;
		Matrix4x4 indent_matrix = Matrix4x4::IdentityMatrix();

		device->SetTransform(D3DTS_TEXTURE0, (D3DMATRIX *) &texture_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &indent_matrix);

		device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 |  D3DTTFF_PROJECTED );
		device->SetRenderState(D3DRS_LIGHTING, FALSE);
		// `畫出矩形`
		device->SetFVF(D3DFVF_XYZ);
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad, sizeof(Vertex_VT));

		//! 关闭纹理矩阵
		device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

		if (!DRAW_TEAPORT_FIRST)
		{
			//SetupLightingDX9();
			device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);
			g_Model_DX9.Render();
		}


	}
	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 
	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
