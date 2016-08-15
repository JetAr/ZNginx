#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	Matrix4x4 projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &g_view_matrix);
	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	ConvertToDX9(g_Quad, g_Quad_dx9, 4);
	ConvertToDX9(g_LightPosition, g_LightPosition_dx9, NUM_LIGHTS);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	GutResetGraphicsDeviceDX9();
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_fOrthoWidth = g_fOrthoSize;
	g_fOrthoHeight = g_fOrthoSize;
	if ( aspect > 1.0f )
		g_fOrthoHeight *= aspect;
	else
		g_fOrthoWidth /= aspect;

	Matrix4x4 projection_matrix = GutMatrixOrthoRH_DirectX(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
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

static void SetupLighting(void)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `設定環境光`
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vAmbientLight));
	// `指定Diffuse材質的資料來源, 把它設為經由SetMaterial函式中來設定.`
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARMATERIALSOURCE, D3DMCS_MATERIAL);
	device->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	// `設定物件材質的反光能力`
	D3DCOLORVALUE vFullIntensity = {1.0f, 1.0f, 1.0f, 1.0f};
	D3DCOLORVALUE vZeroIntensity = {0.0f, 0.0f, 0.0f, 0.0f};

	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.Ambient = vFullIntensity;
	mtrl.Diffuse = vFullIntensity;
	mtrl.Specular = vFullIntensity;
	mtrl.Emissive = vZeroIntensity;
	mtrl.Power = g_fMaterialShininess;
	// `呼叫SetMaterial來設定材質`
	device->SetMaterial(&mtrl);

	// `把所找到的環境光套入硬體光源`
	int i;
	for ( i=0; i<g_iNumActivatedLights; i++ )
	{
		device->LightEnable(i, TRUE);

		D3DLIGHT9 light;
		ZeroMemory( &light, sizeof(light) );

		light.Type = D3DLIGHT_POINT;
		light.Position = *(D3DVECTOR *) &g_ActivatedLights[i].m_Position;
		light.Diffuse = *(D3DCOLORVALUE*) &g_ActivatedLights[i].m_Diffuse;
		light.Specular = *(D3DCOLORVALUE*) &g_ActivatedLights[i].m_Specular;
		light.Range = 1000.0f;

		light.Attenuation0 = 1.0f;
		light.Attenuation1 = 0.0f;
		light.Attenuation2 = 0.0f;

		device->SetLight(i, &light);
	}

	// `其余的燈關閉`
	for ( ; i<MAX_ACTIVATED_LIGHTS; i++ )
	{
		device->LightEnable(i, FALSE);
	}
}

// `使用Direct3D9來繪圖`
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// `消除畫面`
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// `開始下繪圖指令`
	device->BeginScene();
	// `頂點資料格式`
	device->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE);
	// `暫時關閉打光功能`
	device->SetRenderState(D3DRS_LIGHTING, FALSE);
	Matrix4x4 matIdentity; matIdentity.Identity();
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &matIdentity);
	// `點的大小`
	float fPointSize = 5.0f;
	// `把浮點數的記憶體資料強迫轉換成DWORD型態`
	DWORD dwPointSize = *((DWORD*)&fPointSize);
	// `設定畫點時使用5x5個像素來畫一個點`
	device->SetRenderState(D3DRS_POINTSIZE, dwPointSize);
	// `用點來畫出光源位置`
	device->DrawPrimitiveUP(D3DPT_POINTLIST, NUM_LIGHTS, g_LightPosition_dx9, sizeof(Vertex_DX9));
	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// `設定光源`
	SetupLighting();
	// `移動物件`
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
	// `畫一片矩形來看打光效果`
	device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, g_Quad_dx9, sizeof(Vertex_DX9));
	// `宣告所有的繪圖指令都下完了`
	device->EndScene(); 
	// `把背景backbuffer的畫面呈現出來`
	device->Present( NULL, NULL, NULL, NULL );
}
