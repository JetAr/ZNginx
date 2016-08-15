#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
#include "render_dx10.h"
#include "render_opengl.h"
#include "render_data.h"

GutTimer g_Timer;
float g_fFrame_Time = 0.0f;
int g_iParameter = 1;
bool g_bDirty = false;

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
}

void KeyDown_1(void)
{
	if ( g_bDirty ) printf("\n");
	g_bPosteffect = !g_bPosteffect;
	g_bDirty = false;
	printf("Post effect %s\n", g_bPosteffect ? "on" : "off");
}

void KeyDown_2(void)
{
	if ( g_bDirty ) printf("\n");
	g_iParameter = 1;
	g_bDirty = false;
	printf("Tweaking brightness scale\n");
}

void KeyDown_3(void)
{
	if ( g_bDirty ) printf("\n");
	g_iParameter = 2;
	g_bDirty = false;
	printf("Tweaking brightness offset\n");
}

void KeyPressed_ADD(void)
{
	switch(g_iParameter)
	{
	case 1:
		g_vBrightnessScale += 0.5f * g_fFrame_Time;
		break;
	case 2:
		g_vBrightnessOffset += 0.5f * g_fFrame_Time;
		break;
	default:
		break;
	}
	printf("scale=%6.3f, offset=%6.3f\r", g_vBrightnessScale[0], g_vBrightnessOffset[0]);
	g_bDirty = true;
}

void KeyPressed_SUBTRACT(void)
{
	switch(g_iParameter)
	{
	case 1:
		g_vBrightnessScale -= 0.5f * g_fFrame_Time;
		break;
	case 2:
		g_vBrightnessOffset -= 0.5f * g_fFrame_Time;
		break;
	default:
		break;
	}
	g_bDirty = true;
	printf("scale=%6.3f, offset=%6.3f\r", g_vBrightnessScale[0], g_vBrightnessOffset[0]);
}

void main(void)
{
	// 內定使用DirectX 9來繪圖
	char *device = "dx9";
	void (*render)(void) = RenderFrameDX9;
	bool (*init_resource)(void) = InitResourceDX9;
	bool (*release_resource)(void) = ReleaseResourceDX9;
	void (*resize_func)(int width, int height) = ResizeWindowDX9;

#ifdef _ENABLE_DX10_
	printf("Press\n(1) for Direct3D9\n(2) for OpenGL\n(3) for Direct3D10\n");
#else
	printf("Press\n(1) for Direct3D9\n(2) for OpenGL\n");
#endif

	//int c = getche();
	int c = '1';

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
#ifdef _ENABLE_DX10_
	case '3':
		device = "dx10";
		init_resource = InitResourceDX10;
		release_resource = ReleaseResourceDX10;
		render = RenderFrameDX10;
		resize_func = ResizeWindowDX10;
		break;
#endif // _ENABLE_DX10_
	}

	GutResizeFunc( resize_func );

	// 在(100,100)的位置開啟一個大小為(512x512)的視窗
	GutCreateWindow(100, 100, 512, 512, device);

	// 做OpenGL或DirectX初始化
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);

	GutRegisterKeyPressed(GUTKEY_ADD, KeyPressed_ADD);
	GutRegisterKeyPressed(GUTKEY_SUBTRACT, KeyPressed_SUBTRACT);

	// 載入shader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	int i;
	const int num_samples = KERNELSIZE;

	float sum = 0.0f;
	for ( i=0; i<num_samples; i++ )
	{
		sum += g_weight_table[i];
	}

	for ( i=0; i<num_samples; i++ )
	{
		g_weight_table[i] /= sum;
	}

	// 主回圈
	while( GutProcessMessage() )
	{
		GetUserInput();
		render();
	}

	// 卸載shader
	release_resource();

	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
