#ifndef _GUTMODEL_DX10_H_
#define _GUTMODEL_DX10_H_

#include <d3d10.h>
#include "Gut.h"
#include "GutModel.h"

#define SHADER_FLAGS 5
#define SHADER_PERMUTATIONS (0x01<<5)

struct sModelVertexChunk_DX10
{
	int m_iNumVertices;
	int m_iNumIndices;

	ID3D10Buffer *m_pVertexBuffer;
	ID3D10Buffer *m_pIndexBuffer;

	ID3D10InputLayout *m_pVertexDecl[SHADER_PERMUTATIONS];
	UINT m_iVertexSize;

	int m_iNumBatches;
	sModelBatch *m_pBatchArray;

	sModelVertexChunk_DX10(void)
	{
		m_iNumVertices = 0;
		m_pVertexBuffer = NULL;

		m_iNumIndices = 0;
		m_pIndexBuffer = NULL;

		m_iNumBatches = 0;
		m_pBatchArray = NULL;

		m_iVertexSize = 0;

		for ( int i=0; i<SHADER_PERMUTATIONS; i++ )
		{
			m_pVertexDecl[i] = NULL;
		}
	}

	~sModelVertexChunk_DX10()
	{
		SAFE_RELEASE(m_pVertexBuffer);
		SAFE_RELEASE(m_pIndexBuffer);

		for ( int i=0; i<SHADER_PERMUTATIONS; i++ )
		{
			SAFE_RELEASE(m_pVertexDecl[i]);
		}

		if ( m_pBatchArray )
		{
			delete [] m_pBatchArray;
			m_pBatchArray = NULL;
		}
	}

	void CreateVertexDecl(sVertexDecl *pVertexDecl);
};

struct sModelMesh_DX10
{
	int m_iNumVertexChunks;
	sModelVertexChunk_DX10 *m_pVertexChunk;

	sModelMesh_DX10(void)
	{
		m_iNumVertexChunks = 0;
		m_pVertexChunk = NULL;
	}

	~sModelMesh_DX10()
	{
		if ( m_pVertexChunk )
		{
			delete [] m_pVertexChunk;
			m_pVertexChunk = NULL;
		}
	}
};

// simulate fixed pipeline
struct sModelMaterial_DX10
{
public:
	ID3D10BlendState		*m_pBlendState;
	ID3D10ShaderResourceView *m_pTextures[4];

	//ID3D10SamplerState		*m_pSamplerState[4];

	Vector4 m_vAmbient;
	Vector4 m_vDiffuse;
	Vector4 m_vSpecular;
	Vector4 m_vEmissive;
	float m_fPower;

	int	  m_iNumTextures;
	bool  m_bCullFace;

public:
	sModelMaterial_DX10(void)
	{
		m_iNumTextures = 0;
		m_pBlendState = NULL;

		for ( int i=0; i<4; i++ )
		{
			// m_pSamplerState[i] = NULL;
			m_pTextures[i] = NULL;
		}

		Vector4 vOne(1.0f, 1.0f, 1.0f, 1.0f);
		Vector4 vZero(0.0f, 0.0f, 0.0f, 0.0f);

		m_vAmbient = vZero;
		m_vDiffuse = vOne;
		m_vSpecular = vZero;
		m_vEmissive = vZero;
		m_fPower = 1.0f;

		m_bCullFace = true;
	}

	void Submit(ID3D10InputLayout *pLayout[], UINT mask=0xffffffff);

	void sModelMaterial_DX10::Release(void)
	{
		//SAFE_RELEASE(m_pBlendState);

		for ( int i=0; i<4; i++ )
		{
			//SAFE_RELEASE(m_pSamplerState[i]);
			SAFE_RELEASE(m_pTextures[i]);
		}
	}

};

struct sMatrixConstants
{
	// transform matrix
	Matrix4x4 wvp_matrix;
	Matrix4x4 view_matrix;
	Matrix4x4 world_matrix;
	Matrix4x4 inv_view_matrix;
	Matrix4x4 normal_matrix;
	Matrix4x4 texture_matrix[4];
	Matrix4x4 light_wvp_matrix;
	Matrix4x4 light_wv_matrix;
};

struct sMaterialConstants
{
	Vector4 vEmissive;
	Vector4 vAmbient;
	Vector4 vDiffuse;
	Vector4 vSpecular;
	Vector4 vPower;
};

struct sLightConstants
{
	Vector4 m_vGlobalAmbientLight;
	sGutLight m_Lights[LIGHT_MAX_LIGHTS];
	sLightConstants(void)
	{
		m_vGlobalAmbientLight.Set(0.0f);
	}
};

class CGutModel_DX10
{
public:
	int m_iNumMeshes;
	sModelMesh_DX10 *m_pMeshArray;

	int m_iNumMaterials;
	// fixed pipeline material
	sModelMaterial_DX10 *m_pMaterialArray;

	bool m_FrontFaceCCW;

