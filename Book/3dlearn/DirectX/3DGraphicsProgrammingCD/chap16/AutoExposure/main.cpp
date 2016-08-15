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
bool g_bAnimation = true;

const float g_fSpeed = 0.1f;

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
	//g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_ROTATEOBJECT);
	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_FPSCAMERA);
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
	g_iMode = !g_iMode;
}

void KeyDown_3(void)
{
}

void KeyDown_4(void)
{
}

void KeyPressed_ADD(void)
{
}

void KeyPressed_SUBTRACT(void)
{
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

	CGutModel::SetTexturePath("../../textures/");

	if ( !g_Model.Load_ASCII("../../models/sphere_HDR.gma") )
	{
		printf("Failed to load sphere_HDR.gma\n");
		exit(0);
	}

	GutInputInit();

	g_Control.SetCamera(Vector4(0.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f) );

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);
	GutRegisterKeyDown(GUTKEY_4, KeyDown_4);
	GutRegisterKeyDown(GUTKEY_SPACE, KeyDown_4);

	GutRegisterKeyPressed(GUTKEY_ADD, KeyPressed_ADD);
	GutRegisterKeyPressed(GUTKEY_SUBTRACT, KeyPressed_SUBTRACT);

	// 載入shader
	if ( !init_resource() )
	{
		release_resource();
		std::string msg = GutPopMessage();
		printf("%s\n", msg.c_str());
		exit(0);
	}

	// 主回圈
	while( GutProcessMessage() )
	{
		GetUserInput();
		//frame_move();
		render();
	}

	// 卸載shader
	release_resource();

	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
