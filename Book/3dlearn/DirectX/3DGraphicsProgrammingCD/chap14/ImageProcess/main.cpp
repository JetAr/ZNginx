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
	GutReadKeyboard();
}

void UpdateSaturationMatrix(void)
{
	const float rwgt = 0.3086f;
	const float gwgt = 0.6094f;
	const float bwgt = 0.0820f;
	float inv_s = 1.0f - g_fSaturation;

	Matrix4x4 saturation_transform;
	Matrix4x4 subtract_matrix;

	g_SaturateMatrix.Identity();
	g_SaturateMatrix[0].Set( inv_s*rwgt + g_fSaturation, inv_s*rwgt, inv_s*rwgt, 0.0f );
	g_SaturateMatrix[1].Set( inv_s*gwgt, inv_s*gwgt + g_fSaturation, inv_s*gwgt, 0.0f );
	g_SaturateMatrix[2].Set( inv_s*bwgt, inv_s*bwgt, inv_s*bwgt + g_fSaturation, 0.0f );
}

void KeyPressed_ADD(void)
{
	if ( g_iPosteffect==4 )
	{
		g_fSaturation += g_fFrame_Time * 0.5f;
		if ( g_fSaturation > 2 )
			g_fSaturation = 2;
		printf("Saturation = %5.3f\r", g_fSaturation);
		UpdateSaturationMatrix();
	}
}

void KeyPressed_MINUS(void)
{
	if ( g_iPosteffect==4 )
	{
		g_fSaturation -= g_fFrame_Time * 0.5f;
		if ( g_fSaturation < 0 )
			g_fSaturation = 0;
		printf("Saturation = %5.3f\r", g_fSaturation);
		UpdateSaturationMatrix();
	}
}


void KeyDown_1(void)
{
	g_iPosteffect = 1;
	printf("Post effect off\n");
}

void KeyDown_2(void)
{
	g_iPosteffect = 2;
	printf("Grayscale effect\n");
}

void KeyDown_3(void)
{
	g_iPosteffect = 3;
	printf("Sepia effect\n");
}

void KeyDown_4(void)
{
	g_iPosteffect = 4;
	printf("Saturate effect\n");
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

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);
	GutRegisterKeyDown(GUTKEY_4, KeyDown_4);

	GutRegisterKeyPressed(GUTKEY_ADD, KeyPressed_ADD);
	GutRegisterKeyPressed(GUTKEY_SUBTRACT, KeyPressed_MINUS);
	GutRegisterKeyPressed(GUTKEY_MINUS, KeyPressed_MINUS);

	UpdateSaturationMatrix();

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
