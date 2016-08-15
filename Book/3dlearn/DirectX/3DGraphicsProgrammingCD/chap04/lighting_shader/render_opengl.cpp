#include <windows.h>
// OpenGL extensions header
#include "glew.h"
#include "wglew.h"
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "render_data.h"

static Matrix4x4 g_view_matrix;
static Matrix4x4 g_view_proj_matrix;
static Matrix4x4 g_proj_matrix;

static GLuint g_DirLight_VP = 0;
static GLuint g_PointLight_VP = 0;
static GLuint g_SpotLight_VP = 0;
static GLuint g_Selected_VP = 0;
static GLuint g_FragmentProgramID = 0;

bool InitResourceOpenGL(void)
{
	// `載入pixel program, 它可以跟不同的打光vertex shader共用.`
	g_FragmentProgramID = GutLoadFragmentProgramOpenGL_ASM("../../shaders/vertex_lighting_directional.glpp");
	if ( g_FragmentProgramID==0 )
		return false;
	// `載入diretional light vertex program`
	g_DirLight_VP = GutLoadVertexProgramOpenGL_ASM("../../shaders/vertex_lighting_directional.glvp");
	if ( g_DirLight_VP==0 )
		return false;
	// `載入point light vertex program`
	g_PointLight_VP = GutLoadVertexProgramOpenGL_ASM("../../shaders/vertex_lighting_point.glvp");
	if ( g_PointLight_VP==0 )
		return false;
	// `載入spot light vertex program`
	g_SpotLight_VP = GutLoadVertexProgramOpenGL_ASM("../../shaders/vertex_lighting_spot.glvp");
	if ( g_SpotLight_VP==0 )
		return false;

	// `轉換到鏡頭座標系的矩陣`
	g_view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// `設定投影矩陣`
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	// `把view_matrix跟proj_matrix相乘, 可以滅少日後的計算.`
	g_view_proj_matrix = g_view_matrix * g_proj_matrix;
	// `把正向跟反向的面都畫出來`
	glDisable(GL_CULL_FACE);
	// `啟動zbuffer test`
	glEnable(GL_DEPTH_TEST);

	return true;
}

bool ReleaseResourceOpenGL(void)
{
	if ( g_DirLight_VP )
	{
		GutReleaseVertexProgramOpenGL(g_DirLight_VP);
		g_DirLight_VP = 0;
	}

	if ( g_PointLight_VP )
	{
		GutReleaseVertexProgramOpenGL(g_PointLight_VP);
		g_PointLight_VP = 0;
	}

	if ( g_SpotLight_VP )
	{
		GutReleaseVertexProgramOpenGL(g_SpotLight_VP);
		g_SpotLight_VP = 0;
	}

	if ( g_FragmentProgramID )
	{
		GutReleaseVertexProgramOpenGL(g_FragmentProgramID);
		g_FragmentProgramID = 0;
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
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspect, 0.1f, 100.0f);
	// 把view_matrix跟proj_matrix相乘, 可以滅少日後的計算
	g_view_proj_matrix = g_view_matrix * g_proj_matrix;
}

static void SetupLightingOpenGL(void)
{
	int base = 12;
	Vector4 vShininess = g_fMaterialShininess;
	Vector4 vAmbient, vDiffuse, vSpecular, vSpotLightCoeff;
	// `設定環境光`
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base, 1, &g_vAmbientLight[0]);

	//` 設定光源`
	switch(g_iSelectedLight)
	{
		// `方向光`
	default:
	case 0:
		g_Selected_VP = g_DirLight_VP;

		vAmbient = g_vMaterialAmbient * g_Lights[0].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[0].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[0].m_vSpecularColor;

		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 1, 1, &g_Lights[0].m_vDirection.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 2, 1, &vAmbient.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 3, 1, &vDiffuse.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 4, 1, &vSpecular.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 5, 1, &vShininess.x);

		break;
		// `點光源`
	case 1:
		g_Selected_VP = g_PointLight_VP;

		vAmbient = g_vMaterialAmbient * g_Lights[1].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[1].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[1].m_vSpecularColor;

		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 1, 1, &g_Lights[1].m_vPosition.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 2, 1, &g_Lights[1].m_vAttenuation.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 3, 1, &vAmbient.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 4, 1, &vDiffuse.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 5, 1, &vSpecular.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 6, 1, &vShininess.x);

		break;
		// `聚光燈`
	case 2:
		g_Selected_VP = g_SpotLight_VP;

		vAmbient = g_vMaterialAmbient * g_Lights[2].m_vAmbientColor;
		vDiffuse = g_vMaterialDiffuse * g_Lights[2].m_vDiffuseColor;
		vSpecular = g_vMaterialSpecular * g_Lights[2].m_vSpecularColor;

		float spotlight_cutoff_cosine = FastMath::Cos( FastMath::DegreeToRadian(g_Lights[2].m_fSpotlightCutoff * 0.5f) );
		vSpotLightCoeff.Set(spotlight_cutoff_cosine, g_Lights[2].m_fSpotlightExponent, 0.0f, 0.0f);

		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 1, 1, &g_Lights[2].m_vPosition.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 2, 1, &g_Lights[2].m_vDirection.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 3, 1, &g_Lights[2].m_vAttenuation.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 4, 1, &vSpotLightCoeff.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 5, 1, &vAmbient.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 6, 1, &vDiffuse.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 7, 1, &vSpecular.x);
		glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, base + 8, 1, &vShininess.x);

		break;
	}
}

// `使用OpenGL來繪圖`
void RenderFrameOpenGL(void)
{
	// `清除畫面`
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// `指定要使用Shader可程式化流程`
	glEnable(GL_VERTEX_PROGRAM_ARB);
	glEnable(GL_FRAGMENT_PROGRAM_ARB);
	// `設定光源`	
	SetupLightingOpenGL();
	// `設定使用哪個Vertex跟Fragment Program`
	glBindProgramARB(GL_VERTEX_PROGRAM_ARB, g_Selected_VP);
	glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB, g_FragmentProgramID);
	// `設定要用陣列的方式傳入頂點位置跟顏色`
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3N3), &g_pGridVertices[0].m_Position);
	glNormalPointer (GL_FLOAT, sizeof(Vertex_V3N3), &g_pGridVertices[0].m_Normal);
	// `計算并套入轉換矩陣`
	Matrix4x4 world_view_proj_matrix = g_world_matrix * g_view_proj_matrix; 
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 0, 4, &world_view_proj_matrix[0][0]);
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 4, 4, &g_world_matrix[0][0]);
	// `鏡頭位置, 計算Specular會用到.`
	glProgramLocalParameters4fvEXT(GL_VERTEX_PROGRAM_ARB, 8, 1, &g_eye[0]);
	// `畫出格子`
	glDrawElements (GL_TRIANGLE_STRIP, g_iNumGridIndices, GL_UNSIGNED_SHORT, g_pGridIndices);
	// `把背景backbuffer的畫面呈現出來`
	GutSwapBuffersOpenGL();
}
