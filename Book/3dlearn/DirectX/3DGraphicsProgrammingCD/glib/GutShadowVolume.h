#ifndef __GUT_SHADOW_VOLUME__

#include <vector>

#include "vector4.h"
#include "GutModel.h"

struct sEdge
{
	Vector4 *m_pA, *m_pB; // object space
	Vector4 *m_pTA, *m_pTB; // transformed
	Vector4 *m_pFaceA_V; // remaining vertex
	Vector4 *m_pFaceB_V; // remaining vertex

	int m_FaceA, m_FaceB;
	int m_bShadowVolumeEdge;

	sEdge(void)
	{
		m_FaceA = m_FaceB = -1;

		m_pA = m_pB = NULL;
		m_pTA = m_pTB = NULL;
		m_pFaceA_V = m_pFaceB_V = NULL;

		m_bShadowVolumeEdge = false;
	}
};

struct sFace
{
	Vector4 m_Normal_ObjectSpace;
	Vector4 m_Normal;

	Vector4 *m_pA, *m_pB, *m_pC; // object space
	Vector4 *m_pTA, *m_pTB, *m_pTC; // transformed

	UINT m_Edge0, m_Edge1, m_Edge2;

	bool m_bFrontFace;
};

class CGutShadowVolume
{
public:
	int m_iNumFaces;
	int m_iNumEdges;
	int m_iNumVertices;

	std::vector<sEdge> m_EdgeArray;
	sFace *m_pFaceArray;

	Vector4 *m_pVertexArray;
	Vector4 *m_pTransformedVertexArray;

	int m_iNumShadowVolumeFaces;
	Vector4 *m_pShadowVolume;

public:
	CGutShadowVolume(void);
	~CGutShadowVolume(void);

	void Release(void);
	UINT InsertEdge(sEdge &edge);
	int Preprocess(CGutModel &model);
	int Preprocess_Adj(void);
	int BuildShadowVolume(Matrix4x4 &light_view, Matrix4x4 &light_proj, float fExtent, bool bCaps=false);
	int BuildShadowVolume_DirectionalLight(Matrix4x4 &light_view, float fExtent, bool bCaps=false);
	int BuildShadowVolume_PointLight(Vector4 &light_pos, Matrix4x4 &world_matrix, float fExtent, bool bCaps=false);
};


#endif // __GUT_SHADOW_VOLUME__