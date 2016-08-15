#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static Matrix4x4 g_projection_matrix;
static Matrix4x4 g_mirror_view_matrix;

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
	glEnable(GL_NORMALIZE);
	glDisable(GL_CULL_FACE);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	g_Model_OpenGL.Release();

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

static void RenderModelOpenGL(bool mirror, Vector4 *pPlane)
{
	Matrix4x4 view_matrix;

	if ( mirror )
	{
		Vector4 vEye = g_Control.GetCameraPosition();
		Vector4 vLookAt = g_Control.m_vLookAt;
		Vector4 vUp = g_Control.m_vUp;

		Vector4 mirror_eye = MirrorPoint(vEye, *pPlane);
		Vector4 mirror_lookat = MirrorPoint(vLookAt, *pPlane);
		Vector4 mirror_up = MirrorVector(vUp, *pPlane);

		Matrix4x4 temp_matrix = GutMatrixLookAtRH(mirror_eye, mirror_lookat, mirror_up);

		// 因為是鏡射, 在轉換到鏡頭座標系後要做個左右對調的動作.
		Matrix4x4 mirror_x;
		mirror_x.Identity();
		mirror_x.Scale(-1.0f, 1.0f, 1.0f);

		view_matrix = temp_matrix * mirror_x;
		g_mirror_view_matrix = view_matrix;

		// 右左對調後, 3角形的頂點排列順序會被反過來.
		glFrontFace(GL_CW);
	}
	else
	{
		view_matrix = g_Control.GetViewMatrix();
		glFrontFace(GL_CCW);
	}

	glMatrixMode(GL_MODELVIEW);
	Matrix4x4 world_view_matrix = g_Control.GetObjectMatrix() * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);

	g_Model_OpenGL.Render();

	glFrontFace(GL_CCW);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	Vector4 vPlane(0.0f, 0.0f, 1.0f, -g_mirror_z);

	// 清除畫面
	glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// 畫出茶壼
	RenderModelOpenGL(false, NULL);
	// 設定轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();;
	Matrix4x4 world_view_matrix = view_matrix;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	sModelMaterial_OpenGL material;
	material.Submit(NULL);
	// 設定頂點資料格式
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V), &g_Quad_v[0].m_Position);

	// 畫出鏡子, 同時把鏡子部分的stencil buffer設為1.
	{
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_ALWAYS, 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	// 把鏡子部分的zbuffer清為1.0
	{
		glStencilFunc(GL_EQUAL, 1, 0xff);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		// 把z值的輸出范圍設定為1~1, 也就是z永遠輸出1.
		glDepthRange(1.0f, 1.0f);
		glDepthFunc(GL_ALWAYS);
		// 只更新zbuffer,不需要更新顏色.
		glColorMask(false, false, false, false);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		// 恢復更新顏色的功能
		glColorMask(true, true, true, true);
		glDepthFunc(GL_LESS);
		// 把z值的范圍還原為0~1
		glDepthRange(0.0f, 1.0f);
	}
	// 畫出鏡子里的茶壼
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		RenderModelOpenGL(true, &vPlane);
		glDisable(GL_STENCIL_TEST);
	}
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
