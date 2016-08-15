#include "Gut.h"
#include "GutModel.h"
#include "GutShadowVolume.h"

CGutShadowVolume::CGutShadowVolume(void)
{
	m_iNumEdges = 0;
	m_iNumFaces = 0;
	m_iNumVertices = 0;
	m_iNumShadowVolumeFaces = 0;

	m_pFaceArray = NULL;
	m_pVertexArray = NULL;
	m_pTransformedVertexArray = NULL;
	m_pShadowVolume = NULL;
}

CGutShadowVolume::~CGutShadowVolume(void)
{
	Release();
}

void CGutShadowVolume::Release(void)
{
	m_iNumEdges = 0;
	m_EdgeArray.clear();

	m_iNumFaces = 0;
	RELEASE_ARRAY(m_pFaceArray);

	m_iNumVertices = 0;
	RELEASE_ARRAY(m_pVertexArray);
	RELEASE_ARRAY(m_pTransformedVertexArray);

	m_iNumShadowVolumeFaces = 0;
	RELEASE_ARRAY(m_pShadowVolume);
}

inline UINT DecodeEdgeIndex(int index)
{
	UINT decoded = index & 0x7fffffff;
	return decoded;
}

UINT CGutShadowVolume::InsertEdge(sEdge &edge)
{
	for ( UINT i=0; i<m_iNumEdges; i++ )
	{
		if ( m_EdgeArray[i].m_pA==edge.m_pB && m_EdgeArray[i].m_pB==edge.m_pA )
		{
			if ( m_EdgeArray[i].m_FaceB==-1 )
			{
				m_EdgeArray[i].m_FaceB = edge.m_FaceA;
				m_EdgeArray[i].m_pFaceB_V = edge.m_pFaceA_V;
			}
			UINT index = i | 0x80000000;
			return index;
		}
		else if ( m_EdgeArray[i].m_pA==edge.m_pA && m_EdgeArray[i].m_pB==edge.m_pB )
		{
			assert(0);
		}
	}

	m_EdgeArray.push_back(edge);
	m_iNumEdges = m_EdgeArray.size();

	return m_iNumEdges - 1;
}

