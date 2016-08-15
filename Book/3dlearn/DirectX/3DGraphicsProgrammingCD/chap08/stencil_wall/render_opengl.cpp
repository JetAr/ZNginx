#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_view_matrix;

GLuint g_Texture0_ID = 0;
GLuint g_Texture1_ID = 0;

bool InitResourceOpenGL(void)
{
	// 鏡頭座標系轉換矩陣
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 投影矩陣
	Matrix4x4 projection_matrix = GutMatrixOrthoRH_OpenGL(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// 載入貼圖
	g_Texture0_ID = GutLoadTexture_OpenGL("../../textures/brickwall_broken.tga");
	// 套用Trilinear Filter
	glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
	// 載入貼圖
	g_Texture1_ID = GutLoadTexture_OpenGL("../../textures/spotlight_effect.tga");
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
	g_fOrthoWidth = g_fOrthoSize;
	g_fOrthoHeight = g_fOrthoSize;
	if ( aspect > 1.0f )
		g_fOrthoHeight *= aspect;
	else
		g_fOrthoWidth /= aspect;

	Matrix4x4 projection_matrix = GutMatrixOrthoRH_OpenGL(g_fOrthoWidth, g_fOrthoHeight, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 設定要用陣列的方式傳入頂點位置跟顏色
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// 不需要 zbuffer test
	glDisable(GL_DEPTH_TEST);

	{
		// 把modelview, projection矩陣設成單位矩陣.
		glMatrixMode(GL_MODELVIEW); 
		glLoadIdentity();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix(); // 把projection矩陣存放在stack中
		glLoadIdentity();

		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.5f);

		if ( g_bStencil )
		{
			// 啟動 Stencil Buffer Test
			glEnable(GL_STENCIL_TEST);
			// Stencil Test 判斷條件設定成永遠成立
			// Stencil 參考值設定為 1
			// Stencil Mask 設定成 0xff = 255
			glStencilFunc(GL_ALWAYS, 1, 0xff);
			// Stencil Test 成立時, 把 Stencil 參考值填入 Stencil Buffer 中.
			// 前兩個 GL_KEEP 是指當 Stencil 跟 ZBuffer Test 不成立時, 不去改變 Stencil Buffer
			glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}

		// 套用貼圖
		glBindTexture(GL_TEXTURE_2D, g_Texture0_ID);
		// 畫出矩形, 同時會清除ZBuffer.
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		// 把存放在stack中的projection取回來
		glPopMatrix();
		glDepthFunc(GL_LESS);
		glDisable(GL_ALPHA_TEST);
	}

	{
		if ( g_bStencil )
		{
			// 只更新畫面上Stencil值為1的像素
			glStencilFunc(GL_EQUAL, 1, 0xff);
			// 不去更新 Stencil Buffer 數值
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		}

		// 開啟混色功能
		glEnable(GL_BLEND);
		// 混色公式 = source_color * 1 + dest_color * 1
		glBlendFunc(GL_ONE, GL_ONE);
		// 套用貼圖
		glBindTexture(GL_TEXTURE_2D, g_Texture1_ID);
		// 套入轉換矩陣
		Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
		Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);
		// 畫出矩形
		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
		glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// 關閉混色功能		
		glDisable(GL_BLEND);
	}

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
