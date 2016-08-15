#include "Gut.h"
#include "render_data.h"

bool InitResourceDX9(void)
{
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, 1.0f, 0.1f, 100.0f);
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	device->SetTransform(D3DTS_VIEW, (D3DMATRIX *) &view_matrix);
	// 畫出正向跟反向的三角形
	device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	return true;
}

bool ReleaseResourceDX9(void)
{
	return true;
}

void ResizeWindowDX9(int width, int height)
{
	GutResetGraphicsDeviceDX9();
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_DirectX(g_fFovW, aspect, 0.1f, 100.0f);
	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &projection_matrix);
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

void SetupLightingDX9(void)
{
	// `取得Direct3D9裝置`
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();

	// `開啟打光功能`
	device->SetRenderState(D3DRS_LIGHTING, TRUE);
	// `設定環境光`
	device->SetRenderState(D3DRS_AMBIENT, ConvertToD3DCOLOR(g_vGlobal_AmbientLight));
	// `指定Diffuse材質的資料來源, 把它設為從SetMaterial函式中來設定.`
	device->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
	// `設定物件材質的反光能力`
	D3DMATERIAL9 mtrl;
	ZeroMemory( &mtrl, sizeof(mtrl) );
	mtrl.Ambient = *(D3DCOLORVALUE*) &g_vMaterialAmbient;
	mtrl.Diffuse = *(D3DCOLORVALUE*) &g_vMaterialDiffuse;
	mtrl.Specular = *(D3DCOLORVALUE*) &g_vMaterialSpecular;
	mtrl.Emissive = *(D3DCOLORVALUE*) &g_vMaterialEmissive;
	// `經由呼叫SetMaterial來設定材質`
	device->SetMaterial(&mtrl);
	// `設定光源`
	for ( int i=0; i<g_iNumLights; i++ )
	{
		if ( g_Lights[i].m_bEnabled )
		{
			// `打開這盞光`
			device->LightEnable(i, TRUE);
			// `D3D9經由設定D3DLIGHT9 struct來設定光源屬性`
			D3DLIGHT9 light;
			ZeroMemory( &light, sizeof(light) );

			switch(g_Lights[i].m_eType)
			{
			case LIGHT_DIRECTIONAL:
				// `設定成方向光`
				light.Type = D3DLIGHT_DIRECTIONAL;
				// `設定方向`
				light.Direction = *(D3DVECTOR *) &g_Lights[i].m_vDirection;	
				break;
			case LIGHT_POINT:
				// `設定成點光源`
				light.Type = D3DLIGHT_POINT;
				// `設定位置`
				light.Position = *(D3DVECTOR *) &g_Lights[i].m_vPosition;
				// `隨距離的衰減值`
				// 1/(CONSTANT+LINEAR*d+QUADRATIC*d^2)
				// `公式中的CONSTANT, LINEAR, QUADRATIC值`
				light.Attenuation0 = g_Lights[i].m_vAttenuation[0];
				light.Attenuation1 = g_Lights[i].m_vAttenuation[1];
				light.Attenuation2 = g_Lights[i].m_vAttenuation[2];
				break;
			case LIGHT_SPOT:
				// `設定成聚光燈`
				light.Type = D3DLIGHT_SPOT;
				// `設定位置`
				light.Position = *(D3DVECTOR *) &g_Lights[i].m_vPosition;
				// `設定方向`
				light.Direction = *(D3DVECTOR *) &g_Lights[i].m_vDirection;	
				// `隨距離的衰減值`
				// 1/(CONSTANT+LINEAR*d+QUADRATIC*d^2)
				// `公式中的CONSTANT, LINEAR, QUADRATIC值`
				light.Attenuation0 = g_Lights[i].m_vAttenuation[0];
				light.Attenuation1 = g_Lights[i].m_vAttenuation[1];
				light.Attenuation2 = g_Lights[i].m_vAttenuation[2];
				// `光柱圓錐的一半角度`
				light.Phi = FastMath::DegreeToRadian(g_Lights[i].m_fSpotlightCutoff);
				// `Inner Cone的角度, Inner Cone內部沒有角度衰滅.`
				light.Theta = FastMath::DegreeToRadian(g_Lights[i].m_fSpotLightInnerCone);
				// `光柱的角度衰滅值`
				light.Falloff = g_Lights[i].m_fSpotlightExponent;
				break;
			}
			// `光源的Ambient值`
			light.Ambient = *(D3DCOLORVALUE*) &g_Lights[i].m_vAmbientColor;
			// `光源的Diffuse值`
			light.Diffuse = *(D3DCOLORVALUE*) &g_Lights[i].m_vDiffuseColor;
			// `光源有效范圍, 在此把它設定成浮點數所容計的最大值.`
			light.Range = 1000.0f;
			// `用SetLight來設定光源屬性`
			device->SetLight(i, &light);
		}
		else
		{
			// `關閉這盞光`
			device->LightEnable(i, FALSE);
		}
	}
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// 開始下繪圖指令
	device->BeginScene(); 
	device->SetTransform(D3DTS_WORLD, (D3DMATRIX *) &g_world_matrix);
	// 設定光源
	SetupLightingDX9();
	// 設定資料格式
	device->SetFVF(D3DFVF_XYZ|D3DFVF_NORMAL);
	// 畫出格子
	device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLESTRIP, 0, g_iNumGridVertices, g_iNumGridTriangles, 
		g_pGridIndices, D3DFMT_INDEX16, g_pGridVertices, sizeof(Vertex_V3N3) );
	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 
	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
