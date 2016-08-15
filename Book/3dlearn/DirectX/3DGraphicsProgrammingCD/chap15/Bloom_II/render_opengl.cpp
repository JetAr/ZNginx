#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutOpenGL.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static GLuint g_texture = 0;

static GLuint g_BlurVS = 0;
static GLuint g_BlurPS = 0;
static GLuint g_BlurProgram = 0;

static GLuint g_BrightnessPS = 0;
static GLuint g_BrightnessProgram = 0;

static sImageInfo g_ImageInfo, g_DownSampledImageInfo;

static GLuint g_DownSampledFrameBuffer[2] = {0, 0};
static GLuint g_DownSampledFrameTexture[2] = {0, 0};

static GLuint g_FrameBuffer[2] = {0, 0};
static GLuint g_FrameTexture[2] = {0, 0};
static GLuint g_depthtexture = 0;

static Matrix4x4 g_proj_matrix;

static CGutModel_OpenGL g_ModelSun_OpenGL;
static CGutModel_OpenGL g_ModelEarth_OpenGL;

bool ReInitResourceOpenGL(void)
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

	int w,h;
	GutGetWindowSize(w, h);

	g_ImageInfo.m_bProcedure = true;
	g_ImageInfo.m_iWidth = w;
	g_ImageInfo.m_iHeight = h;

	g_DownSampledImageInfo.m_bProcedure = true;
	g_DownSampledImageInfo.m_iWidth = w/4;
	g_DownSampledImageInfo.m_iHeight = h/4;

	glViewport(0, 0, w, h);

	float aspect = (float)h/(float)w;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, g_fNearZ, g_fFarZ);

	for ( int i=0; i<2; i++ )
	{
		if ( g_DownSampledFrameBuffer[i] )
		{
			glDeleteFramebuffersEXT(1, &g_DownSampledFrameBuffer[i]);
			g_DownSampledFrameBuffer[i] = 0;
		}
		if ( g_DownSampledFrameTexture[i] )
		{
			glDeleteTextures(1, &g_DownSampledFrameTexture[i]);
			g_DownSampledFrameTexture[i] = 0;
		}

		if ( g_FrameBuffer[i] )
		{
			glDeleteFramebuffersEXT(1, &g_FrameBuffer[i]);
			g_FrameBuffer[i] = 0;
		}
		if ( g_FrameTexture[i] )
		{
			glDeleteTextures(1, &g_FrameTexture[i]);
			g_FrameTexture[i] = 0;
		}
	}

	if ( g_depthtexture )
	{
		glDeleteTextures(1, &g_depthtexture);
		g_depthtexture = 0;
	}

	if ( !GutCreateRenderTargetOpenGL(
		g_DownSampledImageInfo.m_iWidth, g_DownSampledImageInfo.m_iHeight, 
		GL_RGBA8, &g_DownSampledFrameBuffer[0], &g_DownSampledFrameTexture[0]) )
	{
		GutPushMessage("Failed to create RenderTarget");
		return false;
	}

	if ( !GutCreateRenderTargetOpenGL(
		g_DownSampledImageInfo.m_iWidth, g_DownSampledImageInfo.m_iHeight, 
		GL_RGBA8, &g_DownSampledFrameBuffer[1], &g_DownSampledFrameTexture[1]) )
	{
		GutPushMessage("Failed to create RenderTarget");
		return false;
	}

	if ( !GutCreateRenderTargetOpenGL(
		w, h, &g_FrameBuffer[0], GL_RGBA8, &g_FrameTexture[0],
		GL_DEPTH_COMPONENT24, &g_depthtexture) )
	{
		GutPushMessage("Failed to create RenderTarget");
		return false;
	}

	if ( !GutCreateRenderTargetOpenGL(
		w, h, GL_RGBA8, &g_FrameBuffer[1], &g_FrameTexture[1]) )
	{
		GutPushMessage("Failed to create RenderTarget");
		return false;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_FrameBuffer[1]);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, g_depthtexture, 0);

	// 檢查framebuffer object有沒有配置成
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		GutPushMessage("RenderTarget3 not ready");
		return false;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	return true;
}

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

	glEnable(GL_TEXTURE_2D);
	glDepthFunc(GL_LEQUAL);

	if ( !g_ModelSun_OpenGL.ConvertToOpenGLModel(&g_ModelSun) )
	{
		GutPushMessage("Failed to convert sun model");
		return false;
	}

	if ( !g_ModelEarth_OpenGL.ConvertToOpenGLModel(&g_ModelEarth) )
	{
		GutPushMessage("Failed to convert earth model");
		return false;
	}

	g_BlurVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect_blur.glvs");
	g_BlurPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_blur.glfs");
	if ( 0==g_BlurVS || 0==g_BlurPS )
		return false;

	g_BlurProgram = GutCreateProgram(g_BlurVS, g_BlurPS);
	if ( 0==g_BlurProgram )
		return false;

	g_BrightnessPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_brightness.glfs");
	if ( 0==g_BrightnessPS )
		return false;
	g_BrightnessProgram = GutCreateProgram(g_BlurVS, g_BrightnessPS);
	if ( 0==g_BrightnessProgram )
		return false;

	if ( !ReInitResourceOpenGL() )
		return false;

	glDisable(GL_CULL_FACE);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	for ( int i=0; i<2; i++ )
	{
		if ( g_FrameBuffer[i] )
		{
			glDeleteFramebuffersEXT(1, &g_FrameBuffer[i]);
			g_FrameBuffer[i] = 0;
		}
		if ( g_FrameTexture[i] )
		{
			glDeleteTextures(1, &g_FrameTexture[i]);
			g_FrameTexture[i] = 0;
		}
	}

	for ( int i=0; i<2; i++ )
	{
		if ( g_DownSampledFrameBuffer[i] )
		{
			glDeleteFramebuffersEXT(1, &g_DownSampledFrameBuffer[i]);
			g_DownSampledFrameBuffer[i] = 0;
		}
		if ( g_DownSampledFrameTexture[i] )
		{
			glDeleteTextures(1, &g_DownSampledFrameTexture[i]);
			g_DownSampledFrameTexture[i] = 0;
		}
	}

	if ( g_depthtexture )
	{
		glDeleteTextures(1, &g_depthtexture);
		g_depthtexture = 0;
	}

	glDeleteShader(g_BlurVS);
	glDeleteShader(g_BlurVS);
	glDeleteProgram(g_BlurProgram);

	glDeleteShader(g_BrightnessPS);
	glDeleteProgram(g_BrightnessProgram);

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	ReInitResourceOpenGL();
}

