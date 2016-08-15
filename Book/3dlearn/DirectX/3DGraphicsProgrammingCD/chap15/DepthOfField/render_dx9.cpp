#include "render_data.h"

#include "Gut.h"
#include "GutDX9.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"
#include "GutModel_DX9.h"

LPD3DXEFFECT g_pEffect = NULL;
D3DXHANDLE g_pDrawImageShader = NULL;
D3DXHANDLE g_pBlurShader = NULL;
D3DXHANDLE g_pDoFShader = NULL;

LPD3DXEFFECT g_pLinearZEffect = NULL;
D3DXHANDLE g_pLinearZShader = NULL;

LPDIRECT3DTEXTURE9 g_pFrameBuffer[4] = {NULL, NULL};
LPDIRECT3DSURFACE9 g_pFrameSurface[4] = {NULL, NULL};

static sImageInfo g_DownSampledImageInfo;
static sImageInfo g_ImageInfo;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_inv_proj_matrix;

static CGutModel_DX9 g_ModelSun_DX9;
static CGutModel_DX9 g_ModelEarth_DX9;

bool ReInitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	int w, h;
	GutGetWindowSize(w, h);
	
	g_ImageInfo.m_iWidth = w;
	g_ImageInfo.m_iHeight = h;
	
	g_DownSampledImageInfo.m_iWidth = w/4;
	g_DownSampledImageInfo.m_iHeight = h/4;

	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, g_fNearZ, g_fFarZ);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *)&g_proj_matrix);

	int width = w;
	int height = h;

	device->CreateTexture(width/4, height/4, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[0], NULL);
	
	device->CreateTexture(width/4, height/4, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[1], NULL);

	device->CreateTexture(width, height, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[2], NULL);

	device->CreateTexture(width, height, 1, 
		D3DUSAGE_RENDERTARGET, D3DFMT_R32F, 
		D3DPOOL_DEFAULT, &g_pFrameBuffer[3], NULL);

	if ( NULL==g_pFrameBuffer[0] || NULL==g_pFrameBuffer[1] || 
		 NULL==g_pFrameBuffer[2] || NULL==g_pFrameBuffer[2] )
		return false;

	g_pFrameBuffer[0]->GetSurfaceLevel(0, &g_pFrameSurface[0]);
	g_pFrameBuffer[1]->GetSurfaceLevel(0, &g_pFrameSurface[1]);
	g_pFrameBuffer[2]->GetSurfaceLevel(0, &g_pFrameSurface[2]);
	g_pFrameBuffer[3]->GetSurfaceLevel(0, &g_pFrameSurface[3]);

	for ( int i=0; i<4; i++ )
	{
		device->SetSamplerState(i,  D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(i,  D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(i,  D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

		device->SetSamplerState(i,  D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		device->SetSamplerState(i,  D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	return true;
}

bool InitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	CGutModel::SetTexturePath("../../textures/");

	if ( !g_ModelEarth_DX9.ConvertToDX9Model(&g_ModelEarth) )
		return false;
	if ( !g_ModelSun_DX9.ConvertToDX9Model(&g_ModelSun) )
		return false;

	g_pEffect = GutLoadFXShaderDX9("../../shaders/Posteffect.fx");
	if ( NULL==g_pEffect )
		return false;

	g_pDrawImageShader = g_pEffect->GetTechniqueByName("DrawImage");
	g_pBlurShader = g_pEffect->GetTechniqueByName("BlurImage");
	g_pDoFShader = g_pEffect->GetTechniqueByName("DoF");

	if ( NULL==g_pBlurShader || NULL==g_pDoFShader || NULL==g_pDrawImageShader )
		return false;

	g_pLinearZEffect = GutLoadFXShaderDX9("../../shaders/LinearZ.fx");
	if ( NULL==g_pLinearZEffect )
		return false;

	g_pLinearZShader = g_pLinearZEffect->GetTechniqueByName("LinearZ");
	if ( NULL==g_pLinearZShader )
		return false;

	if ( !ReInitResourceDX9() )
		return false;

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pEffect);
	SAFE_RELEASE(g_pLinearZEffect);

	for ( int i=0; i<4; i++ )
	{
		SAFE_RELEASE(g_pFrameSurface[i]);
		SAFE_RELEASE(g_pFrameBuffer[i]);
	}

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	for ( int i=0; i<4; i++ )
	{
		SAFE_RELEASE(g_pFrameSurface[i]);
		SAFE_RELEASE(g_pFrameBuffer[i]);
	}

	// Reset Device
	GutResetGraphicsDeviceDX9();
	// 取得Direct3D 9裝置
	ReInitResourceDX9();
}

LPDIRECT3DTEXTURE9 StretchImage(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	
	LPDIRECT3DSURFACE9 pRenderTargetBackup, pDepthStencilBackup;
	device->GetRenderTarget(0, &pRenderTargetBackup);
	device->GetDepthStencilSurface(&pDepthStencilBackup);

	device->SetRenderTarget(0, g_pFrameSurface[1]);
	device->SetDepthStencilSurface(NULL);

	// 計算貼圖像素的貼圖座標間距
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

	// Direct3D9會偏移半個像素, 從貼圖座標來把它修正回來
	Vector4 vTexoffset(fTexelW*0.5f, fTexelH*0.5f, 0.0f, 1.0f);

	Vertex_VT quad[4];
	memcpy(quad, g_FullScreenQuad, sizeof(quad));
	for ( int i=0; i<4; i++ )
	{
		quad[i].m_Texcoord[0] += vTexoffset[0];
		quad[i].m_Texcoord[1] += vTexoffset[1];
	}

	// 使用改變亮度的Shader
	g_pEffect->SetTechnique(g_pDrawImageShader);
	D3DXHANDLE pTextureVar = g_pEffect->GetParameterByName(NULL, "Image");
	
	g_pEffect->SetTexture(pTextureVar, pSource);

	// 設定頂點資料格式	
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);

	g_pEffect->Begin(NULL, 0);
	g_pEffect->BeginPass(0);
	
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex_VT));
	
	g_pEffect->EndPass();
	g_pEffect->End();

	device->SetRenderTarget(0, pRenderTargetBackup);
	device->SetDepthStencilSurface(pDepthStencilBackup);
	
	SAFE_RELEASE(pRenderTargetBackup);
	SAFE_RELEASE(pDepthStencilBackup);

	return g_pFrameBuffer[1];
}

