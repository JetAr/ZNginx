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

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_ROTATEOBJECT);
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

void KeyDown_4(void)
{
	g_bAnimation = !g_bAnimation;
	printf(g_bAnimation ? "Moving" : "Freezing");
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

void frame_move(void)
{
	const float PI = 3.14159;
	const float PI_double = PI * 2.0f;
	const float days_a_year = 365.0f;
	const float days_a_month = 28.0f;
	const float earth_to_sun_distance = 6.0f; // 地球離太陽的假設值
	const float moon_to_earth_distance = 2.0f; // 月球離地球的假設值
	const float simulation_speed = 60.0f; // 1秒相當於60天
	const float spinning = 28.0f;
	const float sun_radius = 3.0f;
	const float earth_radius = 1.0f;
	const float moon_radius = 0.5f;

	float time_diff = g_bAnimation ? g_fFrame_Time : 0.0f;
	static float simulation_days = 0;

	simulation_days += time_diff * simulation_speed;

	Matrix4x4 transoform_sun, transform_earth;
	Matrix4x4 scale_matrix;

	//
	transoform_sun = g_Control.GetObjectMatrix();
	scale_matrix.Scale_Replace(sun_radius, sun_radius, sun_radius);
	g_sun_matrix = scale_matrix * transoform_sun;
	// 算出地球的位置
	transform_earth = transoform_sun; // 把地球放到太陽的座標系上
	transform_earth.RotateZ( 2.0f * PI * simulation_days / days_a_year); 
	transform_earth.TranslateX( earth_to_sun_distance );
	transform_earth.RotateZ( 2.0f * PI * simulation_days / spinning );
	scale_matrix.Scale_Replace(earth_radius, earth_radius, earth_radius);
	g_earth_matrix = scale_matrix * transform_earth;
	// 算出月球的位置
	g_moon_matrix = transform_earth; // 把月球放到地球的座標系上
	g_moon_matrix.RotateZ( 2.0f * PI * simulation_days / days_a_month );
	g_moon_matrix.TranslateX( moon_to_earth_distance );
	scale_matrix.Scale_Replace(moon_radius, moon_radius, moon_radius);
	g_moon_matrix = scale_matrix * g_moon_matrix;
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

	if ( !g_Model.Load_ASCII("../../models/jet.gma") )
	{
		exit(0);
	}

	GutInputInit();
	g_Control.SetCamera(Vector4(0.0f, 0.0f, 10.0f), Vector4(0.0f), Vector4(0.0f, 1.0f, 0.0f) );

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
		frame_move();
		render();
	}

	// 卸載shader
	release_resource();

	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
