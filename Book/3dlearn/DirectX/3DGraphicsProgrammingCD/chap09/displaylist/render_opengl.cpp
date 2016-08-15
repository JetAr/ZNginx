#include <windows.h>
#include "glew.h"
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_view_matrix;
static GLuint g_quad_list[4];

GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	const char *extension = (const char *)glGetString(GL_EXTENSIONS);
	printf("OpenGL Extension : \n%s\n", extension);

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

	GLint modes[] = {
		GL_REPEAT,// 左下角
		GL_MIRRORED_REPEAT, // 左上角
		GL_CLAMP_TO_EDGE, // 右下角
		GL_CLAMP_TO_BORDER, // 右上角
	};

	float border_color[4] = {0.5f, 0.5f, 0.5f, 0.5f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

	Vertex_VT *vertex = NULL;

	for ( int i=0; i<4; i++ )
	{
		g_quad_list[i] = glGenLists(1);
		// `建立一筆新的Display List記錄`
		glNewList(g_quad_list[i], GL_COMPILE);
		// `套用貼圖`
		glBindTexture( GL_TEXTURE_2D, g_TextureID );
		// `使用貼圖內插功能`
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// `設定貼圖座標解讀模式`
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, modes[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, modes[i]);
		// `畫出看板`
		glBegin(GL_QUADS);
		vertex = &g_Quads[i][0];
		glTexCoord2f(vertex->m_Texcoord[0], vertex->m_Texcoord[1]);
		glVertex3fv(vertex->m_Position);

		vertex = &g_Quads[i][1];
		glTexCoord2f(vertex->m_Texcoord[0], vertex->m_Texcoord[1]);
		glVertex3fv(vertex->m_Position);

		vertex = &g_Quads[i][2];
		glTexCoord2f(vertex->m_Texcoord[0], vertex->m_Texcoord[1]);
		glVertex3fv(vertex->m_Position);

		vertex = &g_Quads[i][3];
		glTexCoord2f(vertex->m_Texcoord[0], vertex->m_Texcoord[1]);
		glVertex3fv(vertex->m_Position);
		glEnd();

		glEndList();
	}

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
	// `鏡頭座標轉換矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `轉換矩陣`
	Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	for ( int i=0; i<4; i++ )
	{
		// `重復Display List中所記錄的動作`
		glCallList(g_quad_list[i]);
	}

	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
