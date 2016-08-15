#include <stdio.h>

#include "Gut.h"
#include "render_dx9.h"
#include "render_dx10.h"
#include "render_opengl.h"

void main(int argc, char *argv[])
{
	// 內定使用DirectX 9來繪圖
	char *device = "dx9";
	void (*render)(void) = RenderFrameDX9;
	bool (*init_resource)(void) = InitResourceDX9;
	bool (*release_resource)(void) = ReleaseResourceDX9;

	if ( argc > 1 )
	{
		// 如果命令列參數指定用OpenGL, 就改用OpenGL.
		if ( stricmp(argv[1], "opengl")==0 )
		{
			device = "opengl";
			init_resource = InitResourceOpenGL;
			release_resource = ReleaseResourceOpenGL;
			render = RenderFrameOpenGL;
		}
		// 如果命令列參數指定用DX10, 就改用DX10.
		else if ( stricmp(argv[1], "dx10")==0 )
		{
#ifdef _ENABLE_DX10_
			device = "dx10";
			init_resource = InitResourceDX10;
			release_resource = ReleaseResourceDX10;
			render = RenderFrameDX10;
#endif // _ENABLE_DX10_
		}
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

	// 主回圈
	while( GutProcessMessage() )
	{
		render();
	}

	// 卸載shader
	release_resource();

	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
