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

void ResizeWindowOpenGL(int width, int height)
{
	glViewport(0, 0, width, height);

	// 投影矩陣
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// 設定要用陣列的方式傳入頂點位置跟顏色
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pSphereVertices);
	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pSphereVertices[0].m_RGBA);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &view_matrix);

	glDrawElements(
		GL_TRIANGLES, // 指定所要畫的基本圖形種類
		g_iNumSphereIndices, // 有幾個索引值
		GL_UNSIGNED_SHORT, // 索引值的型態
		g_pSphereIndices // 索引值陣列
		);

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
