#include <windows.h>
#include "glew.h"
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_view_matrix;
GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 設定投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	// 載入貼圖
	g_TextureID = GutLoadTexture_OpenGL("../../textures/lena_rgba.tga");
	// 使用g_TextureID貼圖物件
	glBindTexture( GL_TEXTURE_2D, g_TextureID );
	// 設定顯示貼圖時使用線性內插
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// 設定顯示貼圖時使用線性外插
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// 使用2D貼圖功能
	glEnable(GL_TEXTURE_2D);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_TextureID )
	{
		glDeleteTextures(1, &g_TextureID);
		g_TextureID = 0;
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
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// `使用OpenGL來繪圖`
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// `把正向跟反向的面都畫出來`
	glDisable(GL_CULL_FACE);
	// `設定要用陣列的方式傳入頂點位置跟顏色`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	GLint modes[] = {
		GL_REPEAT,			// `左下角`
		GL_MIRRORED_REPEAT,	// `左上角`
		GL_CLAMP_TO_EDGE,	// `右下角`
		GL_CLAMP_TO_BORDER,	// `右上角`
	};

	float border_color[4] = {0.5f, 0.5f, 0.5f, 0.5f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	for ( int i=0; i<4; i++ )
	{
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_Quads[i][0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), g_Quads[i][0].m_Texcoord);
		// `套用不同的貼圖座標解讀模式`
		// GL_TEXTURE_WRAP_S `設定水平方向解讀模式`
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modes[i]);
		// GL_TEXTURE_WRAP_T `設定垂直方向解讀模式`
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modes[i]);
		// `畫出格子`
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
