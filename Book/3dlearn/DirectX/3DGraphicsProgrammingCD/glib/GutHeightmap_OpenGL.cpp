#include "Gut.h"
#include "GutOpenGL.h"
#include "GutModel_OpenGL.h"
#include "GutHeightmap_OpenGL.h"

CGutHeightmapOpenGL::CGutHeightmapOpenGL(void)
{
}

CGutHeightmapOpenGL::~CGutHeightmapOpenGL()
{
	Release();
}

void CGutHeightmapOpenGL::Render(void)
{
	if ( m_iNumFaces==0 )
		return;

	sModelMaterial_OpenGL mtl;
	mtl.m_bCullFace = false;
	mtl.Submit(NULL);

	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	// 設定頂點資料格式
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, sizeof(sTerrainVertex), m_pVertexArray);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, sizeof(sTerrainVertex), &m_pVertexArray[0].m_Normal);

	glDrawElements(GL_TRIANGLE_STRIP, m_iNumFaces, GL_UNSIGNED_SHORT, m_pIndexArray);

	glPopClientAttrib();
}
