#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
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
	g_bBlur = !g_bBlur;
	printf("Blur %s\n", g_bBlur ? "on" : "off");
}

void frame_move(void)
{
	const float PI = 3.14159f;
	const float PI_double = PI * 2.0f;
	const float days_a_year = 365.0f;
	const float days_a_month = 28.0f;
	const float earth_to_sun_distance = 6.0f; // 地球離太陽的假設值
	const float earth_selfrot_speed = 50.0f;
	const float moon_to_earth_distance = 2.0f; // 月球離地球的假設值
	const float simulation_speed = 60.0f; // 1秒相當於60天

	static float simulation_days = 0;
	simulation_days += g_fFrame_Time * simulation_speed;

	g_sun_matrix = g_Control.GetObjectMatrix();
	// 算出地球的位置
	g_earth_matrix = g_sun_matrix; // 把地球放到太陽的座標系上
	g_earth_matrix.RotateZ( 2.0f * PI * simulation_days / days_a_year); 
	g_earth_matrix.TranslateX( earth_to_sun_distance );
	g_earth_matrix.RotateZ( 2.0f * PI * simulation_days / earth_selfrot_speed);
	// 算出月球的位置
	g_moon_matrix = g_earth_matrix; // 把月球放到地球的座標系上
	g_moon_matrix.RotateZ( 2.0f * PI * simulation_days / days_a_month );
	g_moon_matrix.TranslateX( moon_to_earth_distance );
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
	// 做OpenGL或Direct3D初始化
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	g_Models[0].Load_ASCII("../../models/sun.gma");
	g_Models[1].Load_ASCII("../../models/earth.gma");
	g_Models[2].Load_ASCII("../../models/moon.gma");

	GutInputInit();

	g_Control.SetCamera(Vector4(0.0f, 10.0f, 0.0f), Vector4(0.0f,0.0f,0.0f), Vector4(0.0f, 0.0f, 1.0f) );

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);

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
		frame_move();
		render();
	}
	// 卸載shader
	release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