LPDIRECT3DTEXTURE9 BlurImageDX9(LPDIRECT3DTEXTURE9 pSource, sImageInfo *pInfo)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// 計算貼圖像素的貼圖座標間距
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

	// Direct3D9會偏移半個像素, 從貼圖座標來把它修正回來
	Vector4 vTexoffset(fTexelW*0.5f, fTexelH*0.5f, 0.0f, 1.0f);

	Vertex_VT quad[4];
	memcpy(quad, g_FullScreenQuad, sizeof(quad));
	for ( int i=0; i<4; i++ )
	{
		quad[i].m_Texcoord[0] += vTexoffset[0];
		quad[i].m_Texcoord[1] += vTexoffset[1];
	}

	const int num_samples = KERNELSIZE;
	Vector4 vTexOffsetX[num_samples];
	Vector4 vTexOffsetY[num_samples];
	// 計算出隣近像素的貼圖座標間距
	for ( int i=0; i<num_samples; i++ )
	{
		vTexOffsetX[i].Set(g_uv_offset_table[i] * fTexelW, 0.0f, 0.0f, g_weight_table[i]);
		vTexOffsetY[i].Set(0.0f, g_uv_offset_table[i] * fTexelH, 0.0f, g_weight_table[i]);
	}

	LPDIRECT3DSURFACE9 pRenderTargetBackup, pDepthStencilBackup;
	device->GetRenderTarget(0, &pRenderTargetBackup);
	device->GetDepthStencilSurface(&pDepthStencilBackup);

	// 使用模糊化Shader
	g_pEffect->SetTechnique(g_pBlurShader);
	
	D3DXHANDLE pTexOffsetVar = g_pEffect->GetParameterByName(NULL, "vTexOffset");
	D3DXHANDLE pTextureVar = g_pEffect->GetParameterByName(NULL, "Image");

	// 設定頂點資料格式	
	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
	
	// 水平方向模糊
	{
		g_pEffect->SetVectorArray(pTexOffsetVar, (D3DXVECTOR4*)vTexOffsetX, num_samples);
		g_pEffect->SetTexture(pTextureVar, pSource);
		g_pEffect->Begin(NULL, 0);
		g_pEffect->BeginPass(0);

		device->SetRenderTarget(0, g_pFrameSurface[0]);
		device->SetDepthStencilSurface(NULL);
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex_VT));

		g_pEffect->EndPass();
		g_pEffect->End();
	}
	// 垂直方向模糊
	{
		g_pEffect->SetVectorArray(pTexOffsetVar, (D3DXVECTOR4*)&vTexOffsetY[0][0], num_samples);
		g_pEffect->SetTexture(pTextureVar, g_pFrameBuffer[0]);
		g_pEffect->Begin(NULL, 0);
		g_pEffect->BeginPass(0);

		device->SetRenderTarget(0, g_pFrameSurface[1]);
		device->SetTexture(0, g_pFrameBuffer[0]);
		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, quad, sizeof(Vertex_VT));

		g_pEffect->EndPass();
		g_pEffect->End();
	}

	device->SetRenderTarget(0, pRenderTargetBackup);
	device->SetDepthStencilSurface(pDepthStencilBackup);
	
	SAFE_RELEASE(pRenderTargetBackup);
	SAFE_RELEASE(pDepthStencilBackup);

	return g_pFrameBuffer[1];
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

//
void SetupLightingDX9(void)
{
	// ¨ú±oDirect3D 9¸Ë¸m
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	return;
	//
	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// ³]©wÀô¹Ò¥ú
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));
	// «ü©wDiffuse§÷½èªº¸ê®Æ¨Ó·½, §â¥¦³]¬°¸g¥ÑSetMaterial¨ç¦¡¤¤¨Ó³]©w
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	// ³]©w¥ú·½
	device->LightEnable(0, TRUE);
	GutSetupLightDX9(0, g_Light0);
}

