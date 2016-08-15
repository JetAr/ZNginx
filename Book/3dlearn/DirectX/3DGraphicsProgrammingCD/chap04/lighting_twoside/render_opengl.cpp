#include <windows.h>
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;

bool InitResourceOpenGL(void)
{
	// 使用zbuffer test
	glEnable(GL_DEPTH_TEST);
	// 計算出一個可以轉換到鏡頭座標系的矩陣
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 設定投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	// 沒事做
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

void SetupLightingOpenGL(void)
{
	// `設定環境光`
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float *)&g_vGlobal_AmbientLight);
	// `使用two side lighting功能`
	glLightModeli (GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	// `設定物件材質正面的反光能力`
	glMaterialfv(GL_FRONT, GL_AMBIENT, &g_vMaterialAmbient[0]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &g_vMaterialDiffuse[0]);
	glMaterialfv(GL_FRONT, GL_SPECULAR, &g_vMaterialSpecular[0]);
	glMaterialfv(GL_FRONT, GL_EMISSION, &g_vMaterialEmissive[0]);
	glMaterialf (GL_FRONT, GL_SHININESS, g_fMaterialShininess);
	// `設定物件材質背面的反光能力`
	glMaterialfv(GL_BACK, GL_AMBIENT, &g_vMaterialAmbient_Back[0]);
	glMaterialfv(GL_BACK, GL_DIFFUSE, &g_vMaterialDiffuse_Back[0]);
	glMaterialfv(GL_BACK, GL_SPECULAR, &g_vMaterialSpecular_Back[0]);
	glMaterialfv(GL_BACK, GL_EMISSION, &g_vMaterialEmissive_Back[0]);
	glMaterialf (GL_BACK, GL_SHININESS, g_fMaterialShininess_Back);
	// `開啟打光功能`
	glEnable(GL_LIGHTING);
	// `開啟第0號光`
	glEnable(GL_LIGHT0);
	// `把一個(x,y,z,w)的向量傳入GL_POSITION, 如果w值是0會被視為是方向`
	glLightfv(GL_LIGHT0, GL_POSITION, (float *)&(-g_vLightDirection) );
	// `光源的Ambient值`
	glLightfv(GL_LIGHT0, GL_AMBIENT, &g_vLightAmbient[0]); 
	// `光源的Diffuse值`
	glLightfv(GL_LIGHT0, GL_DIFFUSE, &g_vLightDiffuse[0]);
	// `光源的Specular值`
	glLightfv(GL_LIGHT0, GL_SPECULAR, &g_vLightSpecular[0]);
}

// `使用OpenGL來繪圖`
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	// `把正向跟反向的面都畫出來`
	glDisable(GL_CULL_FACE);
	// `設定光源`
	glLoadMatrixf( (float *) &g_view_matrix);
	SetupLightingOpenGL();
	// `設定要用陣列的方式傳入頂點位置跟面向`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VN), &g_Quad[0].m_Position);
	glNormalPointer(GL_FLOAT, sizeof(Vertex_VN), &g_Quad[0].m_Normal);
	// `計算并套入轉換矩陣`
	Matrix4x4 world_view_matrix = g_world_matrix * g_view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	// `畫出一片矩形`
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
