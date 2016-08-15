#include <windows.h>
// OpenGL 1.2+ & extensions
#include "glew.h"
#include "wglew.h"
// OpenGL 1.2-
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static GLuint g_VertexBufferID = 0;
static GLuint g_IndexBufferID = 0;

bool InitResourceOpenGL(void)
{
	// `投影矩陣`
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(90.0f, 1.0f, 0.1f, 100.0f);
	// `設定視角轉換矩陣`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	glMatrixMode(GL_MODELVIEW);	

	// `產生一塊新的Vertex Buffer Object`
	glGenBuffers(1, &g_VertexBufferID);
	// `使用g_VertexBufferID所代表的Vertex Buffer Object`
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferID);
	// `配置Vertex Buffer的大小, 并同時把g_pSphereVertices的內容拷具過進去.`
	glBufferData(GL_ARRAY_BUFFER, g_iNumSphereVertices * sizeof(Vertex_VC), g_pSphereVertices, GL_STATIC_COPY);

	// `產生一塊新的Index Buffer Object`
	glGenBuffers(1, &g_IndexBufferID);
	// `使用g_IndexBufferID所代表的Index Buffer Object`
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IndexBufferID);
	// `配置Index Buffer的大小, 并同時把g_pSphereIndices的內容拷具過去.`
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, g_iNumSphereIndices * 2, g_pSphereIndices, GL_STATIC_COPY);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_VertexBufferID )
	{
		glDeleteBuffers(1, &g_VertexBufferID);
		g_VertexBufferID = 0;
	}

	if ( g_IndexBufferID )
	{
		glDeleteBuffers(1, &g_IndexBufferID);
		g_IndexBufferID = 0;
	}

	return true;
}

void ResizeWindowOpenGL(int width, int height)
{
	glViewport(0, 0, width, height);
	// `投影矩陣`
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
	// `設定視角轉換矩陣`
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// `使用OpenGL來繪圖`
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	// `使用g\_VertexBufferID所代表的Vertex Buffer Object`
	glBindBuffer(GL_ARRAY_BUFFER, g_VertexBufferID);
	// `使用g\_IndexBufferID所代表的Index Buffer Object`
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_IndexBufferID);

	// `設定要用陣列的方式傳入頂點位置跟顏色`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	// `VertexPointer/ColorPointer不再傳入指標位置`
	// 而是傳入記憶體相對的位移值
	int voffset = (int) &g_pSphereVertices[0].m_Position - (int) &g_pSphereVertices[0];
	int coffset = (int) &g_pSphereVertices[0].m_RGBA - (int) &g_pSphereVertices[0];
	// `最後一個值是在記憶體中的起始位移`
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), (GLvoid *)voffset); // voffset=0
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), (GLvoid *)coffset); // coffset=12

	// `計算出一個可以轉換到鏡頭座標系的矩陣`
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf( (float *) &view_matrix);

	glDrawElements(
		GL_TRIANGLES, // `指定所要畫的基本圖形種類`
		g_iNumSphereIndices, // `有幾個索引值`
		GL_UNSIGNED_SHORT, // `索引值的型態`
		0 // `不再傳入陣列的指標, 而是傳用索引值起始的位移.`
		);

	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
