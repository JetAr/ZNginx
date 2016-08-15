#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
#include "render_opengl.h"
#include "render_data.h"

GutTimer g_Timer;

bool g_bAnimation = true;

void KeyDown_1(void)
{
	g_iMode = 1;
	printf("No anti-aliasing\n");
}

void KeyDown_2(void)
{
	g_iMode = 2;
	printf("Multi-sampling\n");
}

void KeyDown_3(void)
{
	g_iMode = 3;
	printf("Super-sampling\n");
}

void KeyDown_4(void)
{
	g_iMode = 4;
	printf("OpenGL Smooth Line\n");
}

void KeyDown_S(void)
{
	g_bAnimation = !g_bAnimation;
	printf("Animation %s\n", g_bAnimation ? "on" : "off");
}

void KeyDown_W(void)
{
	g_bWireframe = !g_bWireframe;
	printf("%s\n", g_bWireframe ? "Wireframe" : "Solid");
}

void ProcessUserInput(void)
{
	GutReadKeyboard();
}

void FrameMove(void)
{
	float fTimeAdvance = g_Timer.Stop();
	g_Timer.Restart();

	if ( g_bAnimation )
	{
		const float fSpeed = FastMath::DegToRad(30.0f);
		g_world_matrix.RotateZ(fTimeAdvance * fSpeed);
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
	printf("\n");

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

	GutDeviceSpec spec;
	spec.m_iMultiSamples = 4;
	spec.m_szDevice = device;
	// 做OpenGL或DirectX初始化
	if ( !GutInitGraphicsDevice(spec) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);
	GutRegisterKeyDown(GUTKEY_4, KeyDown_4);
	GutRegisterKeyDown(GUTKEY_S, KeyDown_S);
	GutRegisterKeyDown(GUTKEY_W, KeyDown_W);

	g_world_matrix.Identity();
	// 載入shader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	g_Timer.Start();

	// 主回圈
	while( GutProcessMessage() )
	{
		ProcessUserInput();
		FrameMove();
		render();
	}

	// 卸載shader
	release_resource();

	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
