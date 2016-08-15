#include <windows.h>
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceOpenGL(void)
{
	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `投影矩陣`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, 1.0f, 1.0f, 100.0f);
	// `設定視角轉換矩陣`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	// `沒事做`
	return true;
}

// `使用OpenGL來繪圖`
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// `設定好GPU要去哪讀取頂點資料`
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(4, GL_FLOAT, sizeof(Vector4), g_vertices);
	// `設定要變更GL_MODELVIEW矩陣`
	glMatrixMode(GL_MODELVIEW);	

	// `4個金字塔的位置`
	Vector4 pos[4] = 
	{
		Vector4(-1.0f, -1.0f, 0.0f),
		Vector4( 1.0f, -1.0f, 0.0f),
		Vector4(-1.0f,  1.0f, 0.0f),
		Vector4( 1.0f,  1.0f, 0.0f),
	};

	Matrix4x4 world_matrix;
	Matrix4x4 world_view_matrix;

	for ( int i=0; i<4; i++ )
	{
		// `得到位移矩陣`
		world_matrix.Translate_Replace(pos[i]); 
		world_view_matrix = world_matrix * g_view_matrix;
		// `設定轉換矩陣'
		glLoadMatrixf( (float *) &world_view_matrix);
		// `畫出金字塔的8條邊線`
		glDrawArrays(GL_LINES, 0, 16);
	}

	// `把背景backbuffer呈現出來`
	GutSwapBuffersOpenGL();
}
