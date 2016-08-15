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
	}
}

void main(void)
{
	// 使用OpenGL來繪圖
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
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
