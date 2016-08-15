#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"
#include "GutModel_OpenGL.h"

#include "render_data.h"

GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// 設定投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// 載入貼圖
	const char *texture_array[] = 
	{
		"../../textures/uffizi_right.tga",
		"../../textures/uffizi_left.tga",
		"../../textures/uffizi_top.tga",
		"../../textures/uffizi_bottom.tga",
		"../../textures/uffizi_back.tga", // `右手座標系上 Z+ 為鏡頭後方.`
		"../../textures/uffizi_front.tga" // `右手座標系上 Z- 為鏡頭前方.`
	};

	g_TextureID = GutLoadCubemapTexture_OpenGL(texture_array);

	// 使用CUBEMAP貼圖功能
	glEnable(GL_TEXTURE_CUBE_MAP);
	// 套用CUBEMAP貼圖
	glBindTexture( GL_TEXTURE_CUBE_MAP, g_TextureID );

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
	// `使用CUBEMAP貼圖功能`
	glEnable(GL_TEXTURE_CUBE_MAP);
	// `套用CUBEMAP貼圖`
	glBindTexture( GL_TEXTURE_CUBE_MAP, g_TextureID );
	// Trilinear filter
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// `設定轉換矩陣`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// `設定Vertex Array`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VN), g_pSphereVertices);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(3, GL_FLOAT, sizeof(Vertex_VN), g_pSphereVertices[0].m_Normal);
	// `畫球`
	glDrawElements(
		GL_TRIANGLES, // `指定所要畫的基本圖形種類`
		g_iNumSphereIndices, // `有幾個索引值`
		GL_UNSIGNED_SHORT, // `索引值的型態`
		g_pSphereIndices // `索引值陣列`
		);
	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
