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

// callback function. 視窗大小改變時會被呼叫, 并傳入新的視窗大小.
void ResizeWindowOpenGL(int width, int height)
{
	// 使用新的視窗大小做為新的繪圖解析度
	glViewport(0, 0, width, height);
	// 投影矩陣, 重設水平跟垂直方向的視角.
	float aspect = (float) height / (float) width;
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(60.0f, aspect, 0.1f, 100.0f);
	// 設定視角轉換矩陣
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
}

// `使用OpenGL來繪圖`
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
	Matrix4x4 world_view_matrix;

	glMatrixMode(GL_MODELVIEW);
	// `載入鏡頭轉換矩陣`
	glLoadMatrixf( (float *) &view_matrix );
	glPushMatrix(); // `把目前的矩陣存到stack里`
	// `太陽, 套入滑鼠的旋轉控制.`
	glRotatef(FastMath::RadianToDegree(g_fRotate_X), 1.0f, 0.0f, 0.0f);
	glRotatef(FastMath::RadianToDegree(g_fRotate_Y), 0.0f, 1.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pSunVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pSunVertices[0].m_RGBA);
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
	// `水星`
	glPushMatrix(); // `把目前的矩陣存到stack里`
	float mercury_rotate_y = 360.0f * (g_simulation_days / days_a_year_mercury); 
	glRotatef(mercury_rotate_y, 0.0f, 1.0f, 0.0f);
	glTranslatef(mercury_to_sun_distance, 0.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pMoonVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pMoonVertices[0].m_RGBA);
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
	// `從stack里pop出一個矩陣, 并套用到目前所指定要操作的矩陣`GL_MODELVIEW
	glPopMatrix(); 
	// `金星`
	glPushMatrix(); // `把目前的矩陣存到stack里`
	float venus_rotate_y = 360.0f * (g_simulation_days / days_a_year_venus); 
	glRotatef(venus_rotate_y, 0.0f, 1.0f, 0.0f);
	glTranslatef(venus_to_sun_distance, 0.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pMoonVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pMoonVertices[0].m_RGBA);
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
	// `從stack里pop出一個矩陣, 并套用到目前所指定要操作的矩陣`GL_MODELVIEW
	glPopMatrix(); 
	// `地球`
	glPushMatrix();// `把目前的矩陣存到stack里`
	float earth_rotate_y = 360.0f * (g_simulation_days / days_a_year); 
	glRotatef(earth_rotate_y, 0.0f, 1.0f, 0.0f);
	glTranslatef(earth_to_sun_distance, 0.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pEarthVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pEarthVertices[0].m_RGBA);
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
	// `月球`
	float moon_rotate_y = 360.0f * (g_simulation_days / days_a_month);
	glRotatef(moon_rotate_y, 0.0f, 1.0f, 0.0f);
	glTranslatef(moon_to_earth_distance, 0.0f, 0.0f);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_VC), g_pMoonVertices);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex_VC), g_pMoonVertices[0].m_RGBA);
	glDrawElements(GL_TRIANGLES, g_iNumSphereIndices, GL_UNSIGNED_SHORT, g_pSphereIndices);
	// `從stack里pop出一個矩陣, 并套用到目前所指定要操作的矩陣GL_MODELVIEW`
	glPopMatrix(); 
	glPopMatrix();

	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
