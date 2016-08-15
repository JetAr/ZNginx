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
	// 設定投影矩陣
	Matrix4x4 projection_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &projection_matrix);
	// 把正向跟反向的面都畫出來
	glDisable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
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
	// `開啟打光功能`
	glEnable(GL_LIGHTING);
	// `開啟/關閉auto normalize功能`
	if ( g_bAutoNormalize )
		glEnable(GL_NORMALIZE);
	else
		glDisable(GL_NORMALIZE);
	// `開啟/關閉localviewer功能`
	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, g_bLocalViewer ? 1 : 0 );
	// `設定環境光`
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, (float *)&g_vGlobal_AmbientLight);
	// `設定物件材質的反光能力`
	glMaterialfv(GL_FRONT, GL_AMBIENT, (float *) &g_vMaterialAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, (float *) &g_vMaterialDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, (float *) &g_vMaterialSpecular);
	glMaterialfv(GL_FRONT, GL_EMISSION, (float *) &g_vMaterialEmissive);
	glMaterialf (GL_FRONT, GL_SHININESS, g_fMaterialShininess);
	// `設定光源`
	for ( int i=0; i<g_iNumLights; i++ )
	{
		// `每個光源分別用GL_LIGHT0,GL_LIGHT1,GL_LIGHT2來表示`
		// `它們在定義時是依順序排列,可以用GL_LIGHT0+n來取得光源ID`
		int LightID = GL_LIGHT0 + i;

		if ( g_Lights[i].m_bEnabled )
		{
			// 打開這盞光
			glEnable(LightID);

			switch(g_Lights[i].m_eType)
			{
			case LIGHT_DIRECTIONAL:
				// `光源方向`
				// `把一個(x,y,z,w)的向量傳入GL_POSITION, 如果w值是0會被視為是方向光.`
				glLightfv(LightID, GL_POSITION, (float *)&(-g_Lights[i].m_vDirection) );
				break;
			case LIGHT_POINT:
				// `光源位置`
				glLightfv(LightID, GL_POSITION, (float *)&g_Lights[i].m_vPosition);
				// `隨距離的衰減值` 
				// 1/(CONSTANT + LINEAR*d + QUADRATIC*d^2) 
				// `公式中的CONSTANT,LINEAR,QUADRATIC值`
				glLightf(LightID, GL_CONSTANT_ATTENUATION,	g_Lights[i].m_vAttenuation[0]);
				glLightf(LightID, GL_LINEAR_ATTENUATION,	g_Lights[i].m_vAttenuation[1]);
				glLightf(LightID, GL_QUADRATIC_ATTENUATION, g_Lights[i].m_vAttenuation[2]);
				break;
			case LIGHT_SPOT:
				// 光源位置
				glLightfv(LightID, GL_POSITION, (float *)&g_Lights[i].m_vPosition);
				// `隨距離的衰減值`
				// 1/(CONSTANT + LINEAR*d + QUADRATIC*d^2) 
				// `公式中的CONSTANT, LINEAR, QUADRATIC值`
				glLightf(LightID, GL_CONSTANT_ATTENUATION,	g_Lights[i].m_vAttenuation[0]);
				glLightf(LightID, GL_LINEAR_ATTENUATION,	g_Lights[i].m_vAttenuation[1]);
				glLightf(LightID, GL_QUADRATIC_ATTENUATION, g_Lights[i].m_vAttenuation[2]);
				// `光柱的方向`
				glLightfv(LightID, GL_SPOT_DIRECTION, (float *)&g_Lights[i].m_vDirection);
				// `光柱圓錐的一半角度`
				glLightf(LightID, GL_SPOT_CUTOFF, g_Lights[i].m_fSpotlightCutoff * 0.5f);
				// `光柱圓錐的角度衰減`
				glLightf(LightID, GL_SPOT_EXPONENT, g_Lights[i].m_fSpotlightExponent);
				break;
			}
			// `光源的Ambient值`
			glLightfv(LightID, GL_AMBIENT, (float *)&g_Lights[i].m_vAmbientColor); 
			// `光源的Diffuse值`
			glLightfv(LightID, GL_DIFFUSE, (float *)&g_Lights[i].m_vDiffuseColor);
			// `光源的specular值`
			glLightfv(LightID, GL_SPECULAR, (float *)&g_Lights[i].m_vSpecularColor);
		}
		else
		{
			// `關閉這盞光`
			glDisable(LightID);
		}
	}
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_MODELVIEW);
	// 設定要用陣列的方式傳入頂點位置跟顏色
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(Vertex_V3N3), g_pGridVertices[0].m_Position);
	glNormalPointer(GL_FLOAT, sizeof(Vertex_V3N3), g_pGridVertices[0].m_Normal);

	// 計算出一個可以轉換到鏡頭座標系的矩陣
	Matrix4x4 view_matrix = GutMatrixLookAtRH(g_eye, g_lookat, g_up);
	// 套用鏡頭轉換矩陣
	glLoadMatrixf( (float *) &view_matrix);
	// 設定光源, 所有光源會轉換到鏡頭座標系,所以要先套用鏡頭轉換矩陣
	SetupLightingOpenGL();

	Matrix4x4 world_view_matrix = g_world_matrix * view_matrix;
	glLoadMatrixf( (float *) &world_view_matrix);
	// 畫出格子
	glDrawElements (GL_TRIANGLE_STRIP, g_iNumGridIndices, GL_UNSIGNED_SHORT, g_pGridIndices);
	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
