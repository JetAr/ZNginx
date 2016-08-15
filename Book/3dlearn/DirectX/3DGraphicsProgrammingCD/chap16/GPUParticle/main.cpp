#include <stdio.h>
#include <conio.h>

#include "Gut.h"
#include "GutInput.h"
#include "GutTimer.h"

#include "render_dx9.h"
#include "render_opengl.h"
#include "render_data.h"

GutTimer g_Timer;
const float g_fSpeed = 200.0f;

void GetUserInput(void)
{
	g_fTimeAdvance = g_Timer.Stop();
	g_fTimeElapsed += g_fTimeAdvance;

	g_Timer.Restart();

	GutReadKeyboard();

	g_Control.Update(g_fTimeAdvance, CGutUserControl::CONTROLLER_ROTATEOBJECT);
}

void FrameMove(void)
{
	g_num_particles += g_fTimeElapsed * g_fEmitRate;

	if ( g_num_particles > g_max_particles )
	{
		g_num_particles = g_max_particles;
	}
}

void KeyDown_1(void)
{
	if ( g_iMode & 0x01 )
	{
		g_iMode &= ~(0x01);
	}
	else
	{
		g_iMode |= 0x01;
	}
	printf("mask=%b\n", g_iMode);
}

void KeyDown_2(void)
{
	if ( g_iMode & 0x02 )
	{
		g_iMode &= ~(0x02);
	}
	else
	{
		g_iMode |= 0x02;
	}

	printf("mask=%b\n", g_iMode);
}

void main(void)
{
	// 內定使用DirectX 9來繪圖
	char *device = "dx9";
	void (*render)(void) = RenderFrameDX9;
	bool (*init_resource)(void) = InitResourceDX9;
	bool (*release_resource)(void) = ReleaseResourceDX9;
	void (*resize_func)(int width, int height) = ResizeWindowDX9;

	printf("Press\n(1) for Direct3D9\n(2) for OpenGL\n\n");
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

	GutResizeFunc( resize_func );
	// 在(100,100)的位置開啟一個大小為(512x512)的視窗
	GutCreateWindow(100, 100, 512, 512, device);
	// 做OpenGL或DirectX初始化
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	//	srand(timeGetTime());

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);

	g_Control.SetCamera(Vector4(0.0f, 2.0f, 5.0f), Vector4(0.0f, 2.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f) );

	g_pParticleArray = new sParticle[g_max_particles];
	sParticle *pParticle = g_pParticleArray;

	int vindex = 0;

	for ( int y=0; y<g_texture_height; y++ )
	{
		for ( int x=0; x<g_texture_width; x++ )
		{
			pParticle->m_Position[0] = (float)x/(float)g_texture_width;
			pParticle->m_Position[1] = (float)y/(float)g_texture_height;
			pParticle++;
		}
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
		GetUserInput();
		FrameMove();
		render();
	}
	// 卸載shader
	release_resource();
	//
	delete [] g_pParticleArray;
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
