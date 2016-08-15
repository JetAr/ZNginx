#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_projection_matrix;

GLuint g_Texture0_ID = 0;
GLuint g_Texture1_ID = 0;

bool InitResourceOpenGL(void)
{
	// 投影矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
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

	glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);

	// 把正向跟反向的面都畫出來
	glDisable(GL_CULL_FACE);
	// 啟動2D貼圖功能
	glEnable(GL_TEXTURE_2D);
	// 啟動zbuffer隱藏面測試
	glEnable(GL_DEPTH_TEST);

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
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// `設定要用陣列的方式傳入頂點位置跟顏色`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	{
		// `不再用單一顏色來清除畫面, 把整個畫面用磚墻圖片蓋過去.`
		// `把modelview, projection矩陣設成單位矩陣`
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		// `把projection矩陣存放在stack中`
		glPushMatrix(); 
		glLoadIdentity();

		// `ZBuffer測試條件設為永遠成立`
		glDepthFunc(GL_ALWAYS);
		// `套用貼圖`
		glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
		// `畫出矩形, 同時會清除ZBuffer.`
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_FullScreenQuad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_FullScreenQuad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// `把存放在stack中的projection取回來`
		glPopMatrix();
		glDepthFunc(GL_LESS);
	}

	{
		// `只需要清除zbuffer即可`
		glEnable(GL_ALPHA_TEST);
		// `設定只有alpha值大於0.5的圖素才會畫出來.`
		glAlphaFunc(GL_GREATER, 0.5f);
		// `套用貼圖`
		glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);
		// `設定轉換矩陣`
		Matrix4x4 view_matrix = g_Control.GetViewMatrix();
		Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
		Matrix4x4 world_view_matrix = world_matrix * view_matrix;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);
		// `畫出矩形`
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// `關閉alpha test`
		glDisable(GL_ALPHA_TEST);
	}

	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
