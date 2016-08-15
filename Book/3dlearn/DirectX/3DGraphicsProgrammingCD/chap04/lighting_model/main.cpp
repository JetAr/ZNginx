#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"
#include "GutModel.h"

#include "render_dx9.h"
#include "render_dx10.h"
#include "render_opengl.h"
#include "render_data.h"

#ifdef _ENABLE_DX10_
#include "render_dx10.h"
#endif

GutTimer g_Timer;

float g_fFrame_Time = 0.0f;

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_ROTATEOBJECT);
}

void main(int argc, char *argv[])
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
#endif
	}

	printf("Select Model\n");
	printf("(1) teapot\n");
	printf("(2) gingerbread\n");
	printf("(3) trumpet\n");
	printf("(4) ship\n");
	printf("(5) jet fighter\n");
	printf("(6) bike\n");

	char *model = "../../models/box_brick.gma";

	c = getche();
	printf("\n");

	switch(c)
	{
	case '1':
		model = "../../models/teapot_notex.gma";
		break;
	case '2':
		model = "../../models/gingerbread.gma";
		break;
	case '3':
		model = "../../models/trumpet.gma";
		break;
	case '4':
		model = "../../models/ship.gma";
		break;
	case '5':
		model = "../../models/jet.gma";
		break;
	case '6':
		model = "../../models/bmw_bike.gma";
		break;
	default:
		if ( argc > 1 )
			model = argv[1];
		break;
	}

	if ( !g_Model.Load_ASCII(model) )
	{
		printf("Could not load %s\n", model);
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

	float max_ext = g_Model.m_vSize[0];
	max_ext = max(max_ext, g_Model.m_vSize[1]);
	max_ext = max(max_ext, g_Model.m_vSize[2]);

	g_fNear = max_ext * 0.1f + 0.1f;
	g_fFar = max_ext * 4.0f;

	g_Control.SetCamera(Vector4(0.0f, -max_ext*2.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f), Vector4(0.0f, 0.0f, 1.0f) );
	//g_Control.SetCamera(Vector4(0.0f, 0.0f, max_ext*2.0f, 0.0f), Vector4(0.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f) );

	g_Lights[0].m_iLightType = LIGHT_DIRECTIONAL;
	g_Lights[0].m_bEnabled = true;
	g_Lights[0].m_vDirection.Set(0.0f, 1.0f, -1.0f, 0.0f); g_Lights[0].m_vDirection.Normalize();
	//g_Lights[0].m_vDirection.Set(0.0f, 1.0f, 1.0f, 0.0f); g_Lights[0].m_vDirection.Normalize();
	g_Lights[0].m_vDiffuse.Set(0.7f);
	g_Lights[0].m_vSpecular.Set(0.8f);
	g_Lights[0].m_vAmbient.Set(0.1f);

	g_Lights[1].m_iLightType = LIGHT_DIRECTIONAL;
	g_Lights[1].m_bEnabled = true;
	g_Lights[1].m_vDirection.Set(-1.0f, 0.0f, 0.0f, 0.0f); g_Lights[1].m_vDirection.Normalize();
	g_Lights[1].m_vDiffuse.Set(0.7f);
	g_Lights[1].m_vSpecular.Set(0.8f);
	g_Lights[1].m_vAmbient.Set(0.0f);

	GutInputInit();

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
		// 畫出矩形
		render();
	}
	// 卸載shader
	release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
