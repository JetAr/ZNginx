#include "Gut.h"
#include "render_data.h"
#include "GutModel_DX9.h"

CGutModel_DX9 g_Model_DX9;
CGutModel_DX9 g_Terrain_DX9;

LPDIRECT3DTEXTURE9 g_pTexture = NULL;
LPDIRECT3DSURFACE9 g_pDepthStencil = NULL;

static Matrix4x4 g_projection_matrix;

const int g_framebuffer_w = 256;
const int g_framebuffer_h = 256;

LPDIRECT3DTEXTURE9 g_pBlurTextures[2];

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

	// 配置動態貼圖
	{
		device->CreateTexture(g_framebuffer_w, g_framebuffer_h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pTexture, NULL);
		device->CreateDepthStencilSurface(g_framebuffer_w, g_framebuffer_h, D3DFMT_D24S8, D3DMULTISAMPLE_NONE, 0, FALSE, &g_pDepthStencil, NULL);

		device->CreateTexture(g_framebuffer_w, g_framebuffer_h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pBlurTextures[0], NULL);
		device->CreateTexture(g_framebuffer_w, g_framebuffer_h, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &g_pBlurTextures[1], NULL);
	}

	// 載入模型
	{
		CGutModel::SetTexturePath("../../textures/");

		g_Model_DX9.ConvertToDX9Model(&g_Model);
		g_Terrain_DX9.ConvertToDX9Model(&g_Terrain);
	}

	// 設定貼圖內插方法
	{
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

		device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pDepthStencil);

	SAFE_RELEASE(g_pBlurTextures[0]);
	SAFE_RELEASE(g_pBlurTextures[1]);

	g_Model_DX9.Release();
	g_Terrain_DX9.Release();

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
	// 使用自動normalize功能
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

	device->SetSamplerState(1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(1, D3DSAMP_MIPFILTER, D3DTEXF_NONE);

}

