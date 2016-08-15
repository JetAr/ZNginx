#include <windows.h>

// OpenGL extension
#include "glew.h" 
// Standard OpenGL header
#include <GL/gl.h>

#include "Gut.h"
#include "GutWin32.h"
#include "GutTexture_OpenGL.h"
#include "GutFont_OpenGL.h"
#include "GutModel_OpenGL.h"
#include "GutHeightmap_OpenGL.h"

#include "render_data.h"

static Matrix4x4 g_proj_matrix;
static GLuint g_texture;

bool InitResourceOpenGL(void)
{
	// 設定投影矩陣
	Matrix4x4 g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf( (float *) &g_proj_matrix);

	g_texture = GutLoadTexture_OpenGL("../../textures/particle.tga");

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

	float aspec = (float)height/(float)width;
	g_proj_matrix = GutMatrixPerspectiveRH_OpenGL(g_fFovW, aspec, 0.5f, 1000.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf((float*)&g_proj_matrix);
}

// 使用OpenGL來繪圖
void RenderFrameOpenGL(void)
{
	// 清除畫面
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// 
	Matrix4x4 view_matrix = g_Control.GetViewMatrix();
	Matrix4x4 vw_matrix = view_matrix;

	sModelMaterial_OpenGL mtl;
	mtl.m_bBlend = true;
	mtl.m_SrcBlend = GL_ONE;
	mtl.m_DestBlend = GL_ONE;
	mtl.m_Textures[0] =	g_texture;
	mtl.Submit(NULL);

	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf((float*)&vw_matrix);

	glDepthMask(GL_FALSE);

	if ( g_Particle.m_iNumParticles )
	{
		if ( g_bPointSprite )
		{
			int w,h; GutGetWindowSize(w, h);
			float fAspect = (float)h/(float)w;
			float fovH = FastMath::DegToRad(fAspect * g_fFovW);
			float tanH = FastMath::Tan(fovH * 0.5f);

			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(sPointSpriteVertex), g_Particle.m_pPointSpriteArray);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(sPointSpriteVertex), &g_Particle.m_pPointSpriteArray[0].m_Color);

			glEnable(GL_POINT_SPRITE_ARB);

			float quadratic[] =  { 0.0f, 0.0f, tanH*tanH*4.0f };

			glPointParameterfvARB( GL_POINT_DISTANCE_ATTENUATION_ARB, quadratic );
			glPointSize(32.0f);
			glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );

			glDrawArrays(GL_POINTS, 0, g_Particle.m_iNumParticles);

			glDisable(GL_POINT_SPRITE_ARB);
			glTexEnvf(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE);
		}
		else
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			glVertexPointer(3, GL_FLOAT, sizeof(sParticleVertex), g_Particle.m_pVertexArray);
			glEnableClientState(GL_COLOR_ARRAY);
			glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(sParticleVertex), &g_Particle.m_pVertexArray[0].m_Color);
			glEnableClientState(GL_TEXTURE_COORD_ARRAY);
			glTexCoordPointer(2, GL_FLOAT, sizeof(sParticleVertex), g_Particle.m_pVertexArray[0].m_fTexcoord);

			glDrawElements(GL_TRIANGLES, g_Particle.m_iNumParticles*6, 
				GL_UNSIGNED_SHORT, g_Particle.m_pIndexArray);

			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
		}
	}

	// 把背景backbuffer的畫面呈現出來
	GutSwapBuffersOpenGL();
}
