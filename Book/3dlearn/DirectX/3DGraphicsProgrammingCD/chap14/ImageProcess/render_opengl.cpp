#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutOpenGL.h"
#include "GutWin32.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static GLuint g_texture = 0;

static GLuint g_GrayscaleVS = 0;
static GLuint g_GrayscalePS = 0;
static GLuint g_GrayscaleProgram = 0;

static GLuint g_ToneVS = 0;
static GLuint g_TonePS = 0;
static GLuint g_ToneProgram = 0;

// One time Init
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

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	g_texture = GutLoadTexture_OpenGL("../../textures/lena.tga");
	if ( g_texture==0 )
		return false;

	g_GrayscaleVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect_grayscale.glvs");
	g_GrayscalePS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_grayscale.glfs");
	if ( 0==g_GrayscaleVS || 0==g_GrayscalePS )
		return false;

	g_GrayscaleProgram = GutCreateProgram(g_GrayscaleVS, g_GrayscalePS);
	if ( 0==g_GrayscaleProgram )
		return false;

	g_ToneVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect_grayscale.glvs");
	g_TonePS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_colortransform.glfs");
	if ( 0==g_ToneVS || 0==g_TonePS )
		return false;

	g_ToneProgram = GutCreateProgram(g_ToneVS, g_TonePS);
	if ( 0==g_ToneProgram )
		return false;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_texture )
	{
		glDeleteTextures(1, &g_texture);
		g_texture = 0;
	}

	if ( g_GrayscaleVS )
	{
		glDeleteShader(g_GrayscaleVS);
	}

	if ( g_GrayscalePS )
	{
		glDeleteShader(g_GrayscalePS);
	}

	if ( g_GrayscaleProgram )
	{
		glDeleteProgram(g_GrayscaleProgram);
	}

	if ( g_ToneVS )
	{
		glDeleteShader(g_ToneVS);
	}

	if ( g_TonePS )
	{
		glDeleteShader(g_TonePS);
	}

	if ( g_ToneProgram )
	{
		glDeleteProgram(g_ToneProgram);
	}

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//
	switch(g_iPosteffect)
	{
	default:
		glUseProgram(0);
		break;
	case 2:
		glUseProgram(g_GrayscaleProgram);
		break;
	case 3:
		{
			GLint reg = glGetUniformLocation(g_ToneProgram, "ColorMatrix");
			glUniformMatrix4fv(reg, 1, GL_FALSE, (float *) &g_SepiaMatrix );
			glUseProgram(g_ToneProgram);
		}
		break;
	case 4:
		{
			GLint reg = glGetUniformLocation(g_ToneProgram, "ColorMatrix");
			glUniformMatrix4fv(reg, 1, GL_FALSE, (float *) &g_SaturateMatrix );
			glUseProgram(g_ToneProgram);
		}
		break;
	}
	//
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Texcoord);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
