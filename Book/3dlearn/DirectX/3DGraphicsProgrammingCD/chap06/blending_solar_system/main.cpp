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

void KeyDown_Z(void)
{
	g_bZWrite = !g_bZWrite;
	printf("zwrite %s\n", g_bZWrite ? "on" : "off");
}

void KeyDown_1(void)
{
	g_iBlendMode = 1;
	printf("blendmode = add\n");
}

void KeyDown_2(void)
{
	g_iBlendMode = 0;
	printf("blendmode = alphablend\n");
}

void KeyDown_S(void)
{
	g_bSorting = !g_bSorting;
	printf("sorting %s\n", g_bSorting ? "on" : "off");
}

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
	const float earth_to_sun_distance = 5.0f; // `地球離太陽的假設值`
	const float simulation_speed = 60.0f; // `1秒相當於60天`

	static float simulation_days = 0;
	simulation_days += g_fFrame_Time * simulation_speed;

	// `把太陽放在世界座標系原點`
	g_sun_matrix.Identity();
	// `算出地球的位置`
	g_earth_matrix = g_sun_matrix; // `把地球放到太陽的座標系上`
	g_earth_matrix.RotateY( 2.0f * PI * simulation_days / days_a_year); 
	g_earth_matrix.TranslateX( earth_to_sun_distance );

	Matrix4x4 m = g_Control.GetObjectMatrix();
	g_rot_matrix = m;

	Vector4 eye = g_Control.GetCameraPosition();
	// `排序`
	if ( g_bSorting )
	{
		float distance[2];
		Matrix4x4 transformed_matrix;
		Vector4 vPosition, vDiff;

		transformed_matrix = g_sun_matrix * g_rot_matrix;
		vPosition = transformed_matrix[3];
		vDiff = vPosition - eye;
		distance[0] = vDiff.Length();

		transformed_matrix = g_earth_matrix * g_rot_matrix;
		vPosition = transformed_matrix[3];
		vDiff = vPosition - eye;
		distance[1] = vDiff.Length();

		if ( distance[0] > distance[1] )
		{
			g_iOrder[0] = 0;
			g_iOrder[1] = 1;
		}
		else
		{
			g_iOrder[0] = 1;
			g_iOrder[1] = 0;
		}
	}
	else
	{
		g_iOrder[0] = 0;
		g_iOrder[1] = 1;
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

	g_rot_matrix.Identity();

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_S, KeyDown_S);
	GutRegisterKeyDown(GUTKEY_Z, KeyDown_Z);

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_NUMPAD1, KeyDown_1);

	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_NUMPAD2, KeyDown_2);

	g_Control.SetCamera(Vector4(0.0f, 0.0f, 10.0f), Vector4(0.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f) );

	// Sun
	float red[]={0.7f, 0.7f, 0.0f, 0.3f};
	CreateSphere(3.0f, &g_pSunVertices, &g_pSphereIndices, red);
	// Earch
	float blue[]={0.2f, 0.2f, 1.0f, 0.7f};
	CreateSphere(1.0f, &g_pEarthVertices, NULL, blue);

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
