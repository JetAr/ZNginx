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
static bool g_bDiffuseReflection = true;
static bool g_bSpecularReflection = true;

void PrintHelp(void)
{
	printf("Press (1) To turn on/off directional light\n");
	printf("Press (2) To turn on/off point light\n");
	printf("Press (3) To turn on/off spot light\n");
	printf("Press (d) To turn on/off diffuse reflection\n");
	printf("Press (s) To turn on/off specular reflection\n");
	printf("Press (n) To turn on/off auto-normalize\n");
	printf("Press (p) To switch specular lighting equation\n");
	printf("Press (+) To increase specular shininess\n");
	printf("Press (-) To decrease specular shininess\n");
}

void KeyDown_1(void)
{
	g_Simulation.m_Lights[0].m_bEnabled = !g_Simulation.m_Lights[0].m_bEnabled;
	printf("Directional Light %s\n", g_Simulation.m_Lights[0].m_bEnabled ? "on" : "off");
}

void KeyDown_2(void)
{
	g_Simulation.m_Lights[1].m_bEnabled = !g_Simulation.m_Lights[1].m_bEnabled;
	printf("Point Light %s\n", g_Simulation.m_Lights[1].m_bEnabled ? "on" : "off");
}

void KeyDown_3(void)
{
	g_Simulation.m_Lights[2].m_bEnabled = !g_Simulation.m_Lights[2].m_bEnabled;
	printf("Spot Light %s\n", g_Simulation.m_Lights[2].m_bEnabled ? "on" : "off");
}

void KeyDown_D(void)
{
	g_bDiffuseReflection = !g_bDiffuseReflection;
	if ( g_bDiffuseReflection )
	{
		g_Simulation.m_vMaterialDiffuse = Vector4::GetOne();
		printf("Diffuse on\n");
	}
	else
	{
		g_Simulation.m_vMaterialDiffuse = Vector4::GetZero();
		printf("Diffuse off\n");
	}
}

void KeyDown_S(void)
{
	g_bSpecularReflection = !g_bSpecularReflection;
	if ( g_bSpecularReflection )
	{
		g_Simulation.m_vMaterialSpecular = Vector4::GetOne();
		printf("Specular on\n");
	}
	else
	{
		g_Simulation.m_vMaterialSpecular = Vector4::GetZero();
		printf("Specular off\n");
	}
}

void KeyDown_P(void)
{
	g_Simulation.m_bPhongSpecular = !g_Simulation.m_bPhongSpecular;
	if ( g_Simulation.m_bPhongSpecular )
	{
		printf("Phong Specular on\n");
	}
	else
	{
		printf("Phong Specular off\n");
	}
}

void KeyDown_L(void)
{
	g_Simulation.m_bLocalViewer = !g_Simulation.m_bLocalViewer;
	if ( g_Simulation.m_bLocalViewer )
	{
		printf("Local Viewer on\n");
	}
	else
	{
		printf("Local Viewer off\n");
	}
}

void KeyDown_N(void)
{
	g_Simulation.m_bAutoNormalize = !g_Simulation.m_bAutoNormalize;
	printf("Auto normalize %s\n", g_Simulation.m_bAutoNormalize ? "on" : "off" );
}

void KeyPressed_ADD(void)
{
	g_Simulation.m_fMaterialShininess += 1.0f;
	printf("Specular Shinness=%f\n", g_Simulation.m_fMaterialShininess);
}

void KeyPressed_SUBTRACT(void)
{
	g_Simulation.m_fMaterialShininess -= 1.0f;
	if ( g_Simulation.m_fMaterialShininess < 0 ) g_Simulation.m_fMaterialShininess = 0;
	printf("Specular Shinness=%f\n", g_Simulation.m_fMaterialShininess);
}

void KeyPressed_UP(void)
{
	g_Simulation.m_WorldMatrix.Scale(1.05f, 1.05f, 1.05f);	
	g_Simulation.UpdateMatrix();
}

void KeyPressed_DOWN(void)
{
	g_Simulation.m_WorldMatrix.Scale(0.95f, 0.95f, 0.95f);	
	g_Simulation.UpdateMatrix();
}

