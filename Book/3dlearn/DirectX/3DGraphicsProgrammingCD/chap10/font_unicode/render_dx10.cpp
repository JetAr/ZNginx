#ifdef _ENABLE_DX10_

#include <D3DX10.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutFont_DX10.h"

#include "render_data.h"

static ID3D10Device	*g_pDevice = NULL;
static ID3D10SamplerState *g_pSamplerState = NULL;

CGutFontUniCodeDX10 g_Font;

bool InitResourceDX10(void)
{
	int w,h;
	GutGetWindowSize(w, h);

	g_pDevice = GutGetGraphicsDeviceDX10();

	// sampler state / texture filter
	{
		D3D10_SAMPLER_DESC sampler_desc;
		ZeroMemory(&sampler_desc, sizeof(sampler_desc));

		sampler_desc.Filter = D3D10_FILTER_MIN_MAG_MIP_LINEAR;
		sampler_desc.AddressU = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressV = D3D10_TEXTURE_ADDRESS_WRAP;
		sampler_desc.AddressW = D3D10_TEXTURE_ADDRESS_WRAP;

		g_pDevice->CreateSamplerState(&sampler_desc, &g_pSamplerState);

		for ( int i=0; i<4; i++ )
		{
			g_pDevice->PSSetSamplers(i, 1, &g_pSamplerState);
		}
	}

	g_Font.LoadShader("../../shaders/font_dx10.hlsl");

	g_Font.SetConsoleResolution(w, h);
	g_Font.SetFontSize(32, 32);
	g_Font.SetFontAdvance(32, 32);
	g_Font.SetConsoleSize(w/32, h/32);
	g_Font.SetFontTextureLayout(32, 32, 0);

	g_Font.Initialize();
	g_Font.CreateTexture(1024,1024);

	g_Font.Puts(L"");
	g_Font.Puts(L"李白　早發白帝城　繁體中文", false);
	g_Font.Puts(L"");
	g_Font.Puts(L"朝辭白帝彩雲間，千里江陵一日還。", false);
	g_Font.Puts(L"兩岸猿聲啼不住，輕舟已過萬重山。", false);
	g_Font.Puts(L"");
	g_Font.Puts(L"李白　早发白帝城　简体中文", false);
	g_Font.Puts(L"");
	g_Font.Puts(L"朝辞白帝彩云间，千里江陵一日还。", false);
	g_Font.Puts(L"两岸猿声啼不住，轻舟已过万重山。", true);

	return true;
}

bool ReleaseResourceDX10(void)
{
	SAFE_RELEASE(g_pSamplerState);

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
	Vector4 vClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	// frame buffer
	ID3D10RenderTargetView *pRenderTargetView = GutGetDX10RenderTargetView(); 
	// depth/stencil buffer
    ID3D10DepthStencilView *pDepthStencilView = GutGetDX10DepthStencilView(); 
	IDXGISwapChain *pSwapChain = GutGetDX10SwapChain(); // front/back buffer
	// 清除顏色
	g_pDevice->ClearRenderTargetView(pRenderTargetView, (float *)&vClearColor);
	// 清除Depth/Stencil buffer
	g_pDevice->ClearDepthStencilView(pDepthStencilView, D3D10_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.0f, 0);
	//
	g_Font.Render();
	// 等待硬體掃結束, 然後才更新畫面.
	pSwapChain->Present(1, 0);
}

#endif // _ENABLE_DX10_