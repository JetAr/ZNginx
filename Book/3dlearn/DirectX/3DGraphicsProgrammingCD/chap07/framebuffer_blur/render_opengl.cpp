#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "render_data.h"
#include "GutModel_OpenGL.h"

static Matrix4x4 g_view_matrix;
static Matrix4x4 g_projection_matrix;

static GLuint g_texture = 0;
static GLuint g_depthtexture = 0;
static GLuint g_framebuffer = 0;
static GLuint g_depthbuffer = 0;

const int g_framebuffer_w = 256;
const int g_framebuffer_h = 256;

static GLuint g_blurframebuffer[2];
static GLuint g_blurtexture[2];

const int g_blurframebuffer_w = 256;
const int g_blurframebuffer_h = 256;

static CGutModel_OpenGL g_Models_OpenGL[3];

bool InitResourceOpenGL(void)
{
	// 投影矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if ( glGenFramebuffersEXT==NULL )
	{
		printf("Could not create frame buffer object\n");
		return false;
	}

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
		// 宣告貼圖大小及格式
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  g_framebuffer_w, g_framebuffer_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		// framebuffer的RGBA繪圖
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_texture, 0);
	}

	// 配置zbuffer給framebuffer object使用
	{
		glGenRenderbuffersEXT(1, &g_depthbuffer);
		glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, g_depthbuffer);
		glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, g_framebuffer_w, g_framebuffer_h);
		glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, g_depthbuffer);
	}

	// 檢查framebuffer object有沒有配置成?
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
	{
		return false;
	}

	for ( int i=0; i<2; i++ )
	{
		// 再開啟一個framebuffer object
		glGenFramebuffersEXT(1, &g_blurframebuffer[i]);
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_blurframebuffer[i]);

		// 配罝一塊貼圖空間給framebuffer object繪圖使用 
		{
			glGenTextures(1, &g_blurtexture[i]);
			glBindTexture(GL_TEXTURE_2D, g_blurtexture[i]);
			// 設定filter
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			// 宣告貼圖大小及格式
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,  g_blurframebuffer_w, g_blurframebuffer_w, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
			// framebuffer的RGBA繪圖
			glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, g_blurtexture[i], 0);
		}

		// 檢查framebuffer object有沒有配置成功
		status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
		if ( status!=GL_FRAMEBUFFER_COMPLETE_EXT )
		{
			return false;
		}
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	int i;

	CGutModel::SetTexturePath("../../textures/");

	for ( i=0; i<3; i++ )
	{
		g_Models_OpenGL[i].ConvertToOpenGLModel(&g_Models[i]);
	}

	for ( i=0; i<4; i++ )
	{
		g_Quad[i].m_Texcoord[1] = 1.0f - g_Quad[i].m_Texcoord[1];
	}

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

	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

static void RenderSolarSystemOpenGL(void)
{
	// `設定要用陣列的方式傳入頂點位置跟顏色`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	// `投影矩陣`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// `鏡頭矩陣`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();

	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_TEXTURE_2D);

	Matrix4x4 world_view_matrix;
	// `太陽`
	world_view_matrix = g_sun_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	g_Models_OpenGL[0].Render();
	// `地球`
	world_view_matrix = g_earth_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	g_Models_OpenGL[1].Render();
	// `月球`
	world_view_matrix = g_moon_matrix * view_matrix;
	g_Models_OpenGL[2].Render();
}

// OpenGL 7x7 blur
GLuint BlurTexture(GLuint source)
{
	// `使用動態貼圖`
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_blurframebuffer[0]);
	// `把viewport設成跟動態貼圖大小相同`
	glViewport(0, 0, g_blurframebuffer_w, g_blurframebuffer_h);
	// `把所有轉換矩陣設定成單位矩陣`
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, source);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE);
	// `權重系數比, 俞接近中央權重會愈大.`
	float weight_table[7] = 
	{1.0f, 3.0f, 4.0f, 5.0f, 4.0f, 3.0f, 1.0f};
	// `圖片的偏移量`
	float uv_offset_table[7] = 
	{-3.0f, -2.0f, -1.0f, 0.0f, 1.0f, 2.0f, 3.0f};
	float sum = 0.0f;
	// `把權重系數對應到正確的0-1范圍內`
	for ( int i=0; i<7; i++ )
	{
		sum += weight_table[i];
	}
	for ( int i=0; i<7; i++ )
	{
		weight_table[i] /= sum;
	}

	Vector4 vOneTexel_X(1.0f/(float)g_framebuffer_w, 0.0f, 0.0f, 0.0f);
	Vector4 vOneTexel_Y(0.0f, 1.0f/(float)g_framebuffer_h, 0.0f, 0.0f);
	// `設定頂點資料格式`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	Vertex_V3T2 temp_quad[4];
	// `水平方向模糊化`
	for ( int i=0; i<7; i++ )
	{
		Vector4 vOffset = uv_offset_table[i] * vOneTexel_X;

		for ( int p=0; p<4; p++ )
		{
			memcpy(temp_quad[p].m_Position, g_Quad[p].m_Position, sizeof(float)*3);
			temp_quad[p].m_Texcoord[0] = g_Quad[p].m_Texcoord[0] + vOffset[0];
			temp_quad[p].m_Texcoord[1] = g_Quad[p].m_Texcoord[1] + vOffset[1];
		}

		glColor4f(weight_table[i], weight_table[i], weight_table[i], weight_table[i]);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3T2), &temp_quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_V3T2), &temp_quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_blurframebuffer[1]);
	glClear(GL_COLOR_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, g_blurtexture[0]);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// `垂直方向模糊化`
	for ( int i=0; i<=7; i++ )
	{
		Vector4 vOffset = uv_offset_table[i] * vOneTexel_Y;

		for ( int p=0; p<4; p++ )
		{
			memcpy(temp_quad[p].m_Position, g_Quad[p].m_Position, sizeof(float)*3);
			temp_quad[p].m_Texcoord[0] = g_Quad[p].m_Texcoord[0] + vOffset[0];
			temp_quad[p].m_Texcoord[1] = g_Quad[p].m_Texcoord[1] + vOffset[1];
		}

		glColor4f(weight_table[i], weight_table[i], weight_table[i], weight_table[i]);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3T2), &temp_quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_V3T2), &temp_quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	return g_blurframebuffer[1];
}

// `使用OpenGL來繪圖`
void RenderFrameOpenGL(void)
{
	{
		// `使用` g_framebuffer framebuffer object	
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, g_framebuffer);
		glViewport(0, 0, g_framebuffer_w, g_framebuffer_h);
		// `清除畫面`
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// 
		RenderSolarSystemOpenGL();
	}
	// `對貼圖做模糊化`
	GLuint blurred_texture = BlurTexture(g_texture);
	{
		// `使用主framebuffer object, 也就是視窗.`
		glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
		int w, h;
		GutGetWindowSize(w, h);
		glViewport(0, 0, w, h);
		// `清除畫面`
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		// `把所有轉換矩陣設定成單位矩陣`
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		// `設定頂點資料格式`
		glEnableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_COLOR_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glEnable(GL_TEXTURE_2D);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		if ( g_bBlur )
			glBindTexture(GL_TEXTURE_2D, blurred_texture);
		else
			glBindTexture(GL_TEXTURE_2D, g_texture);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3T2), &g_Quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_V3T2), &g_Quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
