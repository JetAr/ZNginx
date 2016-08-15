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
static Vector4 g_vFogColor(0.5f, 0.5f, 0.5f, 1.0f);

void InitState(float aspect)
{
	// 投影矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFOV, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_DEPTH_TEST);
}

bool InitResourceOpenGL(void)
{
	InitState(1.0f);

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
	InitState(aspect);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClearColor(g_vFogColor[0], g_vFogColor[1], g_vFogColor[2], g_vFogColor[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 object_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = object_matrix * view_matrix;
	// 設定投影轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);
	// 設定鏡頭轉換矩陣
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	// 設定霧
	glEnable(GL_FOG); // 開啟霧的功能
	glFogfv(GL_FOG_COLOR, &g_vFogColor[0]); // 顏色

	switch(g_iFogMode)
	{
	case 0:
		glDisable(GL_FOG);
		break;
	case 1:
		// 霧隨距離線性變濃
		glFogi(GL_FOG_MODE, GL_LINEAR);
		// 設定霧的范圍
		glFogf(GL_FOG_START, 0.0f);
		glFogf(GL_FOG_END, 10.0f);
		// 計算公式為
		// (fog_end - distance_to_camera) / (fog_end - fog_start)
		break;
	case 2:
		// `套用指數函式來變化的霧 `
		glFogi(GL_FOG_MODE, GL_EXP);
		glFogf(GL_FOG_DENSITY, 0.5f);
		// 計算公式為
		// power(e, -(fog_density * distance_to_camera))
		break;
	case 3:
		// `套用指數函式來變化的霧 `
		glFogi(GL_FOG_MODE, GL_EXP2);
		glFogf(GL_FOG_DENSITY, 0.5f);
		// 計算公式為
		// power(e, -(fog_density * distance_to_camera)^2)
		break;
	}

	// 畫出模型
	g_Model_OpenGL.Render();
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
