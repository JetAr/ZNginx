#include "Gut.h"
#include "GutDX9.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"

#include "render_data.h"

LPDIRECT3DTEXTURE9 g_pTexture = NULL;
LPDIRECT3DVERTEXSHADER9 g_pVertexShader = NULL;
LPDIRECT3DPIXELSHADER9 g_pPixelShader = NULL;
LPDIRECT3DPIXELSHADER9 g_pColorTransformPS = NULL;

bool ReInitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetSamplerState(0,  D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0,  D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	device->SetSamplerState(0,  D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	device->SetSamplerState(0,  D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
	device->SetSamplerState(0,  D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

	device->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	return true;
}

bool InitResourceDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	if ( !ReInitResourceDX9() )
		return false;

	g_pTexture = GutLoadTexture_DX9("../../textures/lena.tga");
	if ( NULL==g_pTexture )
		return false;

	g_pVertexShader = GutLoadVertexShaderDX9_HLSL("../../shaders/Posteffect_grayscale.hlsl", "VS", "vs_2_0");
	g_pPixelShader = GutLoadPixelShaderDX9_HLSL("../../shaders/Posteffect_grayscale.hlsl", "PS", "ps_2_0");
	g_pColorTransformPS = GutLoadPixelShaderDX9_HLSL("../../shaders/Posteffect_colortransform.hlsl", "PS", "ps_2_0");
	if ( NULL==g_pVertexShader || NULL==g_pPixelShader || NULL==g_pColorTransformPS )
		return false;

	return true;
}

bool ReleaseResourceDX9(void)
{
	SAFE_RELEASE(g_pTexture);
	SAFE_RELEASE(g_pVertexShader);
	SAFE_RELEASE(g_pPixelShader);
	SAFE_RELEASE(g_pColorTransformPS);

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// 取得Direct3D 9裝置
	ReInitResourceDX9();
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	LPDIRECT3DTEXTURE9 pBlurredTexture;

	device->BeginScene(); 
	device->Clear(0, NULL, D3DCLEAR_TARGET, 0, 1.0f, 0);
	Matrix4x4 ident_mat; ident_mat.Identity();

	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	switch(g_iPosteffect)
	{
	default:
		device->SetPixelShader(NULL);
		device->SetVertexShader(NULL);
		break;
	case 2:
		device->SetPixelShader(g_pPixelShader);
		device->SetVertexShader(g_pVertexShader);
		device->SetVertexShaderConstantF(0, (float *)&ident_mat, 4);
		break;
	case 3:
		device->SetPixelShader(g_pColorTransformPS);
		device->SetVertexShader(g_pVertexShader);
		device->SetVertexShaderConstantF(0, (float *)&ident_mat, 4);
		device->SetPixelShaderConstantF(0, (float *)&g_SepiaMatrix, 4);
		break;
	case 4:
		device->SetPixelShader(g_pColorTransformPS);
		device->SetVertexShader(g_pVertexShader);
		device->SetVertexShaderConstantF(0, (float *)&ident_mat, 4);
		device->SetPixelShaderConstantF(0, (float *)&g_SaturateMatrix, 4);
		break;
	}

	device->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1);
	device->SetTexture(0, g_pTexture);

	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_FullScreenQuad, sizeof(Vertex_VT));

	device->EndScene(); 

	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
