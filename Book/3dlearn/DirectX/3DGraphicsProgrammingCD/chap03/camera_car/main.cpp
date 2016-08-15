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

void GetUserInput(void)
{
	// `讀取滑鼠`
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// `讀取鍵盤`
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);
	// `計算目前鏡頭面向`
	Vector4 facing = g_lookat - g_eye;
	// `確保它是個單位向量`
	facing.Normalize(); 
	// `取得畫完前一個畫面到現在所經歷的時間`
	float time_diff = g_Timer.Stop();
	g_Timer.Restart();

	float moving_speed = 2.0f * time_diff;
	float rotation_speed = 1.0 * time_diff;
	float ry = 0.0f;

	// `如果按下滑鼠左鍵, 就旋轉鏡頭.`
	if ( mouse.button[0] ) 
		ry = mouse.x * rotation_speed;
	// `按下A或是方向鍵<-, 就左旋.`
	if ( keyboard_state[GUTKEY_A] || keyboard_state[GUTKEY_LEFT] )
		ry = -rotation_speed;
	// `按下D或是方向鍵->, 就右旋.`
	if ( keyboard_state[GUTKEY_D] || keyboard_state[GUTKEY_RIGHT] )
		ry = rotation_speed;

	if ( ry )
	{
		// `先取得一個旋轉矩陣`
		Matrix4x4 rotate_matrix;
		rotate_matrix.RotateY_Replace(-ry);
		// `把原本的面向沿Y軸旋轉ry度, 取得新的面向.`
		facing = facing * rotate_matrix;
	}

	// `按下W或方向鍵向上`
	if ( keyboard_state[GUTKEY_W] || keyboard_state[GUTKEY_UP] )
	{
		// `鏡頭向前移動`
		g_eye += facing * moving_speed; 
	}

	// `按下S或方向鍵向下`
	if ( keyboard_state[GUTKEY_S] || keyboard_state[GUTKEY_DOWN] )
	{
		// `鏡頭向後移動`
		g_eye -= facing * moving_speed; 
	}

	// `計算出鏡頭對準的點, 產生鏡頭轉換矩陣時會用到.`
	g_lookat = g_eye + facing; 
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
	g_object_matrix.Identity();

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
