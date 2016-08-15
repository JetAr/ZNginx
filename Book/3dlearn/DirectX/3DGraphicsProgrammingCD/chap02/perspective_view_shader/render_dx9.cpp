#include "Gut.h"
#include "render_data.h"

static LPDIRECT3DVERTEXSHADER9 g_pVertexShaderDX9 = NULL;
static LPDIRECT3DPIXELSHADER9  g_pPixelShaderDX9 = NULL;
static LPDIRECT3DVERTEXDECLARATION9 g_pVertexShaderDecl = NULL;

bool InitResourceDX9(void)
{
	// 載入Vertex Shader
	g_pVertexShaderDX9 = GutLoadVertexShaderDX9_HLSL(
		"../../shaders/vertex_transform.shader", "VS", "vs_1_1");
	if ( g_pVertexShaderDX9==NULL )
		return false;
	// 載入Pixel Shader
	g_pPixelShaderDX9 = GutLoadPixelShaderDX9_HLSL(
		"../../shaders/vertex_transform.shader", "PS", "ps_2_0");
	if ( g_pPixelShaderDX9==NULL )
		return false;

	// 鏡頭座標系轉換矩陣
	Matrix4x4 view_matrix = 
		GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 投影轉換矩陣
	Matrix4x4 projection_matrix = 
		GutMatrixPerspectiveRH_DirectX(90.0f, 1.0f, 1.0f, 100.0f);
	// 把前兩個矩陣相乘
	Matrix4x4 view_projection_matrix = 
		view_matrix * projection_matrix;

	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 設定視角轉換矩陣
	device->SetVertexShaderConstantF(0, (float *)&view_projection_matrix, 4);

	return true;
}

bool ReleaseResourceDX9(void)
{
	if ( g_pVertexShaderDX9 )
	{
		g_pVertexShaderDX9->Release();
		g_pVertexShaderDX9 = NULL;
	}

	if ( g_pPixelShaderDX9 )
	{
		g_pPixelShaderDX9->Release();
		g_pPixelShaderDX9 = NULL;
	}

	return true;
}

// 使用DirectX9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	device->Clear(
		0, NULL, // 清除整個畫面
		D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, // 清除顏色跟Z Buffer
		D3DCOLOR_ARGB(0, 0, 0, 0), // 設定要把顏色清成黑色
		1.0f, // 設定要把Z值清為1, 也就是離鏡頭最遠.
		0 // 設定要把Stencil buffer清為0, 在這沒差.
		);

	// 開始下繪圖指令
	device->BeginScene(); 
	// 設定資料格式
	device->SetFVF(D3DFVF_XYZW); 
	// 設定Vertex/Pixel Shader
	device->SetVertexShader(g_pVertexShaderDX9);
	device->SetPixelShader(g_pPixelShaderDX9);
	// 畫出金字塔的8條邊線
	device->DrawPrimitiveUP(D3DPT_LINELIST, 8, g_vertices, sizeof(Vector4)); 
	// 宣告所有的繪圖指令都下完了
	device->EndScene(); 

	// 把背景backbuffer的畫面呈現出來
	device->Present( NULL, NULL, NULL, NULL );
}
