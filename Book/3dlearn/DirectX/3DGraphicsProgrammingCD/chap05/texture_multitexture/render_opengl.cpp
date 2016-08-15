#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

GLuint g_Texture0_ID = 0;
GLuint g_Texture1_ID = 0;

bool InitResourceOpenGL(void)
{
	// 投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// 載入貼圖
	g_Texture0_ID = GutLoadTexture_OpenGL("../../textures/brickwall.tga");
	// 套用Trilinear Filter
	glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// 載入貼圖
	g_Texture1_ID = GutLoadTexture_OpenGL("../../textures/graffiti_happy.tga");
	// 套用Trilinear Filter
	glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_Texture0_ID )
	{
		// 釋放貼圖資源
		glDeleteTextures(1, &g_Texture0_ID);
		g_Texture0_ID = 0;
	}

	if ( g_Texture1_ID )
	{
		// 釋放貼圖資源
		glDeleteTextures(1, &g_Texture1_ID);
		g_Texture1_ID = 0;
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

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// `把正向跟反向的面都畫出來`
	glDisable(GL_CULL_FACE);
	// `設定要用陣列的方式傳入頂點位置跟顏色`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);

	switch(g_iMode)
	{
	case 1:
		// `打開第0層貼圖`
		glActiveTexture(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
		// `指定第0層貼圖的貼圖座標`
		glClientActiveTexture(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
		// `關閉第1層貼圖`
		glActiveTexture(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		// `不去讀取第1層貼圖的貼圖座標`
		glClientActiveTexture(GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		break;

	case 2:
		// `打開第0層貼圖`
		glActiveTexture(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);
		// `指定第0層貼圖的貼圖座標`
		glClientActiveTexture(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
		// `關閉第1層貼圖`
		glActiveTexture(GL_TEXTURE1_ARB);
		glDisable(GL_TEXTURE_2D);
		// `不去讀取第1層貼圖的貼圖座標`
		glClientActiveTexture(GL_TEXTURE1_ARB);
		glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		break;

	case 3:
		// `打開第0層貼圖`
		glActiveTexture(GL_TEXTURE0_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
		// `指定第0層貼圖的貼圖座標`
		glClientActiveTexture(GL_TEXTURE0_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
		// `打開第1層貼圖`
		glActiveTexture(GL_TEXTURE1_ARB);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);
		// `設定兩張貼圖相加`
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
		// `指定第1層貼圖的貼圖座標`
		glClientActiveTexture(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);

		break;
	}

	// `設定轉換矩陣`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// `畫出看板`
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
