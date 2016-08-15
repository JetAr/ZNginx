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
	printf("Press (1) Switch to directional light\n");
	printf("Press (2) Switch to point light\n");
	printf("Press (3) Switch to spot light\n");
	printf("Press (d) To turn on/off diffuse reflection\n");
	printf("Press (s) To turn on/off specular reflection\n");
	printf("Press (+) To increase specular shininess\n");
	printf("Press (-) To decrease specular shininess\n");
}

void KeyPressed(int key)
{
	switch(key)
	{
	case GUTKEY_1:
	case GUTKEY_NUMPAD1:
		g_iSelectedLight = 0;
		g_Lights[0].m_bEnabled = !g_Lights[0].m_bEnabled;
		printf("Directional Light\n");
		break;
	case GUTKEY_2:
	case GUTKEY_NUMPAD2:
		g_iSelectedLight = 1;
		g_Lights[1].m_bEnabled = !g_Lights[1].m_bEnabled;
		printf("Point Light\n");
		break;
	case GUTKEY_3:
	case GUTKEY_NUMPAD3:
		g_iSelectedLight = 2;
		g_Lights[2].m_bEnabled = !g_Lights[2].m_bEnabled;
		printf("Spot Light\n");
		break;
	case GUTKEY_ADD:
		g_fMaterialShininess += 5;
		printf("Specular Shinness=%f\n", g_fMaterialShininess);
		break;
	case GUTKEY_SUBTRACT:
		g_fMaterialShininess -= 5;
		if ( g_fMaterialShininess < 0 ) g_fMaterialShininess = 0;
		printf("Specular Shinness=%f\n", g_fMaterialShininess);
		break;

	case GUTKEY_D:
		g_bDiffuseReflection = !g_bDiffuseReflection;
		if ( g_bDiffuseReflection )
		{
			g_vMaterialDiffuse = Vector4::GetOne();
			printf("Diffuse on\n");
		}
		else
		{
			g_vMaterialDiffuse = Vector4::GetZero();
			printf("Diffuse off\n");
		}
		break;
	case GUTKEY_S:
		g_bSpecularReflection = !g_bSpecularReflection;
		if ( g_bSpecularReflection )
		{
			g_vMaterialSpecular = Vector4::GetOne();
			printf("Specular on\n");
		}
		else
		{
			g_vMaterialSpecular = Vector4::GetZero();
			printf("Specular off\n");
		}
		break;
	case GUTKEY_UP:
		g_world_matrix.Scale(1.1f, 1.1f, 1.1f);	
		break;
	case GUTKEY_DOWN:
		g_world_matrix.Scale(0.9f, 0.9f, 0.9f);
		break;
	default:
		PrintHelp();
		break;
	}
}

void DetectKeypress(char keyboard_state[256])
{
	for ( int i=0; i<255; i++ )
	{
		if ( g_keyboard_state[i]==0 && keyboard_state[i]!=0 )
		{
			KeyPressed(i);
		}
		g_keyboard_state[i] = keyboard_state[i];
	}
}

void GetUserInput(void)
{
	// 讀取鍵盤
	char keyboard_state[256];
	if ( GutReadKeyboard(keyboard_state) )
	{
		DetectKeypress(keyboard_state);
	}

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
	// 內定使用Direct3D9來繪圖
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

	g_view_matrix.Identity();
	g_world_matrix.Identity();

	GutCreateGrids(g_iNumGrids, g_iNumGrids, 
		&g_pGridVertices, g_iNumGridVertices, 
		&g_pGridIndices, g_iNumGridIndices,
		g_iNumGridTriangles);

	GutInputInit();
	memset(g_keyboard_state, 0, sizeof(g_keyboard_state));

	// 載入shader
	if ( !init_resource() )
	{
		release_resource();
		printf("Failed to load resources\n");
		exit(0);
	}

	g_bDiffuseReflection = true;
	g_bSpecularReflection = true;

	// 物件材質
	g_vMaterialAmbient.Set(1.0f);
	g_vMaterialDiffuse.Set(1.0f);
	g_vMaterialSpecular.Set(1.0f);
	g_vMaterialEmissive.Set(0.0f);
	g_fMaterialShininess = 100.0f;

	// 設定光源
	g_vAmbientLight.Set(0.1f, 0.1f, 0.1f, 1.0f);

	// 把第一個光設定成方向光
	{
		g_Lights[0].m_eType = LIGHT_DIRECTIONAL;
		g_Lights[0].m_bEnabled = true;

		g_Lights[0].m_vDirection.Set(0.0f, 0.0f, 1.0f, 0.0f);
		g_Lights[0].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Lights[0].m_vDiffuseColor.Set(0.6f, 0.0f, 0.0f, 1.0f);
		g_Lights[0].m_vSpecularColor.Set(1.0f, 0.0f, 0.0f, 1.0f);
	}
	// 第二個光設成點光源
	{
		g_Lights[1].m_eType = LIGHT_POINT;
		g_Lights[1].m_bEnabled = false;

		g_Lights[1].m_vPosition.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Lights[1].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Lights[1].m_vDiffuseColor.Set(0.0f, 0.6f, 0.0f, 1.0f);
		g_Lights[1].m_vSpecularColor.Set(0.0f, 1.0f, 0.0f, 1.0f);
		g_Lights[1].m_vAttenuation.Set(1.0f, 0.0f, 1.0f);
	}
	// 第三個光設成聚光燈
	{
		g_Lights[2].m_eType = LIGHT_SPOT;
		g_Lights[2].m_bEnabled = false;

		g_Lights[2].m_vPosition.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Lights[2].m_vDirection.Set(0.0f, 0.0f,-1.0f, 0.0f);
		g_Lights[2].m_vAmbientColor.Set(0.0f, 0.0f, 0.0f, 1.0f);
		g_Lights[2].m_vDiffuseColor.Set(0.0f, 0.0f, 0.8f, 1.0f);
		g_Lights[2].m_vSpecularColor.Set(0.0f, 0.0f, 1.0f, 1.0f);
		g_Lights[2].m_vAttenuation.Set(1.0f, 0.0f, 1.0f);

		g_Lights[2].m_fSpotlightCutoff = 60.0f;
		g_Lights[2].m_fSpotlightExponent = 20.0f;
	}

	// 主回圈
	while( GutProcessMessage() )
	{
		GetUserInput();
		render();
	}

	// 釋放棋盤格模型記憶體空間	
	GutReleaseGrids(&g_pGridVertices, &g_pGridIndices);
	// 卸載shader
	release_resource();
	// 關閉OpenGL/DirectX繪圖裝置
	GutReleaseGraphicsDevice();
}
