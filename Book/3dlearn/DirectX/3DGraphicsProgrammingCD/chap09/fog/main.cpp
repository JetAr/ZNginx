#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
#include "render_opengl.h"
#include "render_data.h"

GutTimer g_Timer;

void KeyDown_0(void)
{
	g_iFogMode = 0;
	printf("Fog effect off\n");
}

void KeyDown_1(void)
{
	g_iFogMode = 1;
	printf("Linear fog\n");
}

void KeyDown_2(void)
{
	g_iFogMode = 2;
	printf("Exp fog\n");
}

void KeyDown_3(void)
{
	g_iFogMode = 3;
	printf("Exp2 fog\n");
}

void GetUserInput(void)
{
	float fTimeAdvance = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
	g_Control.Update(fTimeAdvance, CGutUserControl::CONTROLLER_FPSCAMERA);
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

	GutRegisterKeyDown(GUTKEY_0, KeyDown_0);
	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);

	g_Control.SetCamera(Vector4(0.0f, 0.0f, 1.0f), Vector4(0.0f, 1.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f) );

	g_Model.Load_ASCII("..\\..\\models\\terrain.gma");

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
		render();
	}

	// 卸載shader
	release_resource();

	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
