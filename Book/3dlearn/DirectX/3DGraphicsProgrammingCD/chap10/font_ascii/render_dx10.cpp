#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "Gut.h"
#include "GutFont_DX10.h"

static ID3D10Device	*g_pDevice = NULL;

CGutFontDX10 g_Font;

bool InitResourceDX10(void)
{
	g_pDevice = GutGetGraphicsDeviceDX10();

	g_Font.LoadShader("../../shaders/texture_alphatest_dx10.hlsl");

	g_Font.SetFontSize(32, 32);
	g_Font.SetFontAdvance(20, 30);
	g_Font.SetConsoleSize(32, 16);
	// 載入字型
	g_Font.LoadTexture("../../textures/ascii_couriernew.tga");

	g_Font.Puts("3D Graphics", false);
	g_Font.Puts("Font Rendering Demo", false);
	g_Font.Puts("", false);
	g_Font.Puts("0123456789+-*/{}|\\", false);
	g_Font.Puts("abcdefghijklmnopqrstuvwxyz", false);
	g_Font.Puts("ABCDEFGHIJKLMNOPQRSTUVWXYZ", true);

	return true;
}

bool ReleaseResourceDX10(void)
{
	return true;
}

void ResizeWindowDX10(int width, int height)
{
	GutResetGraphicsDeviceDX10();
	// 	
	g_Font.SetConsoleResolution(width, height);
	g_Font.BuildMesh();
}

void RenderFrameDX10(void)
{
	// 取得呼叫GutCreateGraphicsDeviceDX10時所產生的D3D10物件
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView();
	ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	// 清除晝面
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	// 畫出文字
	g_Font.Render();
	// 等待硬體掃結束, 然後才更新畫面
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_