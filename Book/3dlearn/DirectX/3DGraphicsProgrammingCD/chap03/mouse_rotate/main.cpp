#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"

#include "render_dx9.h"
#include "render_dx10.h"
#include "render_opengl.h"
#include "render_data.h"

void GetUserInput(void)
{
	GutMouseInfo mouse;
	GutReadMouse(&mouse);

	if ( mouse.button[0] )
	{
		// 如果有按下左鍵, 就旋轉物件
		float rz = mouse.x * 0.01f;
		float rx = mouse.y * -0.01f;

		Matrix4x4 rotation_matrix;
		rotation_matrix.RotateZ_Replace(rz);
		rotation_matrix.RotateX(rx);

		g_object_matrix = g_object_matrix * rotation_matrix;
	}

	if ( mouse.z )
	{
		// 如果滾輪有在動, 就使用滾輪來縮放物件
		float scale = 1.0f + mouse.z * 0.001f;
		g_object_matrix.Scale(scale, scale, scale);
	}
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


	// 載入shader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	// 初始化DirectX Input物件
	GutInputInit();
	// 先把矩陣設為單位矩陣
	g_object_matrix.Identity();

	// 主回圈
	while( GutProcessMessage() )
	{
		// 讀取滑鼠, 并計算新的旋轉及縮放矩陣
		GetUserInput();
		// 畫圖
		render();
	}

	// 卸載shader
	release_resource();

	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