static LPDIRECT3DTEXTURE9 BlurTexture(LPDIRECT3DTEXTURE9 pSource)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	Matrix4x4 identiyMatrix = Matrix4x4::IdentityMatrix();

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &identiyMatrix);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &identiyMatrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &identiyMatrix);

	LPDIRECT3DSURFACE9 pFrameBufferBackup, pDepthBufferBackup;
	device->GetRenderTarget(0, &pFrameBufferBackup); pFrameBufferBackup->Release();
	device->GetDepthStencilSurface(&pDepthBufferBackup); pDepthBufferBackup->Release();
	device->SetRenderState(D3DRS_CULLMODE , D3DCULL_NONE);

	float weight_table[7] = {1.0f, 3.0f, 4.0f, 5.0f, 4.0f, 3.0f, 1.0f};
	float uv_offset_table[7] = {-3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f};

	float sum = 0.0f;

	for ( int i=0; i<7; i++ )
	{
		sum += weight_table[i];
	}

	for ( int i=0; i<7; i++ )
	{
		weight_table[i] /= sum;
	}

	Vector4 vOneTexel_X(1.0f/(float)g_framebuffer_w, 0.0f, 0.0f, 0.0f);
	Vector4 vOneTexel_Y(0.0f, 1.0f/(float)g_framebuffer_h, 0.0f, 0.0f);
	Vertex_VT temp_quad[4];

	LPDIRECT3DSURFACE9 pSurface;
	g_pBlurTextures[0]->GetSurfaceLevel(0, &pSurface); 

	device->SetRenderTarget(0, pSurface);
	device->SetDepthStencilSurface(NULL);
	device->SetRenderState(D3DRS_ZENABLE, FALSE);
	device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);

	device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
	device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	device->SetTexture(0, pSource);
	device->SetFVF(D3DFVF_XYZW|D3DFVF_TEX1); 

	// 設定顏色來源為 texture factor * texture
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);

	// X方向blur
	for ( int i=0; i<7; i++ )
	{
		Vector4 vOffset = uv_offset_table[i] * vOneTexel_X;

		for ( int p=0; p<4; p++ )
		{
			temp_quad[p].m_Position = g_Quad[p].m_Position;
			temp_quad[p].m_Texcoord = g_Quad[p].m_Texcoord + vOffset;
		}

		int weight = (int) (weight_table[i] * 255);
		device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(weight, weight, weight, weight));
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, temp_quad, sizeof(Vertex_VT));
	}

	pSurface->Release();

	g_pBlurTextures[1]->GetSurfaceLevel(0, &pSurface); 
	device->SetRenderTarget(0, pSurface);
	device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_RGBA(0, 0, 0, 0), 1.0f, 0);
	device->SetTexture(0, g_pBlurTextures[0]);

	// Y方向blur
	for ( int i=0; i<7; i++ )
	{
		Vector4 vOffset = uv_offset_table[i] * vOneTexel_Y;

		for ( int p=0; p<4; p++ )
		{
			temp_quad[p].m_Position = g_Quad[p].m_Position;
			temp_quad[p].m_Texcoord = g_Quad[p].m_Texcoord + vOffset;
		}

		int weight = (int) (weight_table[i] * 255);
		device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(weight, weight, weight, weight));
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, temp_quad, sizeof(Vertex_VT));
	}

	pSurface->Release();

	// 還原設定
	device->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(255, 255, 255, 255));
	device->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	device->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	device->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	device->SetRenderState(D3DRS_ZENABLE, TRUE);
	device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

	device->SetRenderTarget(0, pFrameBufferBackup);
	device->SetDepthStencilSurface(pDepthBufferBackup);

	return g_pBlurTextures[1];
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

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	LPDIRECT3DTEXTURE9 pBlurredTexture = NULL;

	Matrix4x4 light_projection_matrix;
	Matrix4x4 light_view_matrix;

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	// 開始下繪圖指令
	device->BeginScene(); 
	{
		// 保存主framebuffer
		LPDIRECT3DSURFACE9 pFrameBufferBackup, pDepthBufferBackup;
		device->GetRenderTarget(0, &pFrameBufferBackup); pFrameBufferBackup->Release();
		device->GetDepthStencilSurface(&pDepthBufferBackup); pDepthBufferBackup->Release();
		// 取出動態貼圖中的surface
		LPDIRECT3DSURFACE9 pSurface;
		g_pTexture->GetSurfaceLevel(0, &pSurface); 
		// 把繪圖結果輸出到動態貼圖中
		device->SetRenderTarget(0, pSurface);
		device->SetDepthStencilSurface(g_pDepthStencil);
		// 清除畫面
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(255, 255, 255, 255), 1.0f, 0);
		// 設定光源位置
		Vector4 vLightPos = g_Light.m_Position;
		Vector4 vLightUp(0.0f, 1.0f, 0.0f);
		Vector4 vLightLookat(0.0f, 0.0f, 0.0f);

		light_projection_matrix = GutMatrixPerspectiveRH_DirectX(60.0f, 1.0f, 0.1f, 100.0f);
		light_view_matrix = GutMatrixLookAtRH(vLightPos, vLightLookat, vLightUp);
		// 把鏡頭放到光源位置來畫陰影
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&light_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&light_view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&world_matrix);

		// 把所有反射關閉，讓模型在畫面上呈現黑色。
		D3DCOLORVALUE zero = {0.0f, 0.0f, 0.0f, 1.0f};

		sModelMaterial_DX9 material;
		material.m_Material.Ambient = zero;
		material.m_Material.Emissive = zero;
		material.m_Material.Diffuse = zero;
		material.m_Material.Specular = zero;
		material.m_bCullFace = false;

		material.Submit();

		SetupLightingDX9();
		// 畫出模型
		g_Model_DX9.Render(0);
		// 告知direct3d9裝置rendertarget使用完畢
		pSurface->Release();
		// 還原主framebuffer
		device->SetRenderTarget(0, pFrameBufferBackup);
		device->SetDepthStencilSurface(pDepthBufferBackup);
	}

	// 把影子柔化
	{
		pBlurredTexture = BlurTexture(g_pTexture);
	}

	// 把上一個步驟的結果當成貼圖來使用
	{
		// 消除畫面
		device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 150, 255), 1.0f, 0);
		// 設定轉換矩陣
		device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
		device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);
		// 設定光源
		SetupLightingDX9();

		device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

		// 畫出茶壼
		g_Model_DX9.Render();

		sModelMaterial_DX9 material;
		material.m_pTextures[0] = pBlurredTexture;
		material.Submit();
		// 計算貼圖矩陣
		Matrix4x4 inv_view_matrix = g_Control.GetCameraMatrix();
		Matrix4x4 uv_offset_matrix;
		uv_offset_matrix.Identity();
		uv_offset_matrix.Scale(0.5f, -0.5f, 0.5f);
		uv_offset_matrix[3].Set(0.5f, 0.5f, 0.5f, 1.0f);

		Matrix4x4 texture_matrix = inv_view_matrix * light_view_matrix * light_projection_matrix * uv_offset_matrix;
		Matrix4x4 indent_matrix = Matrix4x4::IdentityMatrix();
		// 設定轉換矩陣
		device->SetTransform(D3DTS_TEXTURE0, (D3DMATRIX *) &texture_matrix);
		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &indent_matrix);
		// 開啟自動產生貼圖座標功能
		device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION);
		device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT4 | D3DTTFF_PROJECTED);

		device->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		device->SetSamplerState(1, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(1, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		// 畫出地表
		g_Terrain_DX9.Render(0);

		device->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
		device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	}

	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 

	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