void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->BeginScene(); 

	device->SetRenderState(D3DRS_ZENABLE, TRUE);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 vp_matrix = view_matrix * g_proj_matrix;

	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&g_proj_matrix);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&view_matrix);

	LPDIRECT3DSURFACE9 pMainFrameBuffer, pMainDepthBuffer;
	device->GetRenderTarget(0, &pMainFrameBuffer); pMainFrameBuffer->Release();
	device->GetDepthStencilSurface(&pMainDepthBuffer); pMainDepthBuffer->Release();

	// linear Z pass
	if ( g_bPosteffect )
	{
		device->SetRenderTarget(0, g_pFrameSurface[3]);
		device->SetDepthStencilSurface(pMainDepthBuffer);
		device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.0f, 0);

		device->SetRenderState(D3DRS_ZENABLE, TRUE);
		//device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		D3DXHANDLE hWvp_matrix = g_pLinearZEffect->GetParameterByName(NULL, "wvp_matrix");
		D3DXHANDLE hWv_matrix = g_pLinearZEffect->GetParameterByName(NULL, "wv_matrix");

		Matrix4x4 wvp_matrix = g_earth_matrix * vp_matrix;
		Matrix4x4 wv_matrix = g_earth_matrix * view_matrix;

		g_pLinearZEffect->SetTechnique("LinearZ");
		g_pLinearZEffect->SetMatrix(hWvp_matrix, (D3DXMATRIX*)&wvp_matrix);
		g_pLinearZEffect->SetMatrix(hWv_matrix, (D3DXMATRIX*)&wv_matrix);

		g_pLinearZEffect->Begin(NULL, 0);
		g_pLinearZEffect->BeginPass(0);

		g_ModelEarth_DX9.Render(0);

		g_pLinearZEffect->EndPass();
		g_pLinearZEffect->End();


		wvp_matrix = g_sun_matrix * vp_matrix;
		wv_matrix = g_sun_matrix * view_matrix;

		g_pLinearZEffect->SetTechnique("LinearZ");
		g_pLinearZEffect->SetMatrix(hWvp_matrix, (D3DXMATRIX*)&wvp_matrix);
		g_pLinearZEffect->SetMatrix(hWv_matrix, (D3DXMATRIX*)&wv_matrix);

		g_pLinearZEffect->Begin(NULL, 0);
		g_pLinearZEffect->BeginPass(0);

		g_ModelSun_DX9.Render(0);

		g_pLinearZEffect->EndPass();
		g_pLinearZEffect->End();

		device->SetRenderTarget(0, g_pFrameSurface[2]);
		device->SetDepthStencilSurface(pMainDepthBuffer);
	}

	// normal pass
	{
		device->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 0x0, 1.0f, 0);
		//device->Clear(0, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0);
	
		device->SetPixelShader(NULL);
		device->SetVertexShader(NULL);

		SetupLightingDX9();

		device->SetRenderState(D3DRS_ZENABLE, TRUE);
		device->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&g_earth_matrix);
		g_ModelEarth_DX9.Render();

		device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&g_sun_matrix);
		g_ModelSun_DX9.Render();
	}

	// downsampling

	if ( g_bPosteffect )
	{
		LPDIRECT3DTEXTURE9 pTexture = StretchImage(g_pFrameBuffer[2], &g_ImageInfo);
		pTexture = BlurImageDX9(pTexture, &g_DownSampledImageInfo);

		device->SetRenderTarget(0, pMainFrameBuffer);
		device->SetDepthStencilSurface(pMainDepthBuffer);

		Vector4 vZInv = g_inv_proj_matrix.GetColumn(3);

		D3DXHANDLE FramebufferVar = g_pEffect->GetParameterByName(NULL, "FramebufferTexture");
		D3DXHANDLE ZBufferVar = g_pEffect->GetParameterByName(NULL, "ZBufferTexture");
		D3DXHANDLE BlurtextureVar = g_pEffect->GetParameterByName(NULL, "BlurbufferTexture");
		D3DXHANDLE DepthOfFieldVar = g_pEffect->GetParameterByName(NULL, "vDepthOfField");
		D3DXHANDLE ZInvVar = g_pEffect->GetParameterByName(NULL, "vZInv");

		g_pEffect->SetTexture(FramebufferVar, g_pFrameBuffer[2]);
		g_pEffect->SetTexture(ZBufferVar, g_pFrameBuffer[3]);
		g_pEffect->SetTexture(BlurtextureVar, pTexture);

		g_pEffect->SetVector(DepthOfFieldVar, (D3DXVECTOR4*)&g_vDepthOfField);
		g_pEffect->SetVector(ZInvVar, (D3DXVECTOR4*)&vZInv);

		g_pEffect->SetTechnique(g_pDoFShader);

		g_pEffect->Begin(NULL, 0);
		g_pEffect->BeginPass(0);

		device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_FullScreenQuad, sizeof(Vertex_VT));

		g_pEffect->EndPass();
		g_pEffect->End();
	}

	device->EndScene();
	device->Present( NULL, NULL, NULL, NULL );
}
