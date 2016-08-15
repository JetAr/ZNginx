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

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_ROTATEOBJECT);
	//g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_FPSCAMERA);
}

void KeyDown_1(void)
{
	g_iMode = 1;
	printf("25 samples PCF Filter\n");
}

void KeyDown_2(void)
{
	g_iMode = 2;
	printf("25 samples PCF Filter + Lerp\n");
}

void KeyPressed_ADD(void)
{
	g_fZBias += 0.01f * g_fFrame_Time;
	printf("ZBias = %5.3f\r", g_fZBias);
}

void KeyPressed_MINUS(void)
{
	g_fZBias -= 0.01f * g_fFrame_Time;
	printf("ZBias = %5.3f\r", g_fZBias);
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
	case '3':
#ifdef _ENABLE_DX10_
		device = "dx10";
		init_resource = InitResourceDX10;
		release_resource = ReleaseResourceDX10;
		render = RenderFrameDX10;
		resize_func = ResizeWindowDX10;
#endif
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

	float texel = 1.0f/(float)shadowmap_size;
	int index = 0;

	for ( int y=-PCF_sample_span; y<=PCF_sample_span; y++ )
	{
		for ( int x=-PCF_sample_span; x<=PCF_sample_span; x++ )
		{
			g_vTexOffset[index++].Set(x*texel, y*texel, 0.0f, 0.0f);
		}
	}

	index = 0;
	for ( int y=-1; y<=2; y++ )
	{
		for ( int x=-1; x<=2; x++ )
		{
			g_vTexOffset2[index++].Set(x*texel, y*texel, 0.0f, 0.0f);
		}
	}

	GutInputInit();
	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyPressed(GUTKEY_ADD, KeyPressed_ADD);
	GutRegisterKeyPressed(GUTKEY_SUBTRACT, KeyPressed_MINUS);

	g_Control.SetCamera(Vector4(5.0f, 0.0f, 5.0f), Vector4(0.0f), Vector4(0.0f, 1.0f, 0.0f) );

	g_Model.SetTexturePath("../../textures/");
	if ( !g_Model.Load_ASCII("../../models/altar.gma") )
	{
		exit(0);
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
		render();
	}
	// 卸載shader
	release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
