#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "GutTexture_OpenGL.h"
#include "render_data.h"

static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_mirror_view_matrix;

static CGutModel_OpenGL g_Model_OpenGL;

static GLuint g_texture = 0;
static GLuint g_depthtexture = 0;
static GLuint g_framebuffer = 0;
static GLuint g_depthbuffer = 0;

bool InitResourceOpenGL(void)
{
	if ( glGenFramebuffersEXT==NULL )
	{
		printf("Could not create frame buffer object\n");
		return false;
	}

	GLuint a = GutLoadTexture_OpenGL("../../textures/lobbycubemap.dds");

	// 投影矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);	
	glEnable(GL_NORMALIZE);
	glEnable(GL_CULL_FACE);

	// 開啟一個framebuffer object
	glGenFramebuffersEXT(1, &g_framebuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer);

	// 配罝一塊貼圖空間給framebuffer object繪圖使用 
	{
		glGenTextures(1, &g_texture);
		glBindTexture(GL_TEXTURE_2D, g_texture);
		// 設定filter
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// 宣告貼圖大小及格式
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// framebuffer的RGBA繪圖
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texture, 0);
	}

	// 配置zbuffer給framebuffer object使用
	{
		glGenRenderbuffersEXT(1, &g_depthbuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_depthbuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 512, 512);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_depthbuffer);
	}

	// 檢查framebuffer object有沒有配置成?
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_framebuffer )
	{
		glDeleteFramebuffersEXT(1, &g_framebuffer);
		g_framebuffer = 0;
	}

	if ( g_depthbuffer )
	{
		glDeleteRenderbuffersEXT(1, &g_depthbuffer);
		g_depthbuffer = 0;
	}

	g_Model_OpenGL.Release();

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

static void RenderModelOpenGL(bool mirror, Vector4 *pPlane)
{
	Matrix4x4 view_matrix;
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	if ( mirror )
	{
		Vector4 vEye = g_Control.GetCameraPosition();
		Vector4 vUp = g_Control.m_vUp;
		Vector4 vLookAt = g_Control.m_vLookAt;

		Vector4 mirror_eye = MirrorPoint(vEye, *pPlane);
		Vector4 mirror_lookat = MirrorPoint(vLookAt, *pPlane);
		Vector4 mirror_up = MirrorPoint(vUp, *pPlane);

		view_matrix = GutMatrixLookAtRH(mirror_eye, mirror_lookat, mirror_up);
		g_mirror_view_matrix = view_matrix;
	}
	else
	{
		view_matrix = g_Control.GetViewMatrix();
	}

	glMatrixMode(GL_MODELVIEW);
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);

	g_Model_OpenGL.Render();
}

// `使用OpenGL來繪圖`
void RenderFrameOpenGL(void)
{
	Vector4 vPlane(0.0f, 0.0f, 1.0f, -g_mirror_z);

	{
		// `使用` g_framebuffer framebuffer object	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer);
		glViewport(0, 0, 512, 512);
		// `清除畫面`
		glClearColor(0.0f, 0.0f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//
		RenderModelOpenGL(true, &vPlane);
	}

	{
		// `使用主framebuffer object, 也就是視窗.`
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		int w, h;
		GutGetWindowSize(w, h);
		glViewport(0, 0, w, h);

		// `清除畫面`
		glClearColor(0.0f, 0.0f, 0.6f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderModelOpenGL(false, NULL);

		// `設定轉換矩陣`
		Matrix4x4 world_view_matrix = g_Control.GetViewMatrix();

		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &g_projection_matrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);

		sModelMaterial_OpenGL material;
		material.m_Textures[0] = g_texture;
		material.Submit(NULL);

		// `設定頂點資料格式`
		glEnableClientState(GL_VERTEX_ARRAY);
		glClientActiveTexture(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		Matrix4x4 uv_offset_matrix;
		uv_offset_matrix.Scale_Replace(0.5f, 0.5f, 1.0f);
		uv_offset_matrix[3].Set(0.5f, 0.5f, 0.0f, 1.0f);

		Matrix4x4 texture_matrix = g_mirror_view_matrix * g_projection_matrix * uv_offset_matrix;
		// `計算貼圖座標, 也可以經由設定texture matrix來做.`
		for ( int i=0; i<4; i++ )
		{
			g_Quad[i].m_Texcoord = g_Quad[i].m_Position * texture_matrix;
		}

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
		glTexCoordPointer(4, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
