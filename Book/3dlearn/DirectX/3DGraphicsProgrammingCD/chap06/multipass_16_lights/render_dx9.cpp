#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutTexture_DX9.h"
#include "GutModel_DX9.h"

#include "render_data.h"

static Matrix4x4 g_projection_matrix;

CGutModel_DX9 g_Model_DX9;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);

	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	CGutModel::SetTexturePath("../../textures/");

	for ( int t=0; t<MAX_NUM_TEXTURES; t++ )
	{
		// trilinear filter
		device->SetSamplerState(t, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(t, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		device->SetSamplerState(t, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);
	}

	g_Model_DX9.ConvertToDX9Model(&g_Model);

	return true;
}

bool ReleaseResourceDX9(void)
{

	return true;
}

void ResizeWindowDX9(int width, int height)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// Reset Device
	GutResetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	int w, h;
	GutGetWindowSize(w, h);
	float aspect = (float) h / (float) w;
	g_projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &g_projection_matrix);
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

void SetupLightingDX9(Light *pLightArray, int num_lights)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// 設定環境光
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));

	int i;

	for ( i=0; i<num_lights; i++ )
	{
		// 設定光源
		device->LightEnable(i, TRUE);

		D3DLIGHT9 light;
		ZeroMemory( &light, sizeof(light) );

		light.Type = D3DLIGHT_POINT;

		light.Position = *(D3DVECTOR *) &pLightArray[i].m_Position;
		light.Diffuse = *(D3DCOLORVALUE*) &pLightArray[i].m_Diffuse;
		light.Specular = *(D3DCOLORVALUE*) &pLightArray[i].m_Specular;
		light.Range = 1000.0f;

		light.Attenuation0 = 0.0f;
		light.Attenuation1 = 2.0f;
		light.Attenuation2 = 0.0f;

		device->SetLight(i, &light);
	}

}

// 使用DirectX9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);

	// `把隱藏面測試規則設為小於或等於` <=
	device->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);

	// `設定轉換矩陣`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &world_matrix);

	// `開始下繪圖指令`
	device->BeginScene(); 

	// `前8盞光`
	if ( g_iSwitch & 0x01 )
	{
		SetupLightingDX9(&g_Lights[0], 8);
		g_Model_DX9.Render();
	}

	// `後8盞光`
	if ( g_iSwitch & 0x02 )
	{
		SetupLightingDX9(&g_Lights[8], 8);

		if ( g_iSwitch & 0x01 )
		{
			// 如果前8盞光有開, 要使用混色來把下8盞光疊加上去.
			device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
			device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
			device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
			// ~SUBMIT_BLEND 會取消套用檔案中的混色設定.
			g_Model_DX9.Render(~SUBMIT_BLEND);
		}
		else
		{
			g_Model_DX9.Render();
		}
	}


	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 

	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