int CGutShadowVolume::Preprocess(CGutModel &model)
{
	Release();

	m_iNumFaces = model.m_iNumFaces;
	m_pFaceArray = new sFace[m_iNumFaces];

	m_iNumVertices = model.m_iNumVertices;
	m_pVertexArray = new Vector4[m_iNumVertices];
	m_pTransformedVertexArray = new Vector4[m_iNumVertices];

	int vbase = 0;
	int findex = 0;
	int vindex = 0;

	for ( int m=0; m<model.m_iNumMeshes; m++ )
	{
		sModelMesh *pMesh = model.m_pMeshArray + m;

		for ( int vc=0; vc<pMesh->m_iNumVertexChunks; vc++ )
		{
			sModelVertexChunk *pVertexChunk = pMesh->m_pVertexChunks + vc;
			sModelVertex *pVertexArray = pVertexChunk->m_pVertexArray;

			for ( int v=0; v<pVertexChunk->m_iNumVertices; v++ )
			{
				m_pVertexArray[vindex++] = pVertexArray[v].m_Position;	
			}

			Vector4 *pTransformedVertices = m_pTransformedVertexArray + vbase;

			for ( int m=0; m<pVertexChunk->m_iNumBatches; m++ )
			{
				sModelBatch *pBatch = pVertexChunk->m_pBatchArray + m;

				for ( int f=0; f<pBatch->m_iNumIndices; f+=3 )
				{
					unsigned short *vindex = pVertexChunk->m_pIndexArray + f;
					int a,b,c;
					sEdge edge;

					edge.m_FaceA = findex;

					a = vindex[0];
					b = vindex[1];
					c = vindex[2];

					edge.m_pA = &pVertexArray[a].m_Position;
					edge.m_pB = &pVertexArray[b].m_Position;
					edge.m_pFaceA_V = &pVertexArray[c].m_Position;

					edge.m_pTA = pTransformedVertices + a;
					edge.m_pTB = pTransformedVertices + b;

					UINT e0 = InsertEdge(edge);

					a = vindex[1];
					b = vindex[2];
					c = vindex[0];

					edge.m_pA = &pVertexArray[a].m_Position;
					edge.m_pB = &pVertexArray[b].m_Position;
					edge.m_pFaceA_V = &pVertexArray[c].m_Position;

					edge.m_pTA = pTransformedVertices + a;
					edge.m_pTB = pTransformedVertices + b;

					UINT e1 = InsertEdge(edge);

					a = vindex[2];
					b = vindex[0];
					c = vindex[1];

					edge.m_pA = &pVertexArray[a].m_Position;
					edge.m_pB = &pVertexArray[b].m_Position;
					edge.m_pFaceA_V = &pVertexArray[c].m_Position;

					edge.m_pTA = pTransformedVertices + a;
					edge.m_pTB = pTransformedVertices + b;

					UINT e2 = InsertEdge(edge);

					Vector4 v[3];
					v[0] = pVertexArray[ vindex[0] ].m_Position;
					v[1] = pVertexArray[ vindex[1] ].m_Position;
					v[2] = pVertexArray[ vindex[2] ].m_Position;

					Vector4 vDiff0 = v[1] - v[0];
					Vector4 vDiff1 = v[2] - v[0];
					Vector4 vNormal = Vector3CrossProduct(vDiff0, vDiff1);
					vNormal.Normalize();

					// object space;
					m_pFaceArray[findex].m_Normal_ObjectSpace = vNormal;

					m_pFaceArray[findex].m_Edge0 = e0;
					m_pFaceArray[findex].m_Edge1 = e1;
					m_pFaceArray[findex].m_Edge2 = e2;

					m_pFaceArray[findex].m_pA = &pVertexArray[ vindex[0] ].m_Position;
					m_pFaceArray[findex].m_pB = &pVertexArray[ vindex[1] ].m_Position;
					m_pFaceArray[findex].m_pC = &pVertexArray[ vindex[2] ].m_Position;

					m_pFaceArray[findex].m_pTA = pTransformedVertices + vindex[0];
					m_pFaceArray[findex].m_pTB = pTransformedVertices + vindex[1];
					m_pFaceArray[findex].m_pTC = pTransformedVertices + vindex[2];

					findex++;
				}
			}
			vbase += pVertexChunk->m_iNumVertices;
		}
	}

	// verify
	for ( int e=0; e<m_iNumEdges; e++ )
	{
		sEdge *pEdge = &m_EdgeArray[e];
		if ( pEdge->m_FaceA==-1 || pEdge->m_FaceB==-1 )
		{
			printf("Error\n");
			return 0;
		}
	}

	return 1;
}

int CGutShadowVolume::Preprocess_Adj(void)
{
	RELEASE_ARRAY(m_pShadowVolume);

	m_pShadowVolume = new Vector4[m_iNumFaces*6];
	Vector4 *pVertex = m_pShadowVolume;

	int edge_size = m_EdgeArray.size();

	for (int f=0; f<m_iNumFaces; f++ )
	{
		sFace *pFace = m_pFaceArray + f;

		UINT edge0 = DecodeEdgeIndex(pFace->m_Edge0);
		UINT edge1 = DecodeEdgeIndex(pFace->m_Edge1);
		UINT edge2 = DecodeEdgeIndex(pFace->m_Edge2);

		sEdge *pEdge0 = &m_EdgeArray[edge0];
		sEdge *pEdge1 = &m_EdgeArray[edge1];
		sEdge *pEdge2 = &m_EdgeArray[edge2];

		pVertex[0] = *pFace->m_pA;
		pVertex[2] = *pFace->m_pB;
		pVertex[4] = *pFace->m_pC;

		/*
		pVertex[1] = pFace->m_Edge0==edge0 ? *pEdge0->m_pFaceA_V : *pEdge0->m_pFaceB_V;
		pVertex[3] = pFace->m_Edge1==edge1 ? *pEdge1->m_pFaceA_V : *pEdge1->m_pFaceB_V;
		pVertex[5] = pFace->m_Edge2==edge2 ? *pEdge2->m_pFaceA_V : *pEdge2->m_pFaceB_V;
		*/

		pVertex[1] = pFace->m_Edge0==edge0 ? *pEdge0->m_pFaceB_V : *pEdge0->m_pFaceA_V;
		pVertex[3] = pFace->m_Edge1==edge1 ? *pEdge1->m_pFaceB_V : *pEdge1->m_pFaceA_V;
		pVertex[5] = pFace->m_Edge2==edge2 ? *pEdge2->m_pFaceB_V : *pEdge2->m_pFaceA_V;

		pVertex += 6;
	}

	return 1;
}

