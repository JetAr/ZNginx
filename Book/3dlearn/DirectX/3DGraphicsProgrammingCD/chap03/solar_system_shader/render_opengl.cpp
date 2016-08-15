#include <windows.h>
// OpenGL extensions header
#include "glew.h"
#include "wglew.h"
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_proj_matrix;

static GLuint g_VertexProgramID = 0;
static GLuint g_FragmentProgramID = 0;

bool InitResourceOpenGL(void)
{
	g_VertexProgramID = GutLoadVertexProgramOpenGL_ASM("../../shaders/vertex_color.glvp");
	if ( g_VertexProgramID==0 )
		return false;

	g_FragmentProgramID = GutLoadFragmentProgramOpenGL_ASM("../../shaders/vertex_color.glpp");
	if ( g_FragmentProgramID==0 )
		return false;

	// 投影矩陣
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);

	glEnable(GL_CULL_FACE);

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

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
}

// `使用OpenGL來繪圖`
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// `指定要使用Shader可程式化流程`
	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	// `設定使用哪個Vertex/Fragment Program`
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, g_VertexProgramID);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, g_FragmentProgramID);

	// `設定要用陣列的方式傳入頂點位置跟顏色`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// `計算轉換矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 view_proj_matrix = view_matrix * g_proj_matrix;
	Matrix4x4 world_view_proj_matrix;

	// `太陽`
	world_view_proj_matrix = g_sun_matrix * view_proj_matrix;
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 0, 4, (float *) &world_view_proj_matrix);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pSunVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pSunVertices[0].m_RGBA);
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
	// `地球`
	world_view_proj_matrix = g_earth_matrix * view_proj_matrix;
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 0, 4, (float *) &world_view_proj_matrix);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pEarthVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pEarthVertices[0].m_RGBA);
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);

	// `月球`
	world_view_proj_matrix = g_moon_matrix * view_proj_matrix;
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 0, 4, (float *) &world_view_proj_matrix);

	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pMoonVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pMoonVertices[0].m_RGBA);
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);

	// `呈現出背景backbuffer`
	GutSwapBuffersOpenGL();
}
