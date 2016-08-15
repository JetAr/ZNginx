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

GutTimer g_Timer;
float g_fFrame_Time = 0.0f;
bool g_bFPS_Camera = false;
static char g_keyboard_state[256];

void GetUserInput_FPSCamera(void)
{
	// 讀取滑鼠
	GutMouseInfo mouse;
	GutReadMouse(&mouse);
	// 讀取鍵盤
	char keyboard_state[256];
	GutReadKeyboard(keyboard_state);

	// 取得畫完前一個畫面到現在所經歷的時間
	float time_diff = g_Timer.Stop();
	g_Timer.Restart();

	float moving_speed = 2.0f * time_diff;
	float rotation_speed = 1.0 * time_diff;

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
	g_up.Normalize();

}

void GetUserInput_RotateObject(void)
{
	// 讀取滑鼠
	GutMouseInfo mouse;
	if ( GutReadMouse(&mouse) )
	{
		// 取得畫完前一個畫面到現在所經歷的時間
		g_fFrame_Time = g_Timer.Stop();
		g_Timer.Restart();

		float moving_speed = 2.0f * g_fFrame_Time;
		float rotation_speed = 1.0 * g_fFrame_Time;

		// 如果按下滑鼠左鍵，就旋轉鏡頭
		if ( mouse.button[0] ) 
		{
			Matrix4x4 rotate_matrix;
			rotate_matrix.RotateY_Replace(mouse.x * rotation_speed);
			rotate_matrix.RotateX(mouse.y * rotation_speed);
			g_world_matrix = g_world_matrix * rotate_matrix;
		}

		if ( mouse.z )
		{
			float scale = 1.0f + mouse.z / 1000.0f;
			g_world_matrix.Scale(scale, scale, scale);
		}
	}
}

void GetUserInput(void)
{
	if ( g_bFPS_Camera )
	{
		GetUserInput_FPSCamera();
	}
	else
	{
		GetUserInput_RotateObject();
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

	printf("\nSelected %s device for rendering.\n", device);

	char *model = model = "..\\..\\models\\jet.gma";
	//char *model = model = "..\\..\\models\\teapot2.gma";
	CGutModel::SetTexturePath("../../textures/");

	// 載入模型
	g_Model.Load_ASCII(model);

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
	g_world_matrix.Identity();

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
	//
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
