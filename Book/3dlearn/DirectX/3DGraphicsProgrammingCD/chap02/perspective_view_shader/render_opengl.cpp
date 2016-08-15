#include <windows.h>
// OpenGL extensions header
#include "glew.h"
#include "wglew.h"
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static GLuint g_VertexProgramID = 0;
static GLuint g_FragmentProgramID = 0;

bool InitResourceOpenGL(void)
{
	g_VertexProgramID = GutLoadVertexProgramOpenGL_ASM
		("../../shaders/vertex_transform.glvp");
	if ( g_VertexProgramID==0 )
		return false;

	g_FragmentProgramID = GutLoadFragmentProgramOpenGL_ASM
		("../../shaders/vertex_transform.glpp");
	if ( g_FragmentProgramID==0 )
		return false;

	// 鏡頭座標系轉換矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 投影轉換矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 1.0f, 100.0f);
	// 前兩個矩陣相乘結果
	Matrix4x4 view_projection_matrix = view_matrix * projection_matrix;
	// 設定視角轉換矩陣
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 0, 4, (float *) &view_projection_matrix);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_VertexProgramID )
	{
		GutReleaseVertexProgramOpenGL(g_VertexProgramID);
		g_VertexProgramID = 0;
	}

	if ( g_FragmentProgramID )
	{
		GutReleaseVertexProgramOpenGL(g_FragmentProgramID);
		g_FragmentProgramID = 0;
	}

	return true;
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 指定要使用Shader可程式化流程
	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	// 設定使用哪個Vertex/Fragment Program
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, g_VertexProgramID);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, g_FragmentProgramID);

	// 畫出金字塔的8條邊線
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), g_vertices);
	glDrawArrays(GL_LINES, 0, 16);

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
