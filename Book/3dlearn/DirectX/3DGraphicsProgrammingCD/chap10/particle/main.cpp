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
sParticleEmitter g_emitter;

bool g_bDirty = false;
float g_fFrame_Time = 0.0f;

void GetUserInput(void)
{
	g_fFrame_Time = g_Timer.Stop();
	g_Timer.Restart();
	GutReadKeyboard();
	//g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_ROTATEOBJECT);
	g_Control.Update(g_fFrame_Time, CGutUserControl::CONTROLLER_FPSCAMERA);
}

void UpdateMessage(const char *msg)
{
	if ( g_bDirty )
	{
		printf("\n");
		g_bDirty = false;
	}
	printf("%s\n", msg);
}

void KeyDown_1(void)
{
	UpdateMessage("Updating emitter rate");
	g_iMode = 1;
}

void KeyDown_2(void)
{
	UpdateMessage("Updating emitter theta");
	g_iMode = 2;
}

void KeyDown_3(void)
{
	UpdateMessage("Updating particle speed");
	g_iMode = 3;
}

void KeyDown_4(void)
{
	UpdateMessage("Updating particle size");
	g_iMode = 4;
}

void KeyDown_5(void)
{
	UpdateMessage("Updating force");
	g_iMode = 5;
}

void KeyDown_6(void)
{
	g_bPointSprite = !g_bPointSprite;
	printf("PointSprite %s\n", g_bPointSprite ? "on" : "off");
}

void UpdateParameter(float multiplier)
{
	g_bDirty = true;
	sParticleEmitter &emitter = g_Particle.GetEmitter();

	switch(g_iMode)
	{
	case 1:
		g_Particle.m_fEmitRateScale += 0.01f * multiplier;
		if ( g_Particle.m_fEmitRateScale < 0.0f )
			g_Particle.m_fEmitRateScale = 0.0f;
		if ( g_Particle.m_fEmitRateScale > 1.0f )
			g_Particle.m_fEmitRateScale = 1.0f;
		printf("emitter rate = %5.2f\r", g_Particle.m_fEmitRateScale * emitter.m_fEmitRate);
		break;

	case 2:
		emitter.m_fEmitTheta += FastMath::DegToRad(1.0f) * multiplier;
		if ( emitter.m_fEmitTheta < 0.0f )
			emitter.m_fEmitTheta = 0;
		printf("emitter theta = %5.2f\r", FastMath::RadToDeg(emitter.m_fEmitTheta));
		break;

	case 3:
		emitter.m_fParticleInitSpeed[0] += 0.01f * multiplier;
		emitter.m_fParticleInitSpeed[1] += 0.01f * multiplier;
		if ( emitter.m_fParticleInitSpeed[0]<0 ||
			emitter.m_fParticleInitSpeed[1]<0 )
		{
			emitter.m_fParticleInitSpeed[0] = 0;
			emitter.m_fParticleInitSpeed[1] = 0;
		}

		printf("particle speed = [%5.2f - %5.2f]\r", 
			emitter.m_fParticleInitSpeed[0],
			emitter.m_fParticleInitSpeed[1]);
		break;

	case 4:
		emitter.m_fParticleSize[0] += 0.01f * multiplier;
		emitter.m_fParticleSize[1] += 0.01f * multiplier;
		if ( emitter.m_fParticleSize[0]<0 ||
			emitter.m_fParticleSize[1]<0 )
		{
			emitter.m_fParticleSize[0] = 0;
			emitter.m_fParticleSize[1] = 0;
		}

		printf("particle size = [%5.2f - %5.2f]\r", 
			emitter.m_fParticleSize[0],
			emitter.m_fParticleSize[1]);
		break;

	case 5:
		{
			float scale = 1.0f + 0.01f * multiplier;
			g_Particle.m_vForce *= scale;
			printf("force = (%5.2f,%5.2f,%5.2f)\r", 
				g_Particle.m_vForce[0], g_Particle.m_vForce[1], g_Particle.m_vForce[2]);
		}
		break;
	default:
		g_bDirty = false;
		break;
	}
}

void KeyPressed_ADD(void)
{
	UpdateParameter(1.0f);
}

void KeyPressed_MINUS(void)
{
	UpdateParameter(-1.0f);
}

void framemove(void)
{
	g_Particle.m_Matrix = g_Control.GetObjectMatrix();
	g_Particle.Simulate(g_fFrame_Time);

	if ( g_bPointSprite )
	{
		g_Particle.BuildPointSprite();
	}
	else
	{
		g_Particle.BuildMesh(g_Control.GetCameraMatrix());
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

	printf("\nSelected %s device for rendering.\n", device);

	GutResizeFunc( resize_func );
	// 在(100,100)的位置開啟一個大小為(512x512)的視窗
	GutCreateWindow(100, 100, 512, 512, device);
	// 做OpenGL或DirectX初始化
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}
	//
	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);
	GutRegisterKeyDown(GUTKEY_4, KeyDown_4);
	GutRegisterKeyDown(GUTKEY_5, KeyDown_5);
	GutRegisterKeyDown(GUTKEY_6, KeyDown_6);

	GutRegisterKeyPressed(GUTKEY_EQUALS, KeyPressed_ADD);
	GutRegisterKeyPressed(GUTKEY_MINUS, KeyPressed_MINUS);
	GutRegisterKeyPressed(GUTKEY_ADD, KeyPressed_ADD);
	GutRegisterKeyPressed(GUTKEY_SUBTRACT, KeyPressed_MINUS);

	g_Control.SetCamera(Vector4(0.0f, 3.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f), Vector4(0.0f, 0.0f, 1.0f) );
	g_Control.SetMoveSpeed(5.0f);

	//
	g_emitter.m_fEmitRate = 300;
	g_emitter.m_fEmitTheta = FastMath::DegToRad(15.0f);
	g_emitter.m_fParticleInitSpeed[0] = 1.0f;
	g_emitter.m_fParticleInitSpeed[1] = 2.0f;
	g_emitter.m_fParticleLife[0] = 1.5f;
	g_emitter.m_fParticleLife[1] = 2.0f;
	g_emitter.m_fParticleSize[0] = 0.05f;
	g_emitter.m_fParticleSize[1] = 0.10f;

	g_Particle.SetEmitter(g_emitter);
	g_Particle.SetForce(Vector4(0.0f, 0.0f, -1.0f));

	// 載入shader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	g_Timer.Restart();
	// 主回圈
	while( GutProcessMessage() )
	{
		GetUserInput();
		//
		framemove();
		// 畫出矩形
		render();
	}
	// 卸載shader
	release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
