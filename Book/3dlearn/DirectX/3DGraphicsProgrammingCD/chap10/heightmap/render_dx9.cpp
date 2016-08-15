#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutFont_DX9.h"
#include "GutHeightmap_DX9.h"

#include "render_data.h"

static CGutFontDX9 g_Font;
static CGutHeightmapDX9 g_Heightmap;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_Font.SetFontSize(32, 32);
	g_Font.SetFontAdvance(20, 30);
	g_Font.SetConsoleSize(32, 16);
	// 載入字型
	g_Font.LoadTexture("../../textures/ascii_couriernew.tga");
	// 載入heightmap
	if ( !g_Heightmap.LoadHeightmapTexture("../../textures/GrandCanyon.tga") )
		return false;
	g_Heightmap.SetRange(Vector4(-50.0f, -50.0f, -5.0f), Vector4(50.0f, 50.0f, 10.0f));

	D3DCAPS9 caps;
	device->GetDeviceCaps(&caps);

	// 某些舊的硬體一口氣不能畫太多頂點
	int verts = caps.MaxPrimitiveCount < 65536 ? 100 : 255;

	if ( !g_Heightmap.BuildMesh(verts, verts) )
		return false;

	Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.5f, 1000.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&proj_matrix);

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
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 	
	g_Font.SetConsoleResolution(width, height);
	g_Font.BuildMesh();

	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	Matrix4x4 proj_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &proj_matrix);
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
static void SetupLighting(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// 設定環境光
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));
	// 指定Diffuse材質的資料來源, 把它設為經由SetMaterial函式中來設定
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);
	device->SetRenderState(D3DRS_NORMALIZENORMALS, TRUE);

	D3DCOLORVALUE vFullIntensity = {1.0f, 1.0f, 1.0f, 1.0f};
	D3DCOLORVALUE vZeroIntensity = {0.0f, 0.0f, 0.0f, 0.0f};

	// 設定物件材質的反光能力

	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.Ambient = vFullIntensity;
	mtrl.Diffuse = vFullIntensity;
	mtrl.Specular = vFullIntensity;
	mtrl.Emissive = vZeroIntensity;
	mtrl.Power = g_fMaterialShininess;

	// 呼叫SetMaterial來設定材質.
	device->SetMaterial(&mtrl);
	// 設定光源
	device->LightEnable(0, TRUE);
	//
	D3DLIGHT9 light;
	ZeroMemory( &light, sizeof(light) );

	light.Type = D3DLIGHT_DIRECTIONAL;

	Vector4 LightDir = -g_Lights[0].m_Direction;
	light.Direction = *(D3DVECTOR *) &LightDir;
	light.Diffuse = *(D3DCOLORVALUE*) &g_Lights[0].m_Diffuse;
	light.Specular = *(D3DCOLORVALUE*) &g_Lights[0].m_Specular;
	light.Range = 1000.0f;

	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.0f;
	light.Attenuation2 = 0.0f;

	device->SetLight(0, &light);
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// 開始下繪圖指令
	device->BeginScene(); 

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *)&view_matrix);
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *)&world_matrix);
	//
	SetupLighting();
	//
	device->SetRenderState(D3DRS_FILLMODE, g_bWireframe ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
	g_Heightmap.Render();
	//
	device->EndScene(); 
	// 把背景backbuffer呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