int CGutShadowVolume::BuildShadowVolume(Matrix4x4 &light_view, Matrix4x4 &light_proj, float fExtent, bool bCaps)
{
	Matrix4x4 light_viewproj = light_view * light_proj;
	bool bPerspective = light_proj[3][3]==0.0f ? true : false;
	int shadowvolume_quads = 0;

	//
	for ( int v=0; v<m_iNumVertices; v++ )
	{
		m_pTransformedVertexArray[v] = m_pVertexArray[v] * light_viewproj;
		m_pTransformedVertexArray[v] /= m_pTransformedVertexArray[v].GetW();
	}

	//
	for ( int f=0; f<m_iNumFaces; f++ )
	{
		Vector4 v0 = *m_pFaceArray[f].m_pTA;
		Vector4 v1 = *m_pFaceArray[f].m_pTB;
		Vector4 v2 = *m_pFaceArray[f].m_pTC;
		Vector4 vDiff1 = v1 - v0;
		Vector4 vDiff2 = v2 - v0;
		m_pFaceArray[f].m_Normal = Vector3CrossProduct(vDiff1, vDiff2);
	}

	// 找出邊線
	for ( int e=0; e<m_iNumEdges; e++ )
	{
		sEdge *pEdge = &m_EdgeArray[e];

		int faceA = DecodeEdgeIndex(pEdge->m_FaceA);
		int faceB = DecodeEdgeIndex(pEdge->m_FaceB);

		Vector4 vDotMul = m_pFaceArray[faceA].m_Normal * m_pFaceArray[faceB].m_Normal;

		if ( vDotMul.GetZ() < 0.0f )
		{
			pEdge->m_bShadowVolumeEdge = true;
			shadowvolume_quads++;
		}
		else if ( vDotMul.GetZ()==0.0f && (
			m_pFaceArray[faceA].m_Normal.GetZ()<0.0f || 
			m_pFaceArray[faceB].m_Normal.GetZ()<0.0f )
			)
		{
			pEdge->m_bShadowVolumeEdge = true;
			shadowvolume_quads++;
		}
		else
		{
			pEdge->m_bShadowVolumeEdge = false;
		}
	}

	m_iNumShadowVolumeFaces = shadowvolume_quads * 2;

	if ( bCaps )
	{
		m_iNumShadowVolumeFaces += m_iNumFaces;
	}

	for ( int v=0; v<m_iNumVertices; v++ )
	{
		m_pTransformedVertexArray[v] = m_pVertexArray[v] * light_view;
	}

	int AllocatedFaces = m_iNumShadowVolumeFaces * 3;
	RELEASE_ARRAY(m_pShadowVolume);
	m_pShadowVolume = new Vector4[AllocatedFaces];

	int vindex = 0;
	m_iNumShadowVolumeFaces = 0;

	Vector4 light_dir(0.0f, 0.0f, -1.0f);
	Vector4 vExtentA = light_dir * fExtent;
	Vector4 vExtentB = light_dir * fExtent;
	Vector4 vExtentC = light_dir * fExtent;

	for ( int e=0; e<m_iNumEdges; e++ )
	{
		sEdge *pEdge = &m_EdgeArray[e];

		if ( pEdge->m_bShadowVolumeEdge )
		{
			Vector4 A = *pEdge->m_pTA;
			Vector4 B = *pEdge->m_pTB;

			int faceA = DecodeEdgeIndex(pEdge->m_FaceA);
			int faceB = DecodeEdgeIndex(pEdge->m_FaceB);

			if ( bPerspective )
			{
				vExtentA = VectorNormalize(A) * fExtent;
				vExtentB = VectorNormalize(B) * fExtent;
			}

			if ( m_iNumShadowVolumeFaces+2 >= AllocatedFaces )
			{
				printf("Run out of buffer\n");
				return false;
			}

			if ( m_pFaceArray[faceA].m_Normal.GetZ()<0.0f )
			{
				m_pShadowVolume[vindex++] = A;
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A + vExtentA; 
				m_iNumShadowVolumeFaces++;

				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = B + vExtentB;
				m_pShadowVolume[vindex++] = A + vExtentA;
				m_iNumShadowVolumeFaces++;
			}
			else
			{
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A;
				m_pShadowVolume[vindex++] = A + vExtentA; 
				m_iNumShadowVolumeFaces++;

				m_pShadowVolume[vindex++] = B + vExtentB;
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A + vExtentA;
				m_iNumShadowVolumeFaces++;
			}
		}
	}

	// 使用zfail時,要把整個模型前後加上蓋子
	if ( bCaps )
	{
		for ( int f=0; f<m_iNumFaces; f++ )
		{
			sFace *pFace = m_pFaceArray + f;

			if ( m_iNumShadowVolumeFaces >= AllocatedFaces )
			{
				printf("Run out of buffer\n");
				return false;
			}

			Vector4 vA = *pFace->m_pTA;
			Vector4 vB = *pFace->m_pTB;
			Vector4 vC = *pFace->m_pTC;

			if ( bPerspective )
			{
				vExtentA = VectorNormalize(vA) * fExtent;
				vExtentB = VectorNormalize(vB) * fExtent;
				vExtentC = VectorNormalize(vC) * fExtent;
			}

			if ( pFace->m_bFrontFace )
			{
				m_pShadowVolume[vindex++] = vA;
				m_pShadowVolume[vindex++] = vB;
				m_pShadowVolume[vindex++] = vC;
			}
			else
			{
				m_pShadowVolume[vindex++] = vA + vExtentA;
				m_pShadowVolume[vindex++] = vB + vExtentB;
				m_pShadowVolume[vindex++] = vC + vExtentC;
			}

			m_iNumShadowVolumeFaces++;
		}
	}

	return m_iNumShadowVolumeFaces;
}

