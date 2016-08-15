#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "render_data.h"

static Matrix4x4 g_proj_matrix;

static GLuint g_VertexShader = 0;
static GLuint g_PixelShader = 0;
static GLuint g_Shader = 0;

bool InitResourceOpenGL(void)
{
	if ( NULL==glCreateProgram )
	{
		printf("Driver does not support GLSL\n");
		return false;
	}
	if ( NULL==glBindFramebufferEXT )
	{
		printf("Driver does not support framebuffer object\n");
		return false;
	}

	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);

	glMatrixMode(GL_MODELVIEW);	
	glEnable(GL_DEPTH_TEST);

	g_VertexShader = GutLoadVertexShaderOpenGL_GLSL("../../shaders/VertexCompression.glvs");
	g_PixelShader = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/VertexCompression.glfs");
	g_Shader = GutCreateProgram(g_VertexShader, g_PixelShader);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	glDeleteShader(g_VertexShader);
	glDeleteShader(g_PixelShader);
	glDeleteProgram(g_Shader);

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClearColor(0.4f, 0.4f, 0.4f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 套用矩陣
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 wv_matrix = object_matrix * view_matrix;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &wv_matrix);
	glMatrixMode(GL_TEXTURE);
	glLoadMatrixf( (float *) &object_matrix);
	// 套用shader
	glUseProgram(g_Shader);
	// 設定頂點資料來源
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(CompressedVertex), g_pCompressedVertices[0].m_Position);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(CompressedVertex), g_pCompressedVertices[0].m_Normal);
	// 畫球
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
