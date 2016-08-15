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
const float g_fSpeed = 200.0f;

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();

	int x,y,buttons[3];
	GutGetMouseState(x, y, buttons);

	if ( buttons[0] )
	{
		Vector4 offset(g_iWidth*0.5f, g_iHeight*0.5f, 0.0f, 0.0f);
		g_vPosition[0] = x;
		g_vPosition[1] = (g_iHeight-y);
		g_vPosition = g_vPosition - offset;
	}

	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_ROTATEOBJECT);
	//g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_FPSCAMERA);
}

void KeyDown_1(void)
{
	if ( g_iMode & 0x01 )
	{
		g_iMode &= ~0x01;
	}
	else
	{
		g_iMode |= 0x01;
	}
}

void KeyDown_2(void)
{
	if ( g_iMode & 0x02 )
	{
		g_iMode &= ~0x02;
	}
	else
	{
		g_iMode |= 0x02;
	}
}

void KeyDown_3(void)
{
	if ( g_iMode & 0x04 )
	{
		g_iMode &= ~0x04;
	}
	else
	{
		g_iMode |= 0x04;
	}
}

void KeyPressed_LEFT(void)
{
	g_vPosition[0] -= g_fSpeed * g_fFrame_Time;
	if ( g_vPosition[0]<-g_iWidth/2.0f )
		g_vPosition[0] = -g_iWidth/2.0f;
}

void KeyPressed_RIGHT(void)
{
	g_vPosition[0] += g_fSpeed * g_fFrame_Time;
	if ( g_vPosition[0]>g_iWidth/2.0f )
		g_vPosition[0] = g_iWidth/2.0f;
}

void KeyPressed_UP(void)
{
	g_vPosition[1] += g_fSpeed * g_fFrame_Time;
	if ( g_vPosition[1]>g_iHeight/2.0f )
		g_vPosition[1] = g_iHeight/2.0f;
}

void KeyPressed_DOWN(void)
{
	g_vPosition[1] -= g_fSpeed * g_fFrame_Time;
	if ( g_vPosition[1]<-g_iHeight/2.0f )
		g_vPosition[1] = -g_iHeight/2.0f;
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
#endif
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

	g_Lights[0].m_vPosition.Set(0.0f, 5.0f, 0.0f);
	g_Lights[0].m_vDiffuse.Set(1.0f);

	GutInputInit();

	CGutModel::SetTexturePath("../../models/j2f/");
	g_Model.Load_ASCII("../../models/j2f/j2f.gma");
	//g_Model.Load_ASCII("../../models/jet.gma");

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);

	GutRegisterKeyPressed(GUTKEY_UP, KeyPressed_UP);
	GutRegisterKeyPressed(GUTKEY_DOWN, KeyPressed_DOWN);
	GutRegisterKeyPressed(GUTKEY_LEFT, KeyPressed_LEFT);
	GutRegisterKeyPressed(GUTKEY_RIGHT, KeyPressed_RIGHT);

	g_Control.SetCamera(Vector4(0.0f, 0.0f, 5.0f), Vector4(0.0f, 0.0f, 0.0f), Vector4(0.0f, 1.0f, 0.0f) );

	g_Lights[0].m_bEnabled = true;
	g_Lights[0].m_iLightType = LIGHT_DIRECTIONAL;
	g_Lights[0].m_vDiffuse.Set(1.0f);
	g_Lights[0].m_vSpecular.Set(1.0f);
	g_Lights[0].m_vDirection.Set(0.0f, 1.0f, 1.0f); g_Lights[0].m_vDirection.Normalize();

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