int CGutShadowVolume::BuildShadowVolume_DirectionalLight(Matrix4x4 &light_view, float fExtent, bool bCaps)
{
	int shadowvolume_quads = 0;
	Vector4 light_dir(0.0f, 0.0f, -1.0f);

	// 把3角形的normal轉換到light view space
	for ( int f=0; f<m_iNumFaces; f++ )
	{
		sFace *pFace = m_pFaceArray + f;
		pFace->m_Normal = light_view.RotateVector(pFace->m_Normal_ObjectSpace);
		pFace->m_bFrontFace = pFace->m_Normal.GetZ() >= 0.0f;
	}

	// 找出邊線
	for ( int e=0; e<m_iNumEdges; e++ )
	{
		sEdge *pEdge = &m_EdgeArray[e];

		int faceA = DecodeEdgeIndex(pEdge->m_FaceA);
		int faceB = DecodeEdgeIndex(pEdge->m_FaceB);

		Vector4 vDotMul = m_pFaceArray[faceA].m_Normal * m_pFaceArray[faceB].m_Normal;

		if ( vDotMul.GetZ() < 0.0f )
		{
			pEdge->m_bShadowVolumeEdge = true;
			shadowvolume_quads++;
		}
		else if ( vDotMul.GetZ()==0.0f && (
			m_pFaceArray[faceA].m_Normal.GetZ()<0.0f || 
			m_pFaceArray[faceB].m_Normal.GetZ()<0.0f )
			)
		{
			pEdge->m_bShadowVolumeEdge = true;
			shadowvolume_quads++;
		}
		else
		{
			pEdge->m_bShadowVolumeEdge = false;
		}
	}

	m_iNumShadowVolumeFaces = shadowvolume_quads * 2;

	if ( bCaps )
	{
		m_iNumShadowVolumeFaces += m_iNumFaces;
	}

	if ( m_pShadowVolume )
	{
		delete [] m_pShadowVolume;
	}

	m_pShadowVolume = new Vector4[m_iNumShadowVolumeFaces*3];

	int vindex = 0;
	m_iNumShadowVolumeFaces = 0;
	Vector4 vExtent = light_dir * fExtent;

	for ( int e=0; e<m_iNumEdges; e++ )
	{
		sEdge *pEdge = &m_EdgeArray[e];
		if ( pEdge->m_bShadowVolumeEdge )
		{
			Vector4 A = *pEdge->m_pA * light_view;
			Vector4 B = *pEdge->m_pB * light_view;

			int faceA = DecodeEdgeIndex(pEdge->m_FaceA);
			int faceB = DecodeEdgeIndex(pEdge->m_FaceB);

			if ( m_pFaceArray[faceA].m_Normal.GetZ()<0.0f )
			{
				m_pShadowVolume[vindex++] = A;
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A + vExtent; 
				m_iNumShadowVolumeFaces++;

				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = B + vExtent;
				m_pShadowVolume[vindex++] = A + vExtent;
				m_iNumShadowVolumeFaces++;
			}
			else
			{
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A;
				m_pShadowVolume[vindex++] = A + vExtent; 
				m_iNumShadowVolumeFaces++;

				m_pShadowVolume[vindex++] = B + vExtent;
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A + vExtent;
				m_iNumShadowVolumeFaces++;
			}
		}
	}

	// 使用zfail時,要把整個模型前後加上蓋子
	if ( bCaps )
	{
		for ( int f=0; f<m_iNumFaces; f++ )
		{
			sFace *pFace = m_pFaceArray + f;

			Vector4 vA = (*pFace->m_pA) * light_view;
			Vector4 vB = (*pFace->m_pB) * light_view;
			Vector4 vC = (*pFace->m_pC) * light_view;

			if ( pFace->m_bFrontFace )
			{
				m_pShadowVolume[vindex++] = vA;
				m_pShadowVolume[vindex++] = vB;
				m_pShadowVolume[vindex++] = vC;
			}
			else
			{
				m_pShadowVolume[vindex++] = vA + vExtent;
				m_pShadowVolume[vindex++] = vB + vExtent;
				m_pShadowVolume[vindex++] = vC + vExtent;
			}
			m_iNumShadowVolumeFaces++;
		}
	}

	return m_iNumShadowVolumeFaces;
}