void GetUserInput(void)
{
	// 讀取鍵盤
	GutReadKeyboard();
	// 讀取滑鼠
	GutMouseInfo mouse;
	//
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
			g_Simulation.m_WorldMatrix = g_Simulation.m_WorldMatrix * rotate_matrix;
			g_Simulation.SetWorldMatrix(g_Simulation.m_WorldMatrix);
		}

		// 滾輪可以增加/減少棋盤格子的數目
		if ( mouse.z )
		{
			g_iNumGrids += mouse.z > 0 ? 1 : -1;

			if ( g_iNumGrids <=0 ) g_iNumGrids=1;
			if ( g_iNumGrids > g_iMaxNumGrids ) g_iNumGrids = g_iMaxNumGrids;

			ReleaseGridsResource(&g_pGridVertices, &g_pGridIndices);
			GenerateGrids(g_iNumGrids, g_iNumGrids, 
				&g_pGridVertices, g_iNumGridVertices, 
				&g_pGridIndices, g_iNumGridIndices,
				g_iNumGridTriangles);

			delete [] g_pGridVerticesDX9;
			g_pGridVerticesDX9 = new Vertex_DX9[g_iNumGridVertices];

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

	printf("\nSelected %s device for rendering.\n", device);
	PrintHelp();

	GutResizeFunc( resize_func );

	// 在(100,100)的位置開啟一個大小為(512x512)的視窗
	GutCreateWindow(100, 100, 512, 512, device);

	// 做OpenGL或DirectX初始化
	if ( !GutInitGraphicsDevice(device) )
	{
		printf("Failed to initialize %s device\n", device);
		exit(0);
	}

	g_Simulation.m_ViewMatrix.Identity();
	g_Simulation.m_WorldMatrix.Identity();
	g_Simulation.m_WorldViewMatrix.Identity();

	GenerateGrids(g_iNumGrids, g_iNumGrids, 
		&g_pGridVertices, g_iNumGridVertices, 
		&g_pGridIndices, g_iNumGridIndices,
		g_iNumGridTriangles);

	g_pGridVerticesDX9 = new Vertex_DX9[g_iNumGridVertices];

	GutInputInit();

	GutRegisterKeyDown(GUTKEY_1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_NUMPAD1, KeyDown_1);
	GutRegisterKeyDown(GUTKEY_2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_NUMPAD2, KeyDown_2);
	GutRegisterKeyDown(GUTKEY_3, KeyDown_3);
	GutRegisterKeyDown(GUTKEY_NUMPAD3, KeyDown_3);

	GutRegisterKeyDown(GUTKEY_D, KeyDown_D);
	GutRegisterKeyDown(GUTKEY_S, KeyDown_S);
	GutRegisterKeyDown(GUTKEY_P, KeyDown_P);
	GutRegisterKeyDown(GUTKEY_N, KeyDown_N);

	GutRegisterKeyPressed(GUTKEY_UP, KeyPressed_UP);
	GutRegisterKeyPressed(GUTKEY_DOWN, KeyPressed_DOWN);
	GutRegisterKeyPressed(GUTKEY_ADD, KeyPressed_ADD);
	GutRegisterKeyPressed(GUTKEY_SUBTRACT, KeyPressed_SUBTRACT);


	// 載入shader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	// 設定計算的復雜度
	g_Simulation.m_bLocalViewer = true;
	g_Simulation.m_bAutoNormalize = false;
	g_Simulation.m_bPhongSpecular = false;

	// 物件材質
	g_Simulation.m_vMaterialAmbient.Set(1.0f);
	g_Simulation.m_vMaterialDiffuse.Set(1.0f);
	g_Simulation.m_vMaterialSpecular.Set(1.0f);
	g_Simulation.m_vMaterialEmissive.Set(0.0f);
	g_Simulation.m_fMaterialShininess = 100.0f;

	// 設定光源

	g_Simulation.m_vAmbientLight.Set(0.1f);

	// 把第一個光設定成方向光
	{
		g_Simulation.m_Lights[0].m_eType = LIGHT_DIRECTIONAL;
		g_Simulation.m_Lights[0].m_bEnabled = true;

		g_Simulation.m_Lights[0].m_vDirection.Set(0.0f, 0.0f, 1.0f, 0.0f);
		g_Simulation.m_Lights[0].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Simulation.m_Lights[0].m_vDiffuseColor.Set(0.6f, 0.0f, 0.0f, 1.0f);
		g_Simulation.m_Lights[0].m_vSpecularColor.Set(1.0f, 0.0f, 0.0f, 1.0f);
	}
	// 第二個光設成點光源
	{
		g_Simulation.m_Lights[1].m_eType = LIGHT_POINT;
		g_Simulation.m_Lights[1].m_bEnabled = false;

		g_Simulation.m_Lights[1].m_vPosition.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Simulation.m_Lights[1].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Simulation.m_Lights[1].m_vDiffuseColor.Set(0.0f, 0.6f, 0.0f, 1.0f);
		g_Simulation.m_Lights[1].m_vSpecularColor.Set(0.0f, 1.0f, 0.0f, 1.0f);
		g_Simulation.m_Lights[1].m_vAttenuation.Set(1.0f, 0.0f, 1.0f);
	}
	// 第三個光設成聚光燈
	{
		g_Simulation.m_Lights[2].m_eType = LIGHT_SPOT;
		g_Simulation.m_Lights[2].m_bEnabled = false;

		g_Simulation.m_Lights[2].m_vPosition.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Simulation.m_Lights[2].m_vDirection.Set(0.0f, 0.0f,-1.0f, 0.0f);
		g_Simulation.m_Lights[2].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Simulation.m_Lights[2].m_vDiffuseColor.Set(0.0f, 0.0f, 0.6f, 1.0f);
		g_Simulation.m_Lights[2].m_vSpecularColor.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Simulation.m_Lights[2].m_vAttenuation.Set(1.0f, 0.0f, 1.0f);

		g_Simulation.m_Lights[2].m_fSpotlightCutoff = 60.0f;
		g_Simulation.m_Lights[2].m_fSpotlightExponent = 20.0f;
	}

	// 主回圈
	while( GutProcessMessage() )
	{
		GetUserInput();
		g_Simulation.CalculateLighting(g_pGridVertices, g_iNumGridVertices);
		render();
	}
	// 釋放棋盤格模型記憶體空間	
	ReleaseGridsResource(&g_pGridVertices, &g_pGridIndices);
	delete [] g_pGridVerticesDX9;
	// 卸載shader
	release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
