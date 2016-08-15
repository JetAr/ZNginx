#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
#include "render_dx10.h"
#include "render_opengl.h"
#include "render_data.h"

GutTimer g_Timer;
float g_fFrame_Time = 0.0f;

/*
enum LIGHT_SOURCE
{
LIGHT_DIRECTIONAL,
LIGHT_POINT,
LIGHT_SPOT
};
*/

static int g_iLightSource = LIGHT_DIRECTIONAL;
static int g_iNumGrids = 15;

void GetUserInput(void)
{
	// 讀取滑鼠
	int width, height;
	int x,y,buttons[3];

	GutGetWindowSize(width, height);
	GutGetMouseState(x, y, buttons);

	// 如果按下滑鼠左鍵，就移動物件
	if ( buttons[0] ) 
	{
		float fx = (float) x / (float) width;
		float fy = (float) y / (float) height;
		fx = (fx - 0.5f) * g_fOrthoWidth;
		fy = (0.5f - fy) * g_fOrthoHeight;
		g_world_matrix.Translate_Replace(fx, fy, 0.0f);
	}
}

void main(void)
{
	// 內定使用DirectX 9來繪圖
	char *device = "dx9";
	void (*render)(void) = RenderFrameDX9;
	bool (*init_resource)(void) = InitResourceDX9;
	bool (*release_resource)(void) = ReleaseResourceDX9;
	void (*resize_func)(int width, int height) = ResizeWindowDX9;

	printf("Press\n(1) for Direct3D9\n(2) for OpenGL\n");
	int c = getche();
	switch(c)
	{
	default:
	case '1':
		render = RenderFrameDX9;
		init_resource = InitResourceDX9;
		release_resource = ReleaseResourceDX9;
		resize_func = ResizeWindowDX9;
		break;
	case '2':
		device = "opengl";
		init_resource = InitResourceOpenGL;
		release_resource = ReleaseResourceOpenGL;
		render = RenderFrameOpenGL;
		resize_func = ResizeWindowOpenGL;
		break;
	}

	printf("\nSelected %s device for rendering.\n", device);

	GutResizeFunc( resize_func );
	GutCreateWindow(100, 100, 512, 512, device);

	// 做OpenGL或DirectX初始化
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	GutTimer timer;
	srand(timer.m_PerformanceCount.LowPart);

	g_view_matrix.Identity();
	g_world_matrix.Identity();

	GutInputInit();

	// 設定環境光的位置跟顏色
	Vector4 vFirstLight(-g_fOrthoWidth * 0.4f, -g_fOrthoHeight * 0.4f, 0.5f, 1.0f);
	Vector4 vLastLight ( g_fOrthoWidth * 0.4f,  g_fOrthoHeight * 0.4f, 0.5f, 1.0f);
	Vector4 vSpan = vLastLight - vFirstLight;
	Vector4 vInc = vSpan / 3.0f;

	Vector4 vPos = vFirstLight;
	int i=0;
	for ( int y=0; y<4; y++ )
	{
		for ( int x=0; x<4; x++, i++ )
		{
			g_LightPosition[i].m_Position = vPos;
			// 用亂數來產生光的強度
			int r = rand() & 0x003ff + 1024; // 1024-2048間的亂數
			int g = rand() & 0x003ff + 1024;
			int b = rand() & 0x003ff + 1024;
			// 讓r,g,b 落在0.5-1之間
			g_LightPosition[i].m_Color.Set(r/2048.0f, g/2048.0f, b/2048.0f, 1.0f);
			vPos[0] += vInc[0];
		}
		vPos[0] = vFirstLight[0];
		vPos[1] += vInc[1];
	}

	// 載入shader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	// 主回圈
	while( GutProcessMessage() )
	{
		GetUserInput();
		//
		LightManager();
		// 畫出矩形
		render();
	}
	// 卸載shader
	release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