int CGutShadowVolume::BuildShadowVolume_PointLight(Vector4 &light_pos, Matrix4x4 &world_matrix, float fExtent, bool bCaps)
{
	int shadowvolume_quads = 0;
	//
	Vector4 light_dir(0.0f, 0.0f, -1.0f);

	// 把3角形的normal轉換world space
	for ( int f=0; f<m_iNumFaces; f++ )
	{
		sFace *pFace = m_pFaceArray + f;
		pFace->m_Normal = world_matrix.RotateVector(pFace->m_Normal_ObjectSpace);
	}

	// 先計算并標示出邊界
	for ( int e=0; e<m_iNumEdges; e++ )
	{
		sEdge *pEdge = &m_EdgeArray[e];

		Vector4 vA = *pEdge->m_pA * world_matrix;
		Vector4 vVectorA = light_pos - vA;

		int faceA = DecodeEdgeIndex(pEdge->m_FaceA);
		int faceB = DecodeEdgeIndex(pEdge->m_FaceB);

		Vector4 vDotFaceA = Vector3Dot(m_pFaceArray[faceA].m_Normal, vVectorA);
		Vector4 vDotFaceB = Vector3Dot(m_pFaceArray[faceB].m_Normal, vVectorA);
		Vector4 vDotMul = vDotFaceA * vDotFaceB;

		if ( vDotMul.GetZ() < 0.0f )
		{
			pEdge->m_bShadowVolumeEdge = true;
			shadowvolume_quads++;
		}
		else
		{
			pEdge->m_bShadowVolumeEdge = false;
		}

		m_pFaceArray[faceA].m_bFrontFace = vDotFaceA.GetX() > 0;
		m_pFaceArray[faceB].m_bFrontFace = vDotFaceB.GetX() > 0;
	}

	m_iNumShadowVolumeFaces = shadowvolume_quads * 2;

	if ( bCaps )
	{
		m_iNumShadowVolumeFaces += m_iNumFaces;
	}

	if ( m_pShadowVolume )
	{
		delete [] m_pShadowVolume;
	}

	m_pShadowVolume = new Vector4 [m_iNumShadowVolumeFaces*3];

	int vindex = 0;
	m_iNumShadowVolumeFaces = 0;

	for ( int e=0; e<m_iNumEdges; e++ )
	{
		sEdge *pEdge = &m_EdgeArray[e];

		if ( pEdge->m_bShadowVolumeEdge )
		{
			Vector4 A = *pEdge->m_pA * world_matrix;
			Vector4 B = *pEdge->m_pB * world_matrix;
			Vector4 vVectorA = A - light_pos; vVectorA.Normalize();
			Vector4 vVectorB = B - light_pos; vVectorB.Normalize();
			Vector4 vExtentA = vVectorA * fExtent;
			Vector4 vExtentB = vVectorB * fExtent;

			int faceA = DecodeEdgeIndex(pEdge->m_FaceA);

			Vector4 vDotFaceA = Vector3Dot(m_pFaceArray[faceA].m_Normal, vVectorA);

			if ( vDotFaceA.GetX()>0.0f )
			{
				m_pShadowVolume[vindex++] = A;
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A + vExtentA; 
				m_iNumShadowVolumeFaces++;

				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = B + vExtentB;
				m_pShadowVolume[vindex++] = A + vExtentA;
				m_iNumShadowVolumeFaces++;
			}
			else
			{
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A;
				m_pShadowVolume[vindex++] = A + vExtentA; 
				m_iNumShadowVolumeFaces++;

				m_pShadowVolume[vindex++] = B + vExtentB;
				m_pShadowVolume[vindex++] = B;
				m_pShadowVolume[vindex++] = A + vExtentA;
				m_iNumShadowVolumeFaces++;
			}
		}
	}

	// 使用zfail時,要把整個模型前後加上蓋子
	if ( bCaps )
	{
		for ( int f=0; f<m_iNumFaces; f++ )
		{
			sFace *pFace = m_pFaceArray + f;

			Vector4 vA = (*pFace->m_pA) * world_matrix;
			Vector4 vB = (*pFace->m_pB) * world_matrix;
			Vector4 vC = (*pFace->m_pC) * world_matrix;

			if ( pFace->m_bFrontFace )
			{
				m_pShadowVolume[vindex++] = vA;
				m_pShadowVolume[vindex++] = vB;
				m_pShadowVolume[vindex++] = vC;
			}
			else
			{
				Vector4 vVectorA = vA - light_pos; vVectorA.Normalize();
				Vector4 vVectorB = vB - light_pos; vVectorB.Normalize();
				Vector4 vVectorC = vC - light_pos; vVectorC.Normalize();

				Vector4 vExtentA = vVectorA * fExtent;
				Vector4 vExtentB = vVectorB * fExtent;
				Vector4 vExtentC = vVectorC * fExtent;

				m_pShadowVolume[vindex++] = vA + vExtentA;
				m_pShadowVolume[vindex++] = vB + vExtentB;
				m_pShadowVolume[vindex++] = vC + vExtentC;
			}
			m_iNumShadowVolumeFaces++;
		}
	}

	return m_iNumShadowVolumeFaces;
}
