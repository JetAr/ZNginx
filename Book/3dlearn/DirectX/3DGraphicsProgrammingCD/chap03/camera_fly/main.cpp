#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
#include "render_opengl.h"
#include "render_data.h"

#ifdef _ENABLE_DX10_
#include "render_dx10.h"
#endif

GutTimer g_Timer;

void GetUserInput(void)
{
	// `讀取滑鼠`
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// `讀取鍵盤`
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);
	// `取得畫完前一個畫面到現在所經歷的時間`
	float time_diff = g_Timer.Stop();
	g_Timer.Restart();

	float moving_speed = 2.0f * time_diff;
	float rotation_speed = 1.0 * time_diff;

	// `極座標系統`
	static float theta = -MATH_PI * 0.5f;
	static float phi = 0.0f;

	// `如果按下滑鼠左鍵, 就旋轉鏡頭.`
	if ( mouse.button[0] ) 
	{
		theta += mouse.x * rotation_speed;
		phi -= mouse.y * rotation_speed;
	}

	float sin_phi, cos_phi;
	float sin_theta, cos_theta;

	FastMath::SinCos(phi, sin_phi, cos_phi);
	FastMath::SinCos(theta, sin_theta, cos_theta);

	// `計算鏡頭的面向`
	Vector4 camera_facing;

	camera_facing[0] = cos_phi * cos_theta;
	camera_facing[1] = sin_phi;
	camera_facing[2] = cos_phi * sin_theta;

	// `計算鏡頭正上方的軸向`
	Vector4 camera_up;
	FastMath::SinCos(phi + MATH_PI*0.5f, sin_phi, cos_phi);

	camera_up[0] = cos_phi * cos_theta;
	camera_up[1] = sin_phi;
	camera_up[2] = cos_phi * sin_theta;

	// `取得鏡面右方的方向`
	Vector4 camera_right = Vector3CrossProduct(camera_facing, camera_up);

	// `按下W或方向鍵向上`
	if ( keyboard_state[GUTKEY_W] || keyboard_state[GUTKEY_UP] )
	{
		g_eye += camera_facing * moving_speed;
	}
	// `按下S或方向鍵向下`
	if ( keyboard_state[GUTKEY_S] || keyboard_state[GUTKEY_DOWN] )
	{
		g_eye -= camera_facing * moving_speed;
	}
	// `按下A或方向鍵向左`
	if ( keyboard_state[GUTKEY_A] || keyboard_state[GUTKEY_LEFT] )
	{
		g_eye -= camera_right * moving_speed;
	}
	// `按下D或方向鍵向右`
	if ( keyboard_state[GUTKEY_D] || keyboard_state[GUTKEY_RIGHT] )
	{
		g_eye += camera_right * moving_speed;
	}

	// `計算出鏡頭對準的點, 產生鏡頭轉換矩陣時會用到.`
	g_lookat = g_eye + camera_facing;
	// `因為是對2個軸轉動, 需要更新鏡頭朝上的軸.`
	g_up = camera_up;
}

void main(int argc, char *argv[])
{
	// 內定使用DirectX 9來繪圖
	char *device = "dx9";
	void (*render)(void) = RenderFrameDX9;
	bool (*init_resource)(void) = InitResourceDX9;
	bool (*release_resource)(void) = ReleaseResourceDX9;

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
		break;
	case '2':
		device = "opengl";
		init_resource = InitResourceOpenGL;
		release_resource = ReleaseResourceOpenGL;
		render = RenderFrameOpenGL;
		break;
	case '3':
#ifdef _ENABLE_DX10_
		device = "dx10";
		init_resource = InitResourceDX10;
		release_resource = ReleaseResourceDX10;
		render = RenderFrameDX10;
#endif
		break;
	}

	// 在(100,100)的位置開啟一個大小為(512x512)的視窗
	GutCreateWindow(100, 100, 512, 512, device);

	// 做OpenGL或DirectX初始化
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	GutInputInit();

	g_view_matrix.Identity();

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
