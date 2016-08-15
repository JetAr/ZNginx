#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static Matrix4x4 g_projection_matrix;

static CGutModel_OpenGL g_Model_OpenGL;

bool InitResourceOpenGL(void)
{
	// 投影矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	return true;
}

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;

	// 畫出茶壼, 并更新 Stencil Buffer.
	{
		// 畫出茶壼
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &g_projection_matrix);

		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);

		// 啟動 Stencil Test
		glEnable(GL_STENCIL_TEST);
		// 設定 Stencil Test 條件, 讓它永遠成立.
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		// 遞增 Stencil Buffer 內容
		glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
		// 畫出茶壼
		g_Model_OpenGL.Render();
		// 不再去變更 Stencil Buffer 內容
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	sModelMaterial_OpenGL material;
	material.Submit(NULL);

	// 讓它蓋滿整個畫面, 不需要隱藏面測試.
	glDepthFunc(GL_ALWAYS);

	// 設定頂點資料格式
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V), &g_Quad[0].m_Position);

	// 用綠色標示只更新過1次的像素
	{
		glStencilFunc(GL_EQUAL, 1, 0xff);
		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// 用藍色標示更新過2次的像素
	{
		glStencilFunc(GL_EQUAL, 2, 0xff);
		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// 用紅色來標示更新超過3次以上的像素
	{
		glStencilFunc(GL_EQUAL, 3, 0xff);
		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// 用紅色來標示更新超過3次以上的像素
	{
		// 如果3小於stencil buffer的值, 條件才成立. 
		// 也就是stencil buffer值要大於3的意思
		glStencilFunc(GL_LESS, 3, 0xff); 
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// 還原設定
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDepthFunc(GL_LESS);
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
