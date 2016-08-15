#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// 設定投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	// 載入貼圖
	g_TextureID = GutLoadTexture_OpenGL("../../textures/checkerboard.tga");
	// 使用g_TextureID貼圖物件
	glBindTexture( GL_TEXTURE_2D, g_TextureID );
	// 把超過0-1范圍的座標解讀方法設為REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
	// `貼圖被放大時, 取最接近4個點來內插.`
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	// `下面會設定貼圖被縮小時候做法`
	switch(g_iFilterMode)
	{
	case 1: // None
		// `不使用mipmap, 取最近4個點來平均.`
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		// `關閉anisotrophic filter`
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		break;
	case 2: // Bilinear
		// `GL_LINEAR = 貼圖縮小時取4個點來平均`
		// `MIPMAP_NEAREST = 使用最接近大小的圖層`
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST );
		// `關閉anisotrophic filter`
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		break;
	case 3: // Trilinear
		// `GL_LINEAR = 貼圖縮小時取4個點來平均`
		// `MIPMAP_LINEAR = 使用最接近大小的兩張圖層, 然後再內插.`
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		// `關閉anisotrophic filter`
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1.0f);
		break;
	case 4: // Anisotropic filter
		// MIPMAP_LINEAR = `使用最接近大小的兩張圖層 , 然後再內插 .`
		glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		// `啟動 anisotrophic filter`
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 8.0f);
		break;
	}

	// `計算矩陣`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = object_matrix * view_matrix;

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// `畫出格子`
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
