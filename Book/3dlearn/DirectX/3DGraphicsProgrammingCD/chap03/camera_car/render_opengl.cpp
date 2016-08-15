#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceOpenGL(void)
{
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	glMatrixMode(GL_MODELVIEW);	

	glEnable(GL_CULL_FACE);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	// 沒事做
	return true;
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// `設定要用陣列的方式傳入頂點位置跟顏色`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);

	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);

	Vector4 border(-15.0f, 0.0f, -15.0f);
	Vector4 grid_position = border;

	const int grids_x = 30;
	const int grids_z = 30;

	for ( int x=0; x<grids_x; x++ )
	{
		int grid_x = x & 0x07;
		grid_position[2] = border[2];

		for ( int z=0; z<grids_z; z++ )
		{
			int grid_z = z & 0x07;
			char c = g_map[grid_x][grid_z];

			// `設定轉換矩陣`
			Matrix4x4 object_matrix; object_matrix.Identity();

			if ( c==0 )
			{
				object_matrix = view_matrix;
				object_matrix.Translate(grid_position);
				glLoadMatrixf( (float *) &object_matrix);
				// `設定GPU要去哪讀取頂點座標資料`
				glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), &g_road_vertices[0].m_Position);
				// `設定GPU要去哪讀取頂點顏色資料`
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), &g_road_vertices[0].m_RGBA);
				// `畫出地板`
				glDrawElements(
					GL_TRIANGLE_STRIP, // `指定所要畫的基本圖形種類`
					4, // `有幾個索引值`
					GL_UNSIGNED_SHORT, // `索引值的型態`
					g_road_trianglestrip_indices // `索引值陣列`
					);
			}
			else
			{
				// `設定金字塔的高度`
				object_matrix.Scale_Replace(1.0f, (float) c, 1.0f);
				object_matrix[3] = grid_position;
				Matrix4x4 world_view_matrix = object_matrix * view_matrix;
				glLoadMatrixf( (float *) &world_view_matrix);
				// `設定GPU要去哪讀取頂點座標資料`
				glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), &g_pyramid_vertices[0].m_Position);
				// `設定GPU要去哪讀取頂點顏色資料`
				glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), &g_pyramid_vertices[0].m_RGBA);
				// `畫出金字塔`
				glDrawElements(
					GL_TRIANGLE_FAN, // `指定所要畫的基本圖形種類`
					6, // `有幾個索引值`
					GL_UNSIGNED_SHORT, // `索引值的型態`
					g_pyramid_trianglefan_indices // `索引值陣列`
					);
			}

			grid_position[2] += 1.0f;
		}

		grid_position[0] += 1.0f;
	}

	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
