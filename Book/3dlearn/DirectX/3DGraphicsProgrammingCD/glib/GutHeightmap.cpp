#include <math.h>

#include "FastMath.h"
#include "GutHeightmap.h"
#include "GutTGA.h"
#include "GutDDS.h"

CGutHeightmap::CGutHeightmap(void)
{
	m_iArrayWidth = 0;
	m_iArrayHeight = 0;

	m_iNumVertices = 0;
	m_iNumIndices = 0;
	m_iNumFaces = 0;

	m_pHeightUINT8 = NULL;
	m_pVertexArray = NULL;
	m_pIndexArray = NULL;

	SetRange(Vector4(-50,-50,0), Vector4(50,50,10));
}

CGutHeightmap::~CGutHeightmap()
{
	Release();
}

void CGutHeightmap::Release(void)
{
	m_iArrayWidth = 0;
	m_iArrayHeight = 0;

	RELEASE_ARRAY(m_pHeightUINT8);
	RELEASE_ARRAY(m_pVertexArray);
	RELEASE_ARRAY(m_pIndexArray);
}

bool CGutHeightmap::LoadHeightmapTexture(const char *filename)
{
	char file_ext[16];
	_splitpath(filename, NULL, NULL, NULL, file_ext);
	strlwr(file_ext);

	// only loads tga for now
	if ( strcmp(file_ext, ".tga") )
	{
		return false;
	}

	TGAImg Loader;
	if ( !Loader.Load(filename) )
		return false;
	Loader.FlipImg();

	m_iArrayWidth = Loader.GetWidth();
	m_iArrayHeight = Loader.GetHeight();

	m_vGridSize[0] = m_vDiff[0]/(float)m_iArrayWidth;
	m_vGridSize[1] = m_vDiff[1]/(float)m_iArrayHeight;

	int bytes_per_pixel = Loader.GetBPP()/8;

	m_fmt = HEIGHTMAP_UINT8;

	m_pHeightUINT8 = new unsigned char[m_iArrayWidth * m_iArrayHeight];
	unsigned char *pImage = Loader.GetImg();

	for ( int y=0; y<m_iArrayHeight; y++ )
	{
		int base = y * m_iArrayWidth;
		int src_base = y * m_iArrayWidth * Loader.GetBPP();
		for ( int x=0; x<m_iArrayWidth; x++ )
		{
			m_pHeightUINT8[base+x] = pImage[0];
			pImage += bytes_per_pixel;
		}
	}

	return true;
}

sTerrainVertexProc CGutHeightmap::TerrainVertex(float tx, float ty)
{
	float x_expend = tx * m_iArrayWidth;
	float y_expend = ty * m_iArrayHeight;

	int px[5], py[5];

	// center
	px[0] = ((int) x_expend);
	py[0] = ((int) y_expend);
	// right
	px[1] = (px[0] + 1);
	py[1] =  py[0];
	// left
	px[2] = (px[0] - 1);
	py[2] =  py[0];
	// top
	px[3] =  px[0];
	py[3] = (py[0] + 1);
	// bottom
	px[4] =  px[0];
	py[4] = (py[0] - 1);

	int offset[5];
	int i;
	bool bWrap = false;

	for ( i=0; i<5; i++ )
	{
		if ( bWrap )
		{
			if ( px[i] < 0 ) px[i] += m_iArrayWidth;
			if ( px[i] >= m_iArrayWidth ) px[i] -= m_iArrayWidth;
			if ( py[i] < 0 ) py[i] += m_iArrayHeight;
			if ( py[i] >= m_iArrayHeight ) py[i] -= m_iArrayHeight;
		}
		else
		{
			// clamp
			if ( px[i] < 0 ) px[i] = 0;
			if ( px[i] >= m_iArrayWidth ) px[i] = m_iArrayWidth-1;
			if ( py[i] < 0 ) py[i] = 0;
			if ( py[i] >= m_iArrayHeight ) py[i] = m_iArrayHeight-1;
		}

		offset[i] = py[i] * m_iArrayWidth + px[i];
	}

	float fHeight[5];

	switch(m_fmt)
	{
	case HEIGHTMAP_UINT8:
		for (i=0; i<5; i++)
		{
			fHeight[i] = (float)m_pHeightUINT8[offset[i]] / 255.0f;	
		}
		break;
	case HEIGHTMAP_UINT16:
		for (i=0; i<5; i++)
		{
			fHeight[i] = (float)m_pHeightUINT16[offset[i]] / 65535.0f;	
		}
		break;
	case HEIGHTMAP_FLOAT32:
		for (i=0; i<5; i++)
		{
			fHeight[i] = (float)m_pHeightFLOAT32[offset[i]];	
		}
		break;
	}

	Vector4 vCoord(tx, ty, 0.0f, 0.0f);

	Vector4 vPos_Center = m_vMin + vCoord * m_vDiff;
	vPos_Center[2] = m_vMin[2] + fHeight[0] * m_vDiff[2];
	vPos_Center[3] = 1.0f;

	Vector4 vPos_Right = vPos_Center;
	vPos_Right[0] += m_vGridSize[0];
	vPos_Right[2] = m_vMin[2] + fHeight[1] * m_vDiff[2];

	Vector4 vPos_Left = vPos_Center;
	vPos_Left[0] -= m_vGridSize[0];
	vPos_Left[2] = m_vMin[2] + fHeight[2] * m_vDiff[2];

	Vector4 vPos_Top = vPos_Center;
	vPos_Top[1] += m_vGridSize[1];
	vPos_Top[2] = m_vMin[2] + fHeight[3] * m_vDiff[2];

	Vector4 vPos_Bottom = vPos_Center;
	vPos_Bottom[1] -= m_vGridSize[1];
	vPos_Bottom[2] = m_vMin[2] + fHeight[3] * m_vDiff[2];

	Vector4 vVec0 = vPos_Right - vPos_Left;
	Vector4 vVec1 = vPos_Top - vPos_Bottom;
	Vector4 vVec2 = Vector3CrossProduct(vVec0, vVec1);

	vVec2.Normalize();
	//vVec2 = (vVec2 + Vector4(1.0f)) * 0.5f;

	sTerrainVertexProc result;
	result.m_vPosition = vPos_Center;
	result.m_vNormal = vVec2;

	return result;
}

