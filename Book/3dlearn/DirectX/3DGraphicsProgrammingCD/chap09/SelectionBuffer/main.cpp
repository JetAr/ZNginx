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
	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_ROTATEOBJECT);
}

void frame_move(void)
{
	const float PI = 3.14159;
	const float PI_double = PI * 2.0f;
	const float days_a_year = 365.0f;
	const float days_a_month = 28.0f;
	const float earth_to_sun_distance = 2.0f; // 地球離太陽的假設值
	const float moon_to_earth_distance = 2.0f; // 月球離地球的假設值
	const float simulation_speed = 60.0f; // 1秒相當於60天

	static float simulation_days = 0;
	simulation_days += g_fFrame_Time * simulation_speed;

	// 把太陽放在世界座標系原點
	g_sun_matrix.Identity();
	// 算出地球的位置
	g_earth_matrix = g_sun_matrix; // 把地球放到太陽的座標系上
	g_earth_matrix.RotateZ( 2.0f * PI * simulation_days / days_a_year); 
	g_earth_matrix.TranslateX( earth_to_sun_distance );
	// 算出月球的位置
	g_moon_matrix = g_earth_matrix; // 把月球放到地球的座標系上
	g_moon_matrix.RotateZ( 2.0f * PI * simulation_days / days_a_month );
	g_moon_matrix.TranslateX( moon_to_earth_distance );
}

void main(void)
{
	// 內定使用DirectX 9來繪圖
	char *device = "opengl";
	void (*render)(void) = RenderFrameOpenGL;
	bool (*init_resource)(void) = InitResourceOpenGL;
	bool (*release_resource)(void) = ReleaseResourceOpenGL;
	void (*resize_func)(int width, int height) = ResizeWindowOpenGL;

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

	g_sun_model.Load_ASCII("../../models/sun.gma");
	g_earth_model.Load_ASCII("../../models/earth.gma");

	g_view_matrix.Identity();
	g_world_matrix.Identity();

	GutInputInit();

	g_Control.SetCamera(Vector4(0.0f, 5.0f, 0.0f), Vector4(0.0f), Vector4(0.0f, 0.0f, 1.0f) );

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
