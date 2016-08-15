#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "render_data.h"

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"

static Matrix4x4 g_proj_matrix;

static CGutModel_OpenGL g_Model_OpenGL;

static GLuint g_depthtexture = 0;
static GLuint g_depthbuffer = 0;

static GLuint g_ZPassVS = 0;
static GLuint g_ZPassPS = 0;
static GLuint g_ZPassProgram = 0;

static GLuint g_ShadowmapVS = 0;
static GLuint g_ShadowmapPS = 0;
static GLuint g_ShadowmapZComparePS = 0;
static GLuint g_ShadowmapProgram = 0;
static GLuint g_ShadowmapZCompareProgram = 0;

static Matrix4x4 g_light_matrix, g_remap_matrix;
static Matrix4x4 g_light_proj_matrix, g_light_view_matrix;

static bool g_bUseZCompare = false;

bool InitResourceOpenGL(void)
{
	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);

	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);

	GutCreateRenderTargetOpenGL(g_texture_width, g_texture_height, &g_depthbuffer, 
		0, NULL, 
		GL_DEPTH_COMPONENT24, &g_depthtexture);

	glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);
	glDrawBuffer (GL_TRUE);
	glReadBuffer (GL_TRUE);

	glBindTexture(GL_TEXTURE_2D, g_depthtexture);

	if ( g_bUseZCompare )
	{
		// 啟動自動比較的功能
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE_ARB);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
	}
	else
	{
		// 關閉自動比較的功能
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
	}

	// 畫Shadowmap的Shader
	g_ZPassVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/ZPass.glvs");
	g_ZPassPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/ZPass.glfs");
	if ( 0==g_ZPassVS || 0==g_ZPassPS )
		return false;

	g_ZPassProgram = GutCreateProgram(g_ZPassVS, g_ZPassPS);

	// 畫陰影效果用的Shader
	g_ShadowmapVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/Shadowmap.glvs");
	g_ShadowmapPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/Shadowmap_PCF.glfs");
	g_ShadowmapZComparePS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/ShadowmapZCompare.glfs");
	if ( 0==g_ShadowmapVS || 0==g_ShadowmapPS || 0==g_ShadowmapZComparePS )
		return false;

	g_ShadowmapProgram = GutCreateProgram(g_ShadowmapVS, g_ShadowmapPS);
	g_ShadowmapZCompareProgram = GutCreateProgram(g_ShadowmapVS, g_ShadowmapZComparePS);

	// 轉換到貼圖座標系的矩陣
	g_remap_matrix.Identity();
	g_remap_matrix.Scale(0.5f, 0.5f, 0.5f);
	g_remap_matrix[3].Set(0.5f, 0.5f, 0.5f, 1.0f);

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
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
}
