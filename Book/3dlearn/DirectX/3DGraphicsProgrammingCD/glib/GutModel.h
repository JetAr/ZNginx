#ifndef _GUTMODEL_H_
#define _GUTMODEL_H_

#include "Gut.h"
#include "Vector4.h"
#include "Matrix4x4.h"

#define MAX_TEXTURE_NAME 128

#define SUBMIT_TEXTURES	0x01
#define SUBMIT_MTL		0x02
#define SUBMIT_SHADER	0x04
#define SUBMIT_CULLFACE	0x08
#define SUBMIT_BLEND	0x10
#define SUBMIT_FVF		0x20

#define SUBMIT_DEFAULT	0xffffffff

struct sModelMaterial
{
	Vector4 m_Emissive;
	Vector4 m_Ambient;
	Vector4 m_Diffuse;
	Vector4 m_Specular;
	float   m_fShininess;

	bool	m_bCullFace;
	char	m_szBlendMode[16];

	int		m_MapChannel[MAX_NUM_TEXTURES];
	char	m_szTexture[MAX_NUM_TEXTURES][MAX_TEXTURE_NAME];
	char	m_szTextureCombine[MAX_NUM_TEXTURES][16];

	sModelMaterial(void);
};

struct sModelVertex
{
	Vector4 m_Position;
	Vector4 m_Normal;
	Vector4 m_Color;
	Vector4 m_Texcoord[MAX_TEXCOORDS];
	Vector4 m_Tangent;
	Vector4 m_BiNormal;
};

struct sModelBatch
{
	int m_iMaterialID;
	int m_iNumPrimitives;
	int m_iIndexArrayBegin;
	int m_iIndexArrayEnd;
	int m_iNumIndices;
	int m_iNumVertices;
};

struct sModelVertexChunk
{
	int m_iNumPrimitives;
	int m_iNumVertices;
	int m_iNumIndices;
	int m_iNumBatches;

	sVertexDecl m_VertexDecl;

	sModelVertex *m_pVertexArray;
	unsigned short *m_pIndexArray;
	sModelBatch *m_pBatchArray;

	sModelVertexChunk(void)
	{
		m_iNumVertices = 0;
		m_iNumIndices = 0;
		m_iNumBatches = 0;

		m_pVertexArray = NULL;
		m_pIndexArray =NULL;
		m_pBatchArray = NULL;
	}

	~sModelVertexChunk()
	{
		if ( m_pVertexArray )
		{
			delete [] m_pVertexArray;
			m_pVertexArray = NULL;
		}

		if ( m_pIndexArray )
		{
			delete [] m_pIndexArray;
			m_pIndexArray = NULL;
		}
	}

	void OutputVertexBuffer(void *p);
};

struct sModelMesh
{
	Matrix4x4 m_Matrix;

	int m_iNumFaces, m_iNumVertices;
	// bounding box
	Vector4 m_vMin, m_vMax;

	int m_iNumVertexChunks;
	sModelVertexChunk *m_pVertexChunks;

	sModelMesh(void)
	{
		m_iNumVertexChunks = 0;
		m_pVertexChunks = NULL;

		m_Matrix.Identity();
	}

	~sModelMesh()
	{
		if ( m_pVertexChunks )
		{
			delete [] m_pVertexChunks;
			m_pVertexChunks = NULL;
		}
	}

	void Load_ASCII(FILE *file);
	void Load_BINARY(FILE *file);
};

class CGutModel
{
public:

	int m_iNumMaterials;
	int m_iNumMeshes;
	int m_iNumFaces;
	int m_iNumVertices;

	// bounding box
	Vector4 m_vMin;
	Vector4 m_vMax;
	Vector4 m_vSize;

	FILE *m_pFile;

	sModelMaterial *m_pMaterialArray;
	sModelMesh *m_pMeshArray;

	static char *m_pszTexturePath;

public:

	CGutModel(void);
	~CGutModel();
	//
	void Release(void);
	// ascii version
	bool LoadMaterial_ASCII(void);
	bool LoadMesh_ASCII(void);
	bool Load_ASCII(const char *filename);
	// binary version
	bool LoadMaterial_BINARY(void);
	bool LoadMesh_BINARY(void);
	bool Load_BINARY(const char *filename);
	// procedure mesh
	bool CreateSphere(float radius, sVertexDecl *pVertexDecl, int stacks=20, int slices=20);
	static void SetTexturePath(const char *path);
	static const char *GetTexturePath(void) { return m_pszTexturePath; }
	const sVertexDecl *GetVertexFormat(void);
};

#endif // _GUTMODEL_H_