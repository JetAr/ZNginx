#include <d3dx9.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutFont_DX9.h"

static CGutFontUniCodeDX9 g_Font;

bool InitResourceDX9(void)
{
	int w,h;
	GutGetWindowSize(w, h);

	// 取得Direct3D 9裝置
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 關閉打光
	device->SetRenderState(D3DRS_LIGHTING, FALSE);

	g_Font.SetConsoleResolution(w, h);
	g_Font.SetFontSize(32, 32);
	g_Font.SetFontAdvance(32, 32);
	g_Font.SetConsoleSize(w/32, h/32);
	g_Font.SetFontTextureLayout(32, 32, 0);

	g_Font.Initialize();
	g_Font.CreateTexture(1024,1024);

	g_Font.Puts(L"按下 Alt+F4 離開");
	g_Font.Puts(L"繁體中文");
	g_Font.Puts(L"");
	g_Font.Puts(L"　　　早發白帝城　　李白", false);
	g_Font.Puts(L"");
	g_Font.Puts(L"朝辭白帝彩雲間，千里江陵一日還。", false);
	g_Font.Puts(L"兩岸猿聲啼不住，輕舟已過萬重山。", false);
	g_Font.Puts(L"");
	g_Font.Puts(L"按下 Alt+F4 离开");
	g_Font.Puts(L"简体中文");
	g_Font.Puts(L"");
	g_Font.Puts(L"　　　早发白帝城　　李白", false);
	g_Font.Puts(L"");
	g_Font.Puts(L"朝辞白帝彩云间，千里江陵一日还。", false);
	g_Font.Puts(L"两岸猿声啼不住，轻舟已过万重山。", true);

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
}

// 使用DirectX 9來繪圖
void RenderFrameDX9(void)
{
	LPDIRECT3DDEVICE9 device = GutGetGraphicsDeviceDX9();
	// 消除畫面
	device->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	// 開始下繪圖指令
	device->BeginScene(); 
	// 畫出文字
	g_Font.Render();
	//
	device->EndScene(); 
	// 把背景backbuffer呈現出來
    device->Present( NULL, NULL, NULL, NULL );
}
