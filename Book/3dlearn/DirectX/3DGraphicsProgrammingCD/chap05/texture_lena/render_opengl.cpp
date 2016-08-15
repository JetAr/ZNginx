#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>
#include <GL/GLAux.h>

#include "Gut.h"
#include "render_data.h"

//glaux.lib

static Matrix4x4 g_view_matrix;
GLuint g_TextureID = 0;

bool InitResourceOpenGL(void)
{
	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `設定投影矩陣`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	// `載入貼圖`
	AUX_RGBImageRec *pTextureImage = auxDIBImageLoad( "../../textures/lena.bmp" );
	if ( pTextureImage==NULL )
		return false;
	for ( int i=0; i<4; i++ )
	{
		g_Quad[i].m_Texcoord[1] = 1.0f - g_Quad[i].m_Texcoord[1];
	}
	// `產生一個貼圖物件`
	glGenTextures( 1, &g_TextureID );
	// `使用g_TextureID貼圖物件`
	glBindTexture( GL_TEXTURE_2D, g_TextureID );
	// `拷具貼圖資料`
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, 
		pTextureImage->sizeX, pTextureImage->sizeY, 0,
		GL_RGB, GL_UNSIGNED_BYTE, pTextureImage->data );
	// `設定顯示貼圖被縮小時使用線性內插 `
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_LINEAR );
	// `設定顯示貼圖被放大時使用線性外插 `
	glTexParameteri( GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER, GL_LINEAR );

	// `拷具完貼圖後, 就可以把原始資料消去.`
	if( pTextureImage->data )
		free( pTextureImage->data );

	free( pTextureImage );

	// `使用2D貼圖功能`
	glEnable(GL_TEXTURE_2D);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_TextureID )
	{
		glDeleteTextures(1, &g_TextureID);
		g_TextureID = 0;
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
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	// 把正向跟反向的面都畫出來
	glDisable(GL_CULL_FACE);
	// 設定要用陣列的方式傳入頂點位置跟顏色
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Position);
	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex_VT), &g_Quad[0].m_Texcoord);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &world_view_matrix);
	// 畫出格子
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
