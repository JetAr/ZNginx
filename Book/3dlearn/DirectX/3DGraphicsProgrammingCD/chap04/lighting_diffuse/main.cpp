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

static int g_iLightSource = LIGHT_DIRECTIONAL;
static int g_iNumGrids = 15;
static char g_keyboard_state[256];

void KeyDown_1(void)
{
	g_Lights[0].m_bEnabled = !g_Lights[0].m_bEnabled;
	printf("Directional Light %s\n", g_Lights[0].m_bEnabled ? "on" : "off");
}

void KeyDown_2(void)
{
	g_Lights[1].m_bEnabled = !g_Lights[1].m_bEnabled;
	printf("Point Light %s\n", g_Lights[1].m_bEnabled ? "on" : "off");
}

void KeyDown_3(void)
{
	g_Lights[2].m_bEnabled = !g_Lights[2].m_bEnabled;
	printf("Spot Light %s\n", g_Lights[2].m_bEnabled ? "on" : "off");
}

void GetUserInput(void)
{
	// 讀取鍵盤
	GutReadKeyboard();
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

		// 滾輪可以增加/減少棋盤格子的數目
		if ( mouse.z )
		{
			g_iNumGrids += mouse.z > 0 ? 1 : -1;

			if ( g_iNumGrids <=0 ) g_iNumGrids=1;
			if ( g_iNumGrids > g_iMaxNumGrids ) g_iNumGrids = g_iMaxNumGrids;

			GutReleaseGrids(&g_pGridVertices, &g_pGridIndices);
			GutCreateGrids(g_iNumGrids, g_iNumGrids, 
				&g_pGridVertices, g_iNumGridVertices, 
				&g_pGridIndices, g_iNumGridIndices,
				g_iNumGridTriangles);

			printf("Generate %dx%d grids\n", g_iNumGrids, g_iNumGrids);
		}
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

	//int c = getche();
	int c = '1';

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

		/*
		#ifdef _ENABLE_DX10_
		case '3':
		device = "dx10";
		init_resource = InitResourceDX10;
		release_resource = ReleaseResourceDX10;
		render = RenderFrameDX10;
		resize_func = ResizeWindowDX10;
		break;
		#endif
		*/
	}

	printf("\nSelected %s device for rendering.\n", device);
	printf("Press\n(1) to turn on/off directional light.\n(2) to turn on/off point light.\n(3) to turn on/off spot light.\n");

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

	GutCreateGrids(g_iNumGrids, g_iNumGrids, 
		&g_pGridVertices, g_iNumGridVertices, 
		&g_pGridIndices, g_iNumGridIndices,
		g_iNumGridTriangles);

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);

	memset(g_keyboard_state, 0, sizeof(g_keyboard_state));

	// 載入繪圖資料
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	g_vGlobal_AmbientLight.Set(0.1f, 0.1f, 0.1f, 1.0f);

	const int DirectionalLightID = 0;
	const int PointLightID = 1;
	const int SpotLightID = 2;
	// 把第一個光設定成方向光
	{
		g_Lights[0].m_eType = LIGHT_DIRECTIONAL;
		g_Lights[0].m_bEnabled = true;

		g_Lights[0].m_vDirection.Set(0.0f, 0.0f, -1.0f, 0.0f);
		g_Lights[0].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Lights[0].m_vDiffuseColor.Set(1.0f, 0.0f, 0.0f, 1.0f);
	}
	// 第二個光設成點光源
	{
		g_Lights[1].m_eType = LIGHT_POINT;
		g_Lights[1].m_bEnabled = false;

		g_Lights[1].m_vPosition.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Lights[1].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Lights[1].m_vDiffuseColor.Set(0.0f, 1.0f, 0.0f, 1.0f);
		g_Lights[1].m_vAttenuation.Set(0.0f, 0.0f, 1.0f);
	}
	// 第三個光設成聚光燈
	{
		g_Lights[2].m_eType = LIGHT_SPOT;
		g_Lights[2].m_bEnabled = false;

		g_Lights[2].m_vPosition.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Lights[2].m_vDirection.Set(0.0f, 0.0f,-1.0f, 0.0f);
		g_Lights[2].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Lights[2].m_vDiffuseColor.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Lights[2].m_vAttenuation.Set(1.0f, 1.0f, 0.0f);

		g_Lights[2].m_fSpotlightCutoff = 60.0f;
		if ( GutGetGraphicsDeviceType()==GUT_DX9 )
		{	
			// Direct3D 9有inner cone, 而且隨角度衰減的算法不同
			g_Lights[2].m_fSpotLightInnerCone = 0.0f;
			g_Lights[2].m_fSpotlightExponent = 4.0f;
		}
		else
		{
			// OpenGL的聚光燈沒有inner cone 
			g_Lights[2].m_fSpotlightExponent = 20.0f;
		}
	}

	// 主回圈
	while( GutProcessMessage() )
	{
		GetUserInput();
		render();
	}

	// 釋放棋盤格模型記憶體空間	
	GutReleaseGrids(&g_pGridVertices, &g_pGridIndices);
	// 卸載繪圖資料
	release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
