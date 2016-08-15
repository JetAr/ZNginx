#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutTexture_OpenGL.h"
#include "GutFont_OpenGL.h"

static CGutFontUniCodeOpenGL g_Font;

bool InitResourceOpenGL(void)
{
	int w,h;
	GutGetWindowSize(w, h);
	// 啟動示貼圖內插
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// 使用2D貼圖功能
	glEnable(GL_TEXTURE_2D);

	g_Font.SetConsoleResolution(w, h);
	g_Font.SetFontSize(32, 32);
	g_Font.SetFontAdvance(32, 32);
	g_Font.SetFontTextureLayout(32, 32, 0);
	g_Font.SetConsoleSize(w/32, h/32);

	g_Font.CreateTexture(1024, 1024);
	g_Font.Initialize();

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
//
bool ReleaseResourceOpenGL(void)
{
	return true;
}
// callback function. 視窗大小改變時會被呼叫, 並傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	g_Font.SetConsoleResolution(width, height);
	g_Font.BuildMesh();
}
// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 畫出文字	
	g_Font.Render();
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
