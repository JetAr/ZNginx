#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutTexture_OpenGL.h"
#include "GutFont_OpenGL.h"
#include "GutModel_OpenGL.h"
#include "GutHeightmap_OpenGL.h"

#include "render_data.h"

static CGutFontOpenGL g_Font;
static CGutHeightmapOpenGL g_Heightmap;

static Matrix4x4 g_proj_matrix;

bool InitResourceOpenGL(void)
{
	// 設定投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	// 使用2D貼圖功能
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	g_Font.SetFontSize(32, 32);
	g_Font.SetFontAdvance(20, 30);
	g_Font.SetConsoleSize(32, 16);
	g_Font.LoadTexture("../../textures/ascii_couriernew.tga");

	if ( !g_Heightmap.LoadHeightmapTexture("../../textures/GrandCanyon.tga") )
		return false;
	g_Heightmap.SetRange(Vector4(-50.0f, -50.0f, -10.0f), Vector4(50.0f, 50.0f, 10.0f));
	if ( !g_Heightmap.BuildMesh(255,255) )
		return false;

	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.5f, 1000.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&g_proj_matrix);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	g_Font.SetConsoleResolution(width, height);
	g_Font.BuildMesh();
}
//
static void SetupLighting(void)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	// 設定環境光
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float *)&g_vAmbientLight);

	int LightID = GL_LIGHT0;

	glEnable(LightID);

	glLightfv(LightID, GL_POSITION, &g_Lights[0].m_Direction[0]);
	glLightfv(LightID, GL_DIFFUSE, &g_Lights[0].m_Diffuse[0]);
	glLightfv(LightID, GL_SPECULAR, &g_Lights[0].m_Specular[0]);

	glLightf(LightID, GL_CONSTANT_ATTENUATION,	1.0f);
	glLightf(LightID, GL_LINEAR_ATTENUATION,	0.0f);
	glLightf(LightID, GL_QUADRATIC_ATTENUATION, 0.0f);
}
// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 vw_matrix = world_matrix * view_matrix;
	//
	glMatrixMode(GL_MODELVIEW);
	SetupLighting();
	//
	glPolygonMode(GL_FRONT_AND_BACK, g_bWireframe ? GL_LINE : GL_FILL);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&vw_matrix);
	g_Heightmap.Render();
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
