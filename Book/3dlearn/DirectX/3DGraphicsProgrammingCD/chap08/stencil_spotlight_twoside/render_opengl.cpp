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
static CGutModel_OpenGL g_SpotLightModel_OpenGL;

static sModelMaterial_OpenGL g_material_stencilpass;
static sModelMaterial_OpenGL g_material_spotlightpass;

bool InitResourceOpenGL(void)
{
	if ( NULL==glStencilOpSeparate )
	{
		printf("Does not support two sided stencil buffer test\n");
		return false;
	}

	// 投影矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);
	g_SpotLightModel_OpenGL.ConvertToOpenGLModel(&g_SpotLightModel);

	g_material_stencilpass.m_bCullFace = false;

	g_material_spotlightpass.m_bBlend = true;
	g_material_spotlightpass.m_SrcBlend = GL_ONE;
	g_material_spotlightpass.m_DestBlend = GL_ONE;
	g_material_spotlightpass.m_Diffuse.Set(0.3f);

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
	// 轉換矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	// 畫茶壼
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);
		g_Model_OpenGL.Render();
	}

	glEnable(GL_STENCIL_TEST);
	glDepthMask(GL_FALSE);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &view_matrix);

	// 在Stencil Buffer中標示出光柱照射的區域
	{
		// 關閉 face culling
		glDisable(GL_CULL_FACE);

		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		glStencilFunc(GL_ALWAYS, 1, 0xff);

		// 正面的 Stencil Test 設定
		// GL_INCR_WRAP = wrap to 0 if stencil buffer > 255
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_INCR_WRAP);

		// 背面的 Stencil Test 設定
		// GL_DECR_WRAP = wrap to 255 if stencil buffer < 0
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_DECR_WRAP);

		g_SpotLightModel_OpenGL.Render(0);
		glEnable(GL_CULL_FACE);

		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}
	// 畫出光柱照射到的區域
	{
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilFunc(GL_EQUAL, 1, 0xff);

		CGutModel_OpenGL::SetMaterialOverwrite(&g_material_spotlightpass);
		g_SpotLightModel_OpenGL.Render();
		CGutModel_OpenGL::SetMaterialOverwrite(NULL);
	}
	glDisable(GL_STENCIL_TEST);
	// 畫出光柱
	{
		g_SpotLightModel_OpenGL.Render();
	}

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glDepthMask(GL_TRUE);

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