bool CGutHeightmap::BuildMesh(int x_grids, int y_grids)
{
	RELEASE_ARRAY(m_pVertexArray);
	RELEASE_ARRAY(m_pIndexArray);

	m_iNumFaces = 0;
	m_iNumVertices = 0;
	m_iNumIndices = 0;

	m_iMeshGridsX = x_grids;
	m_iMeshGridsY = y_grids;

	const int triangles_per_row = x_grids * 2;
	const int indices_per_row = triangles_per_row + 2;

	m_iNumVertices = (x_grids + 1) * (y_grids + 1);
	if ( m_iNumVertices > 65536 )
		return false;

	m_pVertexArray = new sTerrainVertex[m_iNumVertices];
	if ( m_pVertexArray==NULL )
		return false;

	m_iNumIndices = indices_per_row * y_grids;
	m_pIndexArray = new unsigned short[m_iNumIndices];
	if ( m_pIndexArray==NULL )
		return false;

	m_iNumFaces = m_iNumIndices-2;

	float x_adv = 1.0f/(float)(x_grids+1);
	float y_adv = 1.0f/(float)(y_grids+1);

	int vertex_index = 0;
	float fx, fy;
	int x,y;

	for ( y=0, fy=0; y<=y_grids; y++, fy+=y_adv)
	{
		for ( x=0, fx=0; x<=x_grids; x++, fx+=x_adv, vertex_index++)
		{
			sTerrainVertexProc v = TerrainVertex(fx, fy);

			v.m_vPosition.StoreXYZ(m_pVertexArray[vertex_index].m_Position);
			v.m_vNormal.StoreXYZ(m_pVertexArray[vertex_index].m_Normal);
		}
	}

	const int vertices_per_row = x_grids + 1;
	bool from_left_to_right = true;
	int  index_index = 0;

	for ( y=0; y<y_grids; y++ )
	{
		if ( from_left_to_right )
		{
			// 在奇數列的時候, 三角形從左排到右
			m_pIndexArray[index_index++] = y * vertices_per_row;
			m_pIndexArray[index_index++] = y * vertices_per_row + vertices_per_row;

			for ( x=0; x<x_grids; x++ )
			{
				vertex_index = y * vertices_per_row + x;
				m_pIndexArray[index_index++] = vertex_index + 1;
				m_pIndexArray[index_index++] = vertex_index + 1 + vertices_per_row;
			}
		}
		else
		{
			// 在偶數列的時候, 三角形從右排到左
			m_pIndexArray[index_index++] = y * vertices_per_row + x_grids;
			m_pIndexArray[index_index++] = (y+1) * vertices_per_row + x_grids;

			for ( x=x_grids; x>0; x-- )
			{
				vertex_index = y * vertices_per_row + x;
				m_pIndexArray[index_index++] = vertex_index - 1;
				m_pIndexArray[index_index++] = vertex_index - 1 + vertices_per_row;
			}
		}
		from_left_to_right = !from_left_to_right;
	}

	return true;
}
