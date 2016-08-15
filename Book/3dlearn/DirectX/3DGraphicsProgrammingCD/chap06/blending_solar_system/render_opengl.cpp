#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

bool InitResourceOpenGL(void)
{
	// 投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_CULL_FACE);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	// 沒事做
	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// 設定要用陣列的方式傳入頂點位置跟顏色
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_view_matrix;

	glMatrixMode(GL_MODELVIEW);

	if ( g_bZWrite )
	{
		// 更新zbuffer
		glDepthMask(GL_TRUE);
	}
	else
	{
		// 不更新zbuffer
		glDepthMask(GL_FALSE);
	}

	glEnable(GL_BLEND);
	// 混色模式
	switch(g_iBlendMode)
	{
	case 0:
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		break;
	case 1:
		glBlendFunc(GL_ONE, GL_ONE);
		break;
	}

	for ( int i=0; i<2; i++ )
	{
		switch( g_iOrder[i] )
		{
		case 0:
			// 太陽
			world_view_matrix = g_sun_matrix * g_rot_matrix * view_matrix;
			glLoadMatrixf( (float *) &world_view_matrix);
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pSunVertices);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pSunVertices[0].m_RGBA);
			glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
			break;
		case 1:
			// 地球
			world_view_matrix = g_earth_matrix * g_rot_matrix * view_matrix;
			glLoadMatrixf( (float *) &world_view_matrix);
			glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pEarthVertices);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pEarthVertices[0].m_RGBA);
			glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
			break;
		default:
			break;
		}
	}

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