static GLuint Brightness(GLuint texture, sImageInfo *pInfo)
{
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_DownSampledFrameBuffer[1]);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(g_BrightnessProgram);
	glBindTexture(GL_TEXTURE_2D, texture);

	GLint reg_offset = glGetUniformLocation(g_BrightnessProgram, "IntensityOffset");
	GLint reg_scale = glGetUniformLocation(g_BrightnessProgram, "IntensityScale");
	glUniform4fv(reg_offset, 1, (float *)&g_vBrightnessOffset);
	glUniform4fv(reg_scale, 1, (float *)&g_vBrightnessScale);

	Vertex_VT *pVertexArray = pInfo->m_bProcedure ? g_FullScreenQuadInv : g_FullScreenQuad;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), pVertexArray[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), pVertexArray[0].m_Texcoord);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	return g_DownSampledFrameTexture[1];
}

static GLuint BlurImage(GLuint texture, sImageInfo *pInfo)
{	
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	float fTexelW = 1.0f/(float)w;
	float fTexelH = 1.0f/(float)h;

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

	Vertex_VT *pVertexArray = pInfo->m_bProcedure ? g_FullScreenQuadInv : g_FullScreenQuad;

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), pVertexArray[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), pVertexArray[0].m_Texcoord);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();


	// 水平方向模糊
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_DownSampledFrameBuffer[0]);
		glViewport(0, 0, w, h);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform4fv(reg, num_samples, (float *)vTexOffsetX);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// 垂直方向模糊
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_DownSampledFrameBuffer[1]);
		glBindTexture(GL_TEXTURE_2D, g_DownSampledFrameTexture[0]);
		glUniform4fv(reg, num_samples, (float *)vTexOffsetY);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	return g_DownSampledFrameTexture[1];
}

void RenderFrameOpenGL(void)
{
	int w, h;
	GutGetWindowSize(w, h);

	// 清除畫面
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 wv_matrix;

	if ( g_bPosteffect )
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_FrameBuffer[0]);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, w, h);

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *)&g_proj_matrix );
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *)&view_matrix);

	wv_matrix = g_earth_matrix * view_matrix;
	glLoadMatrixf( (float *)&wv_matrix );

	g_ModelEarth_OpenGL.Render();

	wv_matrix = g_sun_matrix * view_matrix;
	glLoadMatrixf( (float *)&wv_matrix );

	g_ModelSun_OpenGL.Render();

	if ( g_bPosteffect )
	{
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_FrameBuffer[1]);
		glViewport(0, 0, w, h);
		glClear(GL_COLOR_BUFFER_BIT);

		wv_matrix = g_sun_matrix * view_matrix;
		glLoadMatrixf( (float *)&wv_matrix );
		g_ModelSun_OpenGL.Render();

		glDisable(GL_DEPTH_TEST);

		GLuint texture = g_FrameTexture[1];
		texture = Brightness(texture, &g_DownSampledImageInfo);
		texture = BlurImage(texture, &g_DownSampledImageInfo);

		glMatrixMode(GL_PROJECTION); glLoadIdentity();
		glMatrixMode(GL_MODELVIEW); glLoadIdentity();

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glClear(GL_COLOR_BUFFER_BIT);
		glViewport(0, 0, w, h);
		//
		glUseProgram(0);
		glDisable(GL_LIGHTING);

		glEnable(GL_BLEND);
		glBlendFunc(GL_ONE, GL_ONE);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuadInv[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuadInv[0].m_Texcoord);
		//
		glBindTexture(GL_TEXTURE_2D, texture);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//
		glBindTexture(GL_TEXTURE_2D, g_FrameTexture[0]);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		//
		glDisable(GL_BLEND);
	}


	GutSwapBuffersOpenGL();
}
