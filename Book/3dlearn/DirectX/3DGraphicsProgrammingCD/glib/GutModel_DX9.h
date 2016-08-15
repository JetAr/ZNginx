#ifndef _GUTMODEL_DX9_H_
#define _GUTMODEL_DX9_H_

#include <d3d9.h>
#include "Gut.h"
#include "GutModel.h"

struct sModelVertexChunk_DX9
{
	int m_iNumVertices;
	int m_iNumIndices;

	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;
	LPDIRECT3DINDEXBUFFER9 m_pIndexBuffer;
	LPDIRECT3DVERTEXDECLARATION9 m_pVertexDecl;
	DWORD m_FVF;
	int m_iVertexSize;

	int m_iNumBatches;
	sModelBatch *m_pBatchArray;

	sModelVertexChunk_DX9(void)
	{
		m_iNumVertices = 0;
		m_pVertexBuffer = NULL;

		m_iNumIndices = 0;
		m_pIndexBuffer = NULL;

		m_iNumBatches = 0;
		m_pBatchArray = NULL;

		m_iVertexSize = 0;
		m_FVF = 0;
		m_pVertexDecl = NULL;
	}

	~sModelVertexChunk_DX9()
	{
		SAFE_RELEASE(m_pVertexBuffer);
		SAFE_RELEASE(m_pIndexBuffer);
		SAFE_RELEASE(m_pVertexDecl);

		if ( m_pBatchArray )
		{
			delete [] m_pBatchArray;
			m_pBatchArray = NULL;
		}
	}

	LPDIRECT3DVERTEXDECLARATION9 CreateVertexDecl(sVertexDecl *pVertexDecl);
	DWORD CreateFVF(sVertexDecl *pVertexDecl);
};

struct sModelMesh_DX9
{
	int m_iNumVertexChunks;
	sModelVertexChunk_DX9 *m_pVertexChunk;

	sModelMesh_DX9(void)
	{
		m_iNumVertexChunks = 0;
		m_pVertexChunk = NULL;
	}

	~sModelMesh_DX9()
	{
		if ( m_pVertexChunk )
		{
			delete [] m_pVertexChunk;
			m_pVertexChunk = NULL;
		}
	}
};

struct sModelMaterial_DX9
{
	D3DMATERIAL9		m_Material;
	DWORD				m_bBlend;
	DWORD				m_SrcBlend;
	DWORD				m_DestBlend;
	bool				m_bCullFace;
	int					m_MapChannel[4];

	LPDIRECT3DPIXELSHADER9 m_pPixelShader;
	LPDIRECT3DVERTEXSHADER9 m_pVertexShader;
	//LPDIRECT3DTEXTURE9	m_pTextures[4];
	LPDIRECT3DBASETEXTURE9	m_pTextures[4];

	void Submit(int mask = 0xffffffff);

	sModelMaterial_DX9(void)
	{
		Vector4 vOne(1.0f);
		Vector4 vZero(0.0f);

		m_Material.Ambient = *(D3DCOLORVALUE *) &vZero;
		m_Material.Diffuse = *(D3DCOLORVALUE *) &vOne;
		m_Material.Specular = *(D3DCOLORVALUE *) &vZero;
		m_Material.Emissive = *(D3DCOLORVALUE *) &vZero;
		m_Material.Power = 1.0f;

		m_bBlend = FALSE;
		m_SrcBlend = D3DBLEND_ONE;
		m_DestBlend = D3DBLEND_ZERO;
		m_bCullFace = true;

		m_pPixelShader = NULL;
		m_pVertexShader = NULL;

		for ( int i=0; i<4; i++ )
		{
			m_MapChannel[i] = 0;
			m_pTextures[i] = NULL;
		}
	}

	void sModelMaterial_DX9::Release(void)
	{
		for ( int i=0; i<4; i++ )
		{
			SAFE_RELEASE(m_pTextures[i]);
		}
	}

};

class CGutModel_DX9
{
public:
	//
	int m_iNumMeshes;
	sModelMesh_DX9 *m_pMeshArray;
	//
	int m_iNumMaterials;
	sModelMaterial_DX9 *m_pMaterialArray;
	// shader material
	static DWORD s_CullFace;
	static int s_MapOverwrite[3];
	static sModelMaterial_DX9 *s_pMaterialOverwrite;
	static LPDIRECT3DBASETEXTURE9  s_pTextureOverwrite[3];
	static LPDIRECT3DVERTEXSHADER9 s_pVertexShaderOverwrite;
	static LPDIRECT3DPIXELSHADER9  s_pPixelShaderOverwrite;

public:
	CGutModel_DX9(void)
	{
		m_iNumMeshes = 0;
		m_pMeshArray = NULL;

		m_iNumMaterials = 0;
		m_pMaterialArray = NULL;
	}

	~CGutModel_DX9()
	{
		Release();
	}

	void Release(void);
	bool ConvertToDX9Model(CGutModel *pModel);
	void RenderMesh(bool submitmaterial = true);
	void Render(int mask = 0xffffffff);
	//
	static void SetCullMode(DWORD mode) { s_CullFace = mode; }
	static void SetTextureOverwrite(int index, LPDIRECT3DBASETEXTURE9 pTex) { s_pTextureOverwrite[index] = pTex; }
	static void SetUVMapOverwrite(int index, int map) { s_MapOverwrite[index] = map; } 
	static void SetMaterialOverwrite(sModelMaterial_DX9 *pMtl) { s_pMaterialOverwrite = pMtl; }
	static void SetShaderOverwrite(LPDIRECT3DVERTEXSHADER9 pVS, LPDIRECT3DPIXELSHADER9 pPS) { s_pVertexShaderOverwrite=pVS; s_pPixelShaderOverwrite=pPS; }
};

void GutSetDX9BackFace(DWORD cullface);

#endif // _GUTMODEL_DX9_H_