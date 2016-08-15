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

static GLuint g_BlurVS = 0;
static GLuint g_BlurPS = 0;
static GLuint g_BlurProgram = 0;

static sImageInfo g_ImageInfo;

static GLuint g_framebuffer[2] = {0, 0};
static GLuint g_frametexture[2] = {0, 0};

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

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);

	g_texture = GutLoadTexture_OpenGL("../../textures/lena.tga", &g_ImageInfo);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	if ( g_texture==0 )
		return false;

	g_BlurVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect_blur.glvs");
	g_BlurPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_blur.glfs");
	if ( 0==g_BlurVS || 0==g_BlurPS )
		return false;

	g_BlurProgram = GutCreateProgram(g_BlurVS, g_BlurPS);
	if ( 0==g_BlurProgram )
		return false;

	if ( !GutCreateRenderTargetOpenGL(g_ImageInfo.m_iWidth, g_ImageInfo.m_iHeight, GL_RGBA8, &g_framebuffer[0], &g_frametexture[0]) )
		return false;

	if ( !GutCreateRenderTargetOpenGL(g_ImageInfo.m_iWidth, g_ImageInfo.m_iHeight, GL_RGBA8, &g_framebuffer[1], &g_frametexture[1]) )
		return false;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

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

	glDeleteShader(g_BlurVS);
	glDeleteShader(g_BlurPS);
	glDeleteProgram(g_BlurProgram);

	for ( int i=0; i<2; i++ )
	{
		glDeleteFramebuffersEXT(1, &g_framebuffer[i]);
		glDeleteTextures(1, &g_frametexture[i]);
	}

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
}

static GLuint BlurImage(GLuint texture, sImageInfo *pInfo)
{	
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;
	float fTexelW = 1.0f/(float)pInfo->m_iWidth;
	float fTexelH = 1.0f/(float)pInfo->m_iHeight;

	const int num_samples = KERNELSIZE;
	Vector4 vTexOffsetX[num_samples];
	Vector4 vTexOffsetY[num_samples];

	for ( int i=0; i<num_samples; i++ )
	{
		vTexOffsetX[i].Set(g_uv_offset_table[i] * fTexelW, 0.0f, 0.0f, g_weight_table[i]);
		vTexOffsetY[i].Set(0.0f, g_uv_offset_table[i] * fTexelH, 0.0f, g_weight_table[i]);
	}

	glUseProgram(g_BlurProgram);

	GLint reg = glGetUniformLocation(g_BlurProgram, "vTexOffset");

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Texcoord);

	// 水平方向模糊
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer[0]);
		glViewport(0, 0, w, h);
		glBindTexture(GL_TEXTURE_2D, texture);
		if ( reg>=0 )
			glUniform4fv(reg, num_samples, (float *)vTexOffsetX);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// 垂直方向模糊
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer[1]);
		glBindTexture(GL_TEXTURE_2D, g_frametexture[0]);
		if ( reg>=0 )
			glUniform4fv(reg, num_samples, (float *)vTexOffsetY);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	return g_frametexture[1];
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	GLuint texture;
	//
	if ( g_bPosteffect )
	{
		texture = BlurImage(g_texture, &g_ImageInfo);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	}
	else
	{
		texture = g_texture;
	}

	int w, h;
	GutGetWindowSize(w, h);
	glViewport(0, 0, w, h);
	// 清除畫面
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(0);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBindTexture(GL_TEXTURE_2D, texture);
	//
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad[0].m_Texcoord);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
