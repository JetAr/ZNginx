#ifndef _GUTHEIGHTMAP_
#define _GUTHEIGHTMAP_

#include "Gut.h"
#include "Vector4.h"

struct sTerrainVertexProc
{
	Vector4 m_vPosition;
	Vector4 m_vNormal;
};

struct sTerrainVertex
{
	float m_Position[3];
	float m_Normal[3];
};

class CGutHeightmap
{
protected:

	int m_iArrayWidth, m_iArrayHeight;
	int m_iMeshGridsX, m_iMeshGridsY;

	int m_iNumVertices;
	int m_iNumIndices;
	int m_iNumFaces;

	union
	{
		unsigned char *m_pHeightUINT8;
		unsigned short *m_pHeightUINT16;
		unsigned float *m_pHeightFLOAT32;
	};

	enum HeightmapFMT 
	{
		HEIGHTMAP_UINT8,
		HEIGHTMAP_UINT16,
		HEIGHTMAP_FLOAT32
	};

	HeightmapFMT m_fmt;

	Vector4 m_vMin, m_vMax, m_vDiff;
	Vector4 m_vGridSize;

	//
	sTerrainVertex *m_pVertexArray;
	//
	unsigned short *m_pIndexArray;

public:
	CGutHeightmap(void);
	~CGutHeightmap();

	void SetRange(Vector4 &vMin, Vector4 &vMax) { m_vMin = vMin; m_vMax = vMax; m_vDiff = m_vMax - m_vMin; };
	bool LoadHeightmapTexture(const char *filename);
	sTerrainVertexProc TerrainVertex(float tx, float ty);

	virtual bool BuildMesh(int x_grids, int y_grids);
	virtual void Release(void);
	virtual void Render(void) {}
};

#endif //