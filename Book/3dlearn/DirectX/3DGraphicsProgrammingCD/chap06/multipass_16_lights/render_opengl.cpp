#include <windows.h>

#include "glew.h" // OpenGL extension
#include <GL/gl.h>

#include "Gut.h"
#include "GutTexture_OpenGL.h"
#include "GutModel_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_projection_matrix;

CGutModel_OpenGL g_Model_OpenGL;

bool InitResourceOpenGL(void)
{
	// 投影矩陣
	g_projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 1000.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_projection_matrix);

	CGutModel::SetTexturePath("../../textures/");

	g_Model_OpenGL.ConvertToOpenGLModel(&g_Model);

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

void SetupLightingOpenGL(Light *pLightArray, int num_lights)
{
	glEnable(GL_LIGHTING);
	glEnable(GL_NORMALIZE);

	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, &g_vAmbientLight[0]);

	int i;

	for ( i=0; i<num_lights; i++ )
	{
		int LightID = GL_LIGHT0 + i;

		Light *pLight = pLightArray + i;

		glEnable(LightID);

		glLightfv(LightID, GL_POSITION, &pLight->m_Position[0]);
		glLightfv(LightID, GL_DIFFUSE, &pLight->m_Diffuse[0]);
		glLightfv(LightID, GL_SPECULAR, &pLight->m_Specular[0]);

		glLightf(LightID, GL_CONSTANT_ATTENUATION,	0.0f);
		glLightf(LightID, GL_LINEAR_ATTENUATION,	2.0f);
		glLightf(LightID, GL_QUADRATIC_ATTENUATION, 0.0f);
	}

	for ( ; i<8; i++ )
	{
		glDisable(GL_LIGHT0 + i);
	}
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// `把隱藏面測試規則設為小於或等於` <=
	glDepthFunc(GL_LEQUAL);

	// `設定轉換矩陣`
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 world_matrix = g_Control.GetObjectMatrix();
	Matrix4x4 world_view_matrix = world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);

	// `前8盞光`
	if ( g_iSwitch & 0x01 )
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);
		SetupLightingOpenGL(&g_Lights[0], 8);

		glLoadMatrixf( (float *) &world_view_matrix);
		g_Model_OpenGL.Render();
	}

	// `後8盞光`
	if ( g_iSwitch & 0x02 )
	{
		glMatrixMode(GL_MODELVIEW);
		glLoadMatrixf( (float *) &view_matrix);
		SetupLightingOpenGL(&g_Lights[8], 8);

		glLoadMatrixf( (float *) &world_view_matrix);

		if ( g_iSwitch & 0x01 )
		{
			// 如果前8盞光有開, 要使用混色來把下8盞光疊加上去.
			glEnable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ONE);
			// ~SUBMIT_BLEND 會取消套用檔案中的混色設定.
			g_Model_OpenGL.Render(~SUBMIT_BLEND);
		}
		else
		{
			g_Model_OpenGL.Render();
		}
	}

	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
