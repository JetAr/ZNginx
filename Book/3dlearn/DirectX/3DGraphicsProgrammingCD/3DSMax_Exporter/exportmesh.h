#ifndef _EXPORTMESH_
#define _EXPORTMESH_

#define MAX_UVCHANNLES 8

#include <vector>
#include <map>

class GmodelExp;

struct sMeshVertex
{
	// speedup searching
	UINT32 m_Checksum;
	// position, color, normal
	Point3 m_Position;
	Point4 m_Color;
	Point3 m_Normal;
	// texcoords
	Point3 m_UV[MAX_UVCHANNLES];
	// for normalmap per-pixel lighting
	Point3 m_Tangent;
	Point3 m_BiNormal;

	sMeshVertex(void)
	{
		ZeroMemory(this, sizeof(sMeshVertex));
	}
};

struct sMeshFace
{
	int m_iVertexIndex[3];
	int m_iArrayIndex;

	sMeshFace(void)
	{
		m_iArrayIndex = 0;
		m_iVertexIndex[0] = m_iVertexIndex[1] = m_iVertexIndex[2] = -1;
	}
};

// faces belong to the same material
struct sMeshBatch
{
	int m_iMaterialIndex;

	int m_iFaceArrayBegin;

	int m_iIndexArrayBegin_TriList;
	int m_iIndexArrayUsed_TriList;

	int m_iIndexArrayBegin_TriStrip;
	int m_iIndexArrayUsed_TriStrip;

	int m_iNumPrimitives_TriList;
	int m_iNumPrimitives_TriStrip;

	//std::vector<sMeshFace>	m_FaceArray;

	sMeshBatch(void)
	{
		m_iMaterialIndex = -1;

		m_iFaceArrayBegin = 0;

		m_iIndexArrayBegin_TriList = 0;
		m_iIndexArrayUsed_TriList = 0;

		m_iIndexArrayBegin_TriStrip = 0;
		m_iIndexArrayUsed_TriStrip = 0;

		m_iNumPrimitives_TriList = 0;
		m_iNumPrimitives_TriStrip = 0;
	}
};

struct sMeshVertexArray
{
	std::vector<sMeshVertex> m_VertexArray;
	std::vector<sMeshBatch> m_BatchList;
	std::vector<unsigned short> m_IndexArray_TriList;
	std::vector<unsigned short> m_IndexArray_TriStrip;

	int AddVertex_LinearSearch(sMeshVertex &vertex);
	int AddVertex_HashTable(sMeshVertex &vertex);
	int AddVertex(sMeshVertex &vertex);
};

class CMeshBuilder
{
public:
	// no more than 65536 vertexes for each vertex array
	// need multi-vertex arrays for huge model
	int m_iNumUVs;
	int m_iNumVertexArrays;
	std::vector<sMeshVertexArray> m_VertexArray;
	Matrix3 m_Matrix;

public:
	CMeshBuilder(void);
	~CMeshBuilder() {}

	bool BuildMesh_v0(INode *pNode, GmodelExp *pExporter);
	bool BuildMesh(INode *pNode, GmodelExp *pExporter);
	Point3 GetVertexNormal(Mesh &mesh, int findex, int vindex);
};

#endif // _EXPORTMESH_