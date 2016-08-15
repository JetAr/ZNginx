#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// 投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// 載入貼圖
	g_TextureID = GutLoadTexture_OpenGL("../../textures/brick.tga");
	glBindTexture( GL_TEXTURE_2D, g_TextureID );
	// 使用2D貼圖功能
	glEnable(GL_TEXTURE_2D);
	// 把超過0-1范圍的座標解讀方法設為REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// 開啟自動產生貼圖座標功能
	// S/T分別代表貼圖座標的X/Y軸
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	// 設定成參考物件位置來產生貼圖座標
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
	// 列陣數值分別代表a*x+b*y+c*z+d*w的a,b,c,d值
	// 1*x+0*y+0*z+0*w = x, 貼圖座標x軸值等於頂點位置x值
	float fvTexcoordScale_S[] = {1.0f, 0.0f, 0.0f, 0.0f};
	glTexGenfv(GL_S, GL_EYE_PLANE, fvTexcoordScale_S);
	// 0*x+1*y+0*z+0*w = y, 貼圖座標y軸值等於頂點位置y值
	float fvTexcoordScale_T[] = {0.0f, 1.0f, 0.0f, 0.0f};
	glTexGenfv(GL_T, GL_EYE_PLANE, fvTexcoordScale_T);

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
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// 把正向跟反向的面都畫出來
	glDisable(GL_CULL_FACE);
	// 設定要用陣列的方式傳入頂點位置跟顏色
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex), &g_Quad[0].m_Position);
	// Trilinear filter
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// 設定轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// 畫出格子
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
