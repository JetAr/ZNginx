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

void frame_move(void)
{
	const float PI = 3.14159;
	const float PI_double = PI * 2.0f;
	const float days_a_year = 365.0f;
	const float days_a_month = 28.0f;
	const float earth_to_sun_distance = 6.0f; // 地球離太陽的假設值
	const float moon_to_earth_distance = 2.0f; // 月球離地球的假設值
	const float simulation_speed = 60.0f; // 1秒相當於60天

	static float simulation_days = 0;
	simulation_days += g_fFrame_Time * simulation_speed;

	// 把太陽放在世界座標系原點
	g_sun_matrix.Identity();
	// 算出地球的位置
	g_earth_matrix = g_sun_matrix; // 把地球放到太陽的座標系上
	g_earth_matrix.RotateY( 2.0f * PI * simulation_days / days_a_year); 
	g_earth_matrix.TranslateX( earth_to_sun_distance );
	// 算出月球的位置
	g_moon_matrix = g_earth_matrix; // 把月球放到地球的座標系上
	g_moon_matrix.RotateY( 2.0f * PI * simulation_days / days_a_month );
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

	GutInputInit();

	g_Control.SetCamera(Vector4(0.0f, 0.0f, 2.0f), Vector4(0.0f), Vector4(0.0f, 1.0f, 0.0f) );

	// Sun
	float yellow[]={1.0f, 1.0f, 0.0f, 1.0f};
	CreateSphere(2.0f, &g_pSunVertices, &g_pSphereIndices, yellow);
	// Earth
	float blue[]={0.0f, 0.0f, 1.0f, 1.0f};
	CreateSphere(1.0f, &g_pEarthVertices, NULL, blue);
	// Moon
	float white[]={1.0f, 1.0f, 1.0f, 1.0f};
	CreateSphere(0.2f, &g_pMoonVertices, NULL);

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