	// fixed pipeline simulation shader
	static ID3D10VertexShader	*s_pVertexShader[SHADER_PERMUTATIONS];
	static ID3D10PixelShader	*s_pPixelShader[SHADER_PERMUTATIONS];
	static ID3D10Blob			*s_pVertexShaderCode[SHADER_PERMUTATIONS];
	static ID3D10Buffer			*s_pMatrixConstantBuffer;
	static ID3D10Buffer			*s_pMtlConstantBuffer;
	static ID3D10Buffer			*s_pLightConstantBuffer;
	// blend state
	static ID3D10BlendState		*s_pBlend_Replace;
	static ID3D10BlendState		*s_pBlend_Add;
	static ID3D10BlendState		*s_pBlend_Blend;
	//
	static ID3D10RasterizerState *s_pCull_CCW, *s_pCull_CW, *s_pCull_NONE;
	static ID3D10RasterizerState *s_pCull, *s_pNoCull;
	// fixed pipeline matrix
	static Matrix4x4 s_proj_matrix;
	static Matrix4x4 s_view_matrix;
	static Matrix4x4 s_inv_view_matrix;
	static Matrix4x4 s_world_matrix;
	static Matrix4x4 s_wvp_matrix;
	static Matrix4x4 s_texture_matrix[4];
	static Matrix4x4 s_light_wvp_matrix;
	static Matrix4x4 s_light_wv_matrix;
	//
	static bool s_bLighting;
	static sLightConstants s_light; 
	//
	// overwrite material settings
	//
	static int					s_shader_overwrite;
	// textures
	static ID3D10ShaderResourceView *s_pTextureOverwrite[3];
	//
	static sModelMaterial_DX10  *s_pMaterialOverwrite;
	//
	static bool s_bCullCW;
public:
	CGutModel_DX10(void)
	{
		m_iNumMeshes = 0;
		//m_iVertexSize = 0;
		m_pMeshArray = NULL;

		m_iNumMaterials = 0;
		m_pMaterialArray = NULL;
		m_FrontFaceCCW = false;
	}

	~CGutModel_DX10()
	{
		Release();
	}

	void Release(void);
	static void EnableLighting(bool bLighting) { s_bLighting = bLighting; }
	bool ConvertToDX10Model(CGutModel *pModel, void (*CopyVertexBuffer)(void *target, sModelVertex *source, int num_vertices) );
	bool ConvertToDX10Model(CGutModel *pModel);
	void RenderMesh(bool submitmaterial = true);
	void Render(UINT mask = 0xffffffff);
	void RenderInstance(int num_instances, int instance_start = 0, int mask = 0xffffffff);
	// load fixed pipeline simulate shader
	static bool LoadDefaultShader(const char *filename = NULL);
	static void ReleaseDefaultShader(void);
	static bool SetDefaultShader(ID3D10VertexShader *pVS[], ID3D10Blob *pCode[], ID3D10PixelShader *pPS[], int shaders);
	static bool SetDefaultShader(ID3D10VertexShader *pVS, ID3D10Blob *pCode, ID3D10PixelShader *pPS);
	static bool CreateSharedResource(void);
	// function to simulatio fixed pipeline
	static void SetProjectionMatrix(Matrix4x4 &matrix) { s_proj_matrix = matrix; }
	static void SetViewMatrix(Matrix4x4 &matrix) { s_inv_view_matrix = s_view_matrix = matrix; s_inv_view_matrix.FastInvert(); }
	static void SetInvViewMatrix(Matrix4x4 &matrix) { s_inv_view_matrix = matrix; }
	static void SetWorldMatrix(Matrix4x4 &matrix) { s_world_matrix = matrix; }
	static void SetTexcoordMatrix(int index, Matrix4x4 &matrix) { s_texture_matrix[index] = matrix; }
	static void SetLightWVPMatrix(Matrix4x4 &matrix) { s_light_wvp_matrix = matrix; }
	static void SetLightWVMatrix(Matrix4x4 &matrix) { s_light_wv_matrix = matrix; }
	static void UpdateMatrix(void) { s_wvp_matrix = s_world_matrix * s_view_matrix * s_proj_matrix; }
	static void SetLight(int index, sGutLight &light) { s_light.m_Lights[index] = light; }
	// culling state object
	static void SetRasterizeState(ID3D10RasterizerState *pCull, ID3D10RasterizerState *pNoCull) { s_pCull = pCull; s_pNoCull = pNoCull; }
	// material overwrite
	static void SetTextureOverwrite(int index, ID3D10ShaderResourceView *pTexture) { s_pTextureOverwrite[index] = pTexture; }
	static void SetShaderOverwrite(int shader) { s_shader_overwrite = shader; }
	static void SetMaterialOverwrite(sModelMaterial_DX10 *pMtl) { s_pMaterialOverwrite = pMtl; }
	static void SetCullCW(bool bCullCW) { s_bCullCW = bCullCW; }
};

void GutSetDX10BackFace(bool ccw);

#endif // _GUTMODEL_DX10_H_