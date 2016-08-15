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

static GLuint g_PassThroughVS = 0;

static GLuint g_BlurPS = 0;
static GLuint g_BlurProgram = 0;

static GLuint g_DoFPS = 0;
static GLuint g_DoFProgram = 0;

static sImageInfo g_ImageInfo, g_DownSampledImageInfo;

static GLuint g_DownSampledFrameBuffer[2] = {0, 0};
static GLuint g_DownSampledFrameTexture[2] = {0, 0};

static GLuint g_FrameBuffer = 0;
static GLuint g_FrameTexture = 0;
static GLuint g_DepthTexture = 0;

static Matrix4x4 g_proj_matrix;
static Matrix4x4 g_inv_proj_matrix;

static CGutModel_OpenGL g_ModelSun_OpenGL;
static CGutModel_OpenGL g_ModelEarth_OpenGL;

bool ReInitResourceOpenGL(void)
{
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
	g_inv_proj_matrix = g_proj_matrix;
	g_inv_proj_matrix.Invert();

	Vector4 v(0.0f, 0.0f, -0.5f, 1.0f);
	Vector4 t_v = v * g_proj_matrix;
	t_v /= t_v.GetW();

	Vector4 p(0.0f, 0.0f, t_v.GetZ(), 1.0f);
	Vector4 z_inv = g_inv_proj_matrix.GetColumn(3);
	Vector4 vdot = VectorDot(p, z_inv);
	float z = 1.0f/vdot.GetX();

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

	if ( g_FrameBuffer )
	{
		glDeleteFramebuffersEXT(1, &g_FrameBuffer);
		g_FrameBuffer = 0;
	}

	if ( g_DepthTexture )
	{
		glDeleteTextures(1, &g_DepthTexture);
		g_DepthTexture = 0;
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
		w, h, &g_FrameBuffer, GL_RGBA8, &g_FrameTexture,
		GL_DEPTH_COMPONENT24, &g_DepthTexture) )
	{
		GutPushMessage("Failed to create RenderTarget");
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

	g_PassThroughVS = GutLoadVertexShaderOpenGL_GLSL("../../shaders/posteffect.glvs");
	g_BlurPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_blur.glfs");
	if ( 0==g_PassThroughVS || 0==g_BlurPS )
		return false;

	g_BlurProgram = GutCreateProgram(g_PassThroughVS, g_BlurPS);
	if ( 0==g_BlurProgram )
		return false;

	g_DoFPS = GutLoadFragmentShaderOpenGL_GLSL("../../shaders/posteffect_dof.glfs");
	if ( 0==g_DoFPS )
		return false;

	g_DoFProgram = GutCreateProgram(g_PassThroughVS, g_DoFPS);
	if ( 0==g_DoFProgram )
		return false;

	if ( !ReInitResourceOpenGL() )
		return false;

	glDisable(GL_CULL_FACE);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_FrameBuffer )
	{
		glDeleteFramebuffersEXT(1, &g_FrameBuffer);
		g_FrameBuffer = 0;
	}

	if ( g_DepthTexture )
	{
		glDeleteTextures(1, &g_DepthTexture);
		g_DepthTexture = 0;
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

	glDeleteShader(g_PassThroughVS);
	glDeleteShader(g_BlurPS);
	glDeleteProgram(g_BlurProgram);

	glDeleteShader(g_DoFPS);
	glDeleteProgram(g_DoFProgram);

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	ReInitResourceOpenGL();
}

static GLuint StretchImage(GLuint texture, sImageInfo *pInfo)
{
	int w = pInfo->m_iWidth;
	int h = pInfo->m_iHeight;

	glDisable(GL_LIGHTING);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_DownSampledFrameBuffer[1]);
	glViewport(0, 0, w, h);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glUseProgram(0);
	glBindTexture(GL_TEXTURE_2D, texture);

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

	/*
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	*/

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

	/*
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	*/

	return g_DownSampledFrameTexture[1];
}

static void SetupLighting(void)
{
	// 設定環境光
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float *)&g_vAmbientLight);

	int LightID = GL_LIGHT0;

	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);
	glEnable(LightID);

	GutSetupLightOpenGL(0, g_Light0);
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
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_FrameBuffer);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, w, h);

	glEnable(GL_DEPTH_TEST);
	//glDepthMask(GL_TRUE);

	glUseProgram(0);

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *)&g_proj_matrix );
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *)&view_matrix);

	//SetupLighting();

	wv_matrix = g_sun_matrix * view_matrix;
	glLoadMatrixf( (float *)&wv_matrix );

	g_ModelSun_OpenGL.Render();

	wv_matrix = g_earth_matrix * view_matrix;
	glLoadMatrixf( (float *)&wv_matrix );

	g_ModelEarth_OpenGL.Render();

	if ( g_bPosteffect )
	{
		glDisable(GL_DEPTH_TEST);

		Vector4 vZInv = g_inv_proj_matrix.GetColumn(3);

		GLuint texture;
		texture = StretchImage(g_FrameTexture, &g_DownSampledImageInfo);
		texture = BlurImage(texture, &g_DownSampledImageInfo);
		//
		glUseProgram(g_DoFProgram);
		//
		GLint blur_reg = glGetUniformLocation(g_DoFProgram, "BlurredImage");
		GLint dof_reg = glGetUniformLocation(g_DoFProgram, "vDepthOfField");
		GLint zinv_reg = glGetUniformLocation(g_DoFProgram, "vZInv");

		glUniform4fv(dof_reg, 1, (float *)&g_vDepthOfField);
		glUniform4fv(zinv_reg, 1, (float *)&vZInv);

		glDisable(GL_BLEND);

		// 取出貼圖變數被分配到的代碼
		GLint image_reg = glGetUniformLocation(g_DoFProgram, "Image");
		GLint z_reg = glGetUniformLocation(g_DoFProgram, "ZBuffer");

		// 把image_reg代碼所表示的貼圖，連接到第0層貼圖上。
		glUniform1i(image_reg, 0);
		// 把g_FrameTexture套用成第0層貼圖
		glActiveTexture(GL_TEXTURE0_ARB);
		glBindTexture(GL_TEXTURE_2D, g_FrameTexture);

		// 把z_reg代碼所表示的貼圖，連接到第1層貼圖上。
		glUniform1i(z_reg, 1);
		// 把g_DepthTexture套用成第1層貼圖
		glActiveTexture(GL_TEXTURE1_ARB);
		glBindTexture(GL_TEXTURE_2D, g_DepthTexture);

		glActiveTexture(GL_TEXTURE2_ARB);
		glBindTexture(GL_TEXTURE_2D, texture);
		glUniform1i(blur_reg, 2);

		glActiveTexture(GL_TEXTURE0_ARB);

		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		glViewport(0, 0, w, h);
		//
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuadInv[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuadInv[0].m_Texcoord);
		//
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}


	GutSwapBuffersOpenGL();
}
