#include <windows.h>

#include "glew.h" // OpenGL extension
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutModel_OpenGL.h"
#include "render_data.h"

static Matrix4x4 g_projection_matrix;

static CGutModel_OpenGL g_Model_textured_OpenGL;

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
	glDepthFunc(GL_LESS);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_textured_OpenGL.ConvertToOpenGLModel(&g_Model_textured);

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
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();

	Vector4 light_pos(5.0f, 0.0f, 5.0f);
	Vector4 light_lookat(0.0f, 0.0f, 0.0f);
	Vector4 light_up(0.0f, 1.0f, 0.0f);

	Matrix4x4 light_view = GutMatrixLookAtRH(light_pos, light_lookat, light_up);
	Matrix4x4 light_world_view = world_matrix * light_view;
	Matrix4x4 shadow_matrix;
	// 建立shadow volume
	// 建立shadow volume
	if ( g_bDirectionalLight )
	{
		g_ShadowVolume.BuildShadowVolume_DirectionalLight(light_world_view, 20.0f, true);
		shadow_matrix = light_view;
		shadow_matrix.FastInvert();
	}
	else
	{
		g_ShadowVolume.BuildShadowVolume_PointLight(light_pos, world_matrix, 20.0f, true);
		shadow_matrix.Identity();
	}

	/*
	// 建立shadow volume
	if ( g_bDirectionalLight )
	{
	Matrix4x4 ident_matrix; ident_matrix.Identity();
	g_ShadowVolume.BuildShadowVolume(light_world_view, ident_matrix, 20.0f, true);
	shadow_matrix = light_view;
	shadow_matrix.FastInvert();
	}
	else
	{
	Matrix4x4 light_proj = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 1.0f, 100.0f);
	g_ShadowVolume.BuildShadowVolume(light_world_view, light_proj, 20.0f, true);
	shadow_matrix = light_view;
	shadow_matrix.FastInvert();
	}
	*/

	Matrix4x4 world_view_matrix = world_matrix * view_matrix;

	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	// 畫出模型
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &world_view_matrix);
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
		g_Model_textured_OpenGL.Render();
	}
	// 畫出墻壁
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);

		sModelMaterial_OpenGL material;
		material.m_Diffuse.Set(0.0f, 0.0f, 1.0f);
		material.m_bCullFace = false;
		material.Submit(NULL);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_Quad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}

	// 關閉zbuffer更新功能
	glDepthMask(GL_FALSE);
	// 開啟stencil test
	glEnable(GL_STENCIL_TEST);
	// 在Stencil Buffer上標示出陰影區域
	{
		sModelMaterial_OpenGL material;
		material.m_bCullFace = false;
		material.Submit(NULL);
		glDisable(GL_CULL_FACE);

		world_view_matrix = shadow_matrix * view_matrix;
		glLoadMatrixf( (float *) &world_view_matrix);
		// 設定頂點資料格式
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vector4), g_ShadowVolume.m_pShadowVolume);
		// 不更新framebuffer
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
		// 讓 stencil test 永遠成立
		glStencilFunc(GL_ALWAYS, 0x01, 0xff);
		// 正面的三角形會遞減 Stencil Buffer
		// wrap to 0 if stencil buffer > 255
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);
		// 背面的三角形會遞增 Stencil Buffer
		// wrap to 255 if stencil buffer < 0
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
		// 畫出 Shadow Volume
		glDrawArrays(GL_TRIANGLES, 0, g_ShadowVolume.m_iNumShadowVolumeFaces * 3);
		// 恢復更新framebuffer跟zbuffer
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_KEEP, GL_KEEP);
		glStencilOpSeparate(GL_BACK, GL_KEEP, GL_KEEP, GL_KEEP);
	}
	// 畫出陰影
	{
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		// 使用黑色再畫一次墻壁
		sModelMaterial_OpenGL material;
		material.m_bCullFace = false;
		material.m_Diffuse.Set(0.0f, 0.0f, 0.0f, 1.0f);
		material.Submit(NULL);
		// 只更新stencil buffer上值為1的像素
		glEnable(GL_STENCIL_TEST);
		glStencilFunc(GL_EQUAL, 0x01, 0xff);

		glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), g_FullScreenQuad);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	}
	glDisable(GL_STENCIL_TEST);

	// 觀察shadow volume, 除錯用.
	if ( g_bDrawShadowVolume )
	{
		glMatrixMode(GL_PROJECTION);
		glLoadMatrixf( (float *) &g_projection_matrix);
		glMatrixMode(GL_MODELVIEW);
		world_view_matrix = shadow_matrix * view_matrix;
		glLoadMatrixf( (float *) &world_view_matrix);

		// 使用黑色再畫一次墻壁
		sModelMaterial_OpenGL material;
		material.m_bCullFace = false;
		material.m_Diffuse.Set(1.0f);
		material.Submit(NULL);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, sizeof(Vector4), g_ShadowVolume.m_pShadowVolume);
		glDrawArrays(GL_TRIANGLES, 0, g_ShadowVolume.m_iNumShadowVolumeFaces * 3);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	// 重新啟動zbuffer更新功能
	glDepthMask(GL_TRUE);

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
