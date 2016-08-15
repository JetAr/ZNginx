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
	// 讀取滑鼠
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// 讀取鍵盤
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);
	// 取得畫完前一個畫面到現在所經歷的時間
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();

	float moving_speed = 2.0f * g_fFrame_Time;
	float rotation_speed = 1.0 * g_fFrame_Time;

	// 極座標系統
	static float theta = -MATH_PI * 0.5f;
	static float phi = 0.0f;

	// 如果按下滑鼠左鍵，就旋轉鏡頭
	if ( mouse.button[0] ) 
	{
		theta += mouse.x * rotation_speed;
		phi -= mouse.y * rotation_speed;
	}

	float sin_phi, cos_phi;
	float sin_theta, cos_theta;

	FastMath::SinCos(phi, sin_phi, cos_phi);
	FastMath::SinCos(theta, sin_theta, cos_theta);

	// 計算鏡頭的面向
	Vector4 camera_facing;

	camera_facing[0] = cos_phi * cos_theta;
	camera_facing[1] = sin_phi;
	camera_facing[2] = cos_phi * sin_theta;

	// 計算鏡頭正上方的軸向
	Vector4 camera_up;
	FastMath::SinCos(phi + MATH_PI*0.5f, sin_phi, cos_phi);

	camera_up[0] = cos_phi * cos_theta;
	camera_up[1] = sin_phi;
	camera_up[2] = cos_phi * sin_theta;

	// 取得鏡面右方的方向
	Vector4 camera_right = Vector3CrossProduct(camera_up, camera_facing);

	// 按下W或方向鍵向上
	if ( keyboard_state[GUTKEY_W] || keyboard_state[GUTKEY_UP] )
	{
		g_eye += camera_facing * moving_speed;
	}
	// 按下S或方向鍵向下
	if ( keyboard_state[GUTKEY_S] || keyboard_state[GUTKEY_DOWN] )
	{
		g_eye -= camera_facing * moving_speed;
	}
	// 按下A或方向鍵向左
	if ( keyboard_state[GUTKEY_A] || keyboard_state[GUTKEY_LEFT] )
	{
		g_eye -= camera_right * moving_speed;
	}
	// 按下D或方向鍵向右
	if ( keyboard_state[GUTKEY_D] || keyboard_state[GUTKEY_RIGHT] )
	{
		g_eye += camera_right * moving_speed;
	}

	// 計算出鏡頭對準的點, 產生鏡頭轉換矩陣時會用到.
	g_lookat = g_eye + camera_facing;
	// 因為是對2個軸轉動, 需要更新鏡頭朝上的軸
	g_up = camera_up;
}

void GetUserInputOld(void)
{
	// 讀取滑鼠
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// 讀取鍵盤
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);
	// 取得畫完前一個畫面到現在所經歷的時間
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();

	float moving_speed = 4.0f * g_fFrame_Time;
	float rotation_speed = 1.0 * g_fFrame_Time;

	// 要累積下對X跟Y軸的旋轉角度
	static float ry = 0.0f;
	static float rx = 0.0f;

	// 如果按下滑鼠左鍵，就旋轉鏡頭
	if ( mouse.button[0] ) 
	{
		ry += mouse.x * rotation_speed;
		rx -= mouse.y * rotation_speed;
	}

	Matrix4x4 rotate_matrix;
	rotate_matrix.RotateY_Replace(ry);
	rotate_matrix.RotateX(rx);

	// 在右手座標系里, 鏡面的面向是轉換矩陣的-Z軸
	Vector4 camera_facing = -rotate_matrix[2];
	// 取得鏡面右邊的方向
	Vector4 camera_right = rotate_matrix[0];

	// 按下W或方向鍵向上
	if ( keyboard_state[GUTKEY_W] || keyboard_state[GUTKEY_UP] )
	{
		g_eye += camera_facing * moving_speed;
	}
	// 按下S或方向鍵向下
	if ( keyboard_state[GUTKEY_S] || keyboard_state[GUTKEY_DOWN] )
	{
		g_eye -= camera_facing * moving_speed;
	}
	// 按下A或方向鍵向左
	if ( keyboard_state[GUTKEY_A] || keyboard_state[GUTKEY_LEFT] )
	{
		g_eye -= camera_right * moving_speed;
	}
	// 按下D或方向鍵向右
	if ( keyboard_state[GUTKEY_D] || keyboard_state[GUTKEY_RIGHT] )
	{
		g_eye += camera_right * moving_speed;
	}
	// 計算出鏡頭對準的點, 產生鏡頭轉換矩陣時會用到.
	g_lookat = g_eye + camera_facing;
	// 因為是對2個軸轉動, 需要更新鏡頭朝上的軸
	g_up = Vector3CrossProduct(camera_right, camera_facing);
}

void frame_move(void)
{
	const float PI = 3.14159;
	const float PI_double = PI * 2.0f;
	const float days_a_year = 365.0f;
	const float days_a_month = 28.0f;
	const float earth_to_sun_distance = 8.0f; // 地球離太陽的假設值
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

	g_view_matrix.Identity();
	GutInputInit();

	// Sun
	float yellow[]={1.0f, 1.0f, 0.0f, 1.0f};
	CreateSphere(2.0f, &g_pSunVertices, &g_pSphereIndices, yellow);
	// Earch
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
