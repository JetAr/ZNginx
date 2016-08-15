#include "gmodelexp.h"
#include "exportmesh.h"
#include "3dsmaxport.h"
#include "utility.h"
#include "exportmesh.h"

const int g_iHashTableSize = 65536;
const int g_iHastTableColumns = 16;
unsigned short g_HashTable[g_iHashTableSize][g_iHastTableColumns];

int sMeshVertexArray::AddVertex_LinearSearch(sMeshVertex &vertex)
{
	vertex.m_Checksum = GenerateChecksum(&vertex.m_Position, sizeof(sMeshVertex)-4);

	for ( int i=0; i<m_VertexArray.size(); i++ )
	{
		sMeshVertex *pVertex = &m_VertexArray[i];
		if ( pVertex->m_Checksum == vertex.m_Checksum )
		{
			if ( !memcmp(pVertex, &vertex, sizeof(sMeshVertex)) )
				return i;
		}
	}

	m_VertexArray.push_back(vertex);
	int vindex = m_VertexArray.size() - 1;
	return vindex;
}

int sMeshVertexArray::AddVertex_HashTable(sMeshVertex &vertex)
{
	vertex.m_Checksum = GenerateChecksum(&vertex.m_Position, sizeof(sMeshVertex)-4);

	int hash_index = GenerateHash_16bits(&vertex, 36);
	bool found = false;
	int vertex_index = -1;
	int i;

	for ( i=0; i<g_iHastTableColumns; i++ )
	{
		unsigned short vindex = g_HashTable[hash_index][i];

		if ( vindex==0xffff )
		{
			break;
		}
		else
		{
			sMeshVertex *pVertex = &m_VertexArray[vindex];
			if ( pVertex->m_Checksum==vertex.m_Checksum && !memcmp(pVertex, &vertex, sizeof(sMeshVertex)) )
			{
				return vindex;
			}
		}
	}

	if ( i<g_iHastTableColumns )
		// not in vertex array
	{
		m_VertexArray.push_back(vertex);
		vertex_index = m_VertexArray.size() - 1;
		g_HashTable[hash_index][i] = vertex_index;
	}
	else
		// out of hashtable, run linear search instead.
	{
		vertex_index = AddVertex_LinearSearch(vertex);
	}

	return vertex_index;
}

int sMeshVertexArray::AddVertex(sMeshVertex &vertex)
{
	return AddVertex_HashTable(vertex);
}

CMeshBuilder::CMeshBuilder(void)
{
	m_iNumUVs = 0;
	m_iNumVertexArrays = 0;
}

bool CMeshBuilder::BuildMesh_v0(INode *pNode, GmodelExp *pExporter)
{
	Object *obj = pNode->EvalWorldState(pExporter->m_EvaluateTime).obj;
	TriObject *tri = (TriObject *)obj->ConvertToType(pExporter->m_EvaluateTime, triObjectClassID);
	if (tri==NULL)
		return false;

	Mesh& mesh = tri->GetMesh();
	mesh.buildNormals();

	m_VertexArray.clear();

	Mtl *pNodeMaterial = pNode->GetMtl();
	bool bMultiSubMaterial = false;
	if ( pNodeMaterial ) 
		bMultiSubMaterial = pNodeMaterial->IsMultiMtl();

	bool bExportMaterial = pExporter->m_bExportMaterial && pNodeMaterial;

	// put faces applied with the same material in the same group.
	std::map<int, std::vector<int>> MaterialGroup;
	int mtl_id = 0;
	int num_faces = mesh.getNumFaces();
	const int max_faces_each_array = 65536/3;
	int num_faces_each_array = num_faces < max_faces_each_array ? num_faces : max_faces_each_array;
	int base_face_index = 0;
	int remain_faces = num_faces;
	int max_num_vertices = num_faces * 3;
	int v_index[] = {0, 1, 2};

	m_iNumVertexArrays = ( num_faces + max_faces_each_array - 1 ) / max_faces_each_array;
	m_VertexArray.resize(m_iNumVertexArrays);

	std::vector<sMeshFace> FaceArray;
	FaceArray.resize(num_faces_each_array);

	for (int vertex_array=0; vertex_array<m_iNumVertexArrays; vertex_array++)
	{
		memset(g_HashTable, 0xffffffff, sizeof(g_HashTable));
		int faces_to_be_processed = num_faces_each_array < remain_faces ? num_faces_each_array : remain_faces;

		sMeshVertexArray *pVertexArray = &m_VertexArray[vertex_array];
		pVertexArray->m_IndexArray_TriList.resize(faces_to_be_processed * 3);

		sMeshVertex vertex;
		MaterialGroup.clear();

		for (int f=0; f<faces_to_be_processed; f++)
		{
			int findex = base_face_index + f;
			if ( bExportMaterial && bMultiSubMaterial )
			{
				mtl_id = mesh.faces[findex].getMatID();
			}
			MaterialGroup[mtl_id].push_back(f);

			sMeshFace *pFace = &FaceArray[f];
			for ( int i=0; i<3; i++ )
			{
				int position_index = mesh.faces[f].v[v_index[i]];
				vertex.m_Position = mesh.verts[position_index];
				pFace->m_iVertexIndex[i] = pVertexArray->AddVertex(vertex);
			}
		}

		int num_materials = MaterialGroup.size();
		pVertexArray->m_BatchList.resize(num_materials);

		int faces_processed = 0;
		int batch_index = 0;
		std::map<int, std::vector<int>>::iterator item;

		for ( item = MaterialGroup.begin(); item!=MaterialGroup.end(); item++, batch_index++ )
		{
			std::vector<int> &MaterialFaceList = item->second;
			sMeshBatch *pBatch = &pVertexArray->m_BatchList[batch_index];
			int num_mtlgroup_faces = MaterialFaceList.size();

			int MaterialID = -1;

			if ( bExportMaterial )
			{
				Mtl *pMaterial = pNodeMaterial;

				if ( bMultiSubMaterial )
				{
					int submtl_id = item->first;
					pMaterial = pNodeMaterial->GetSubMtl(submtl_id);
				}

				MaterialID = pExporter->SearchMaterial(pMaterial);
			}

			pBatch->m_iMaterialIndex = MaterialID;
			pBatch->m_iFaceArrayBegin = faces_processed;
			pBatch->m_iNumPrimitives_TriList = num_mtlgroup_faces;
			pBatch->m_iIndexArrayBegin_TriList = pBatch->m_iFaceArrayBegin * 3;
			pBatch->m_iIndexArrayUsed_TriList = num_mtlgroup_faces * 3;
			//pBatch->m_FaceArray.resize(num_mtlgroup_faces);

			for ( int f=0; f<num_mtlgroup_faces; f++ )
			{
				int base = (faces_processed + f) * 3;
				int findex = MaterialFaceList[f];
				//pBatch->m_FaceArray[f] = FaceArray[findex];
				pVertexArray->m_IndexArray_TriList[base  ] = FaceArray[findex].m_iVertexIndex[0];
				pVertexArray->m_IndexArray_TriList[base+1] = FaceArray[findex].m_iVertexIndex[1];
				pVertexArray->m_IndexArray_TriList[base+2] = FaceArray[findex].m_iVertexIndex[2];
			}

			faces_processed += num_mtlgroup_faces;
		}

		remain_faces -= faces_to_be_processed;
		base_face_index += faces_to_be_processed;
	}

	return true;
}

Point3 CMeshBuilder::GetVertexNormal(Mesh &mesh, int findex, int vindex)
{
	Face* f = &mesh.faces[findex];
	int vert = f->getVert(vindex);
	RVertex *rv = mesh.getRVertPtr(vert);

	DWORD smGroup = f->smGroup;
	int numNormals;
	Point3 vertexNormal;

	// Is normal specified
	// SPCIFIED is not currently used, but may be used in future versions.
	if (rv->rFlags & SPECIFIED_NORMAL) 
	{
		vertexNormal = rv->rn.getNormal();
	}
	// If normal is not specified it's only available if the face belongs
	// to a smoothing group
	else if ((numNormals = rv->rFlags & NORCT_MASK) && smGroup) 
	{
		// If there is only one vertex is found in the rn member.
		if (numNormals == 1) 
		{
			vertexNormal = rv->rn.getNormal();
		}
		else
		{
			// If two or more vertices are there you need to step through them
			// and find the vertex with the same smoothing group as the current face.
			// You will find multiple normals in the ern member.
			for (int i = 0; i < numNormals; i++) 
			{
				if (rv->ern[i].getSmGroup() & smGroup) 
				{
					vertexNormal = rv->ern[i].getNormal();
				}
			}
		}
	}
	else 
	{
		// Get the normal from the Face if no smoothing groups are there
		vertexNormal = mesh.getFaceNormal(findex);
	}

	return vertexNormal;

}

BOOL TMNegParity(Matrix3 &m)
{
	return (DotProd(CrossProd(m.GetRow(0),m.GetRow(1)),m.GetRow(2))<0.0)?1:0;
}

bool CMeshBuilder::BuildMesh(INode *pNode, GmodelExp *pExporter)
{
	Object *obj = pNode->EvalWorldState(pExporter->m_EvaluateTime).obj;
	TriObject *tri = (TriObject *)obj->ConvertToType(pExporter->m_EvaluateTime, triObjectClassID);
	if (tri==NULL)
		return false;

	//m_Matrix = pNode->GetNodeTM(pExporter->m_EvaluateTime);
	//* Inverse(pNode->GetParentTM(pExporter->m_EvaluateTime));
	m_Matrix = pNode->GetObjectTM(pExporter->m_EvaluateTime);

	Matrix3 RotMatrix = m_Matrix;
	RotMatrix.NoTrans();
	RotMatrix.Orthogonalize();

	Mesh& mesh = tri->GetMesh();
	mesh.buildNormals();

	Mtl *pNodeMaterial = pNode->GetMtl();
	bool bMultiSubMaterial = false;
	if ( pNodeMaterial )
		bMultiSubMaterial = pNodeMaterial->IsMultiMtl();

	const int max_vertices_per_array = 65536-3;
	int mtl_id = 0;
	int num_faces = mesh.getNumFaces();
	int remain_faces = num_faces;
	int num_vertices_in_array = 0;
	int num_faces_in_array = 0;
	int num_faces_processed = 0;
	int base_face_index = 0;
	int v_index[] = {0, 1, 2};

	BOOL negScale = TMNegParity(m_Matrix);

	// Order of the vertices. Get 'em counter clockwise if the objects is
	// negatively scaled.
	if (negScale) 
	{
		v_index[0] = 2;
		v_index[1] = 1;
		v_index[2] = 0;
	}

	int num_uv = mesh.getNumMaps();
	m_iNumUVs = num_uv - 1;

	if ( m_iNumUVs > MAX_UVCHANNLES )
		m_iNumUVs = MAX_UVCHANNLES;

	bool bExportMaterial = pExporter->m_bExportMaterial && pNodeMaterial;
	bool bExportNormal = pExporter->m_bExportVertexNormals;
	bool bExportColor = pExporter->m_bExportVertexColors && mesh.numCVerts;
	bool bExportUVs = pExporter->m_bExportVertexUVs && m_iNumUVs;
	bool bExportTangentSpace = pExporter->m_bExportTangentSpace;

	UVVert *UVVert;
	TVFace *UVFace;

	m_VertexArray.clear();
	m_iNumVertexArrays = 0;

	while( remain_faces )
	{
		memset(g_HashTable, 0xffffffff, sizeof(g_HashTable));
		sMeshVertexArray vtemp;
		m_VertexArray.push_back(vtemp);

		sMeshVertexArray *pVertexArray = &m_VertexArray[m_iNumVertexArrays++];
		sMeshVertex vertex;
		vertex.m_Color.Set(1.0f, 1.0f, 1.0f, 1.0f);

		// put faces applied with the same material in the same group.
		std::map<int, std::vector<int>> MaterialGroup;

		std::vector<sMeshFace> FaceArray;
		FaceArray.clear();

		for (	num_faces_in_array=0, num_vertices_in_array=0;
			num_faces_in_array<remain_faces && num_vertices_in_array<max_vertices_per_array; 
			num_faces_in_array++	)
		{
			int findex = base_face_index + num_faces_in_array;
			if ( bExportMaterial && bMultiSubMaterial )
			{
				mtl_id = mesh.faces[findex].getMatID();
			}
			MaterialGroup[mtl_id].push_back(num_faces_in_array);

			sMeshFace face;
			face.m_iArrayIndex = m_iNumVertexArrays-1;

			for ( int i=0; i<3; i++ )
			{
				int vindex = v_index[i];
				int position_index = mesh.faces[findex].v[vindex];
				vertex.m_Position = mesh.verts[position_index] * m_Matrix;

				if ( pExporter->m_bExportVertexNormals )
				{
					vertex.m_Normal = GetVertexNormal(mesh, findex, vindex) * RotMatrix;
					if ( negScale )
					{
						vertex.m_Normal = -vertex.m_Normal;
					}
					vertex.m_Normal.Normalize();
				}

				// vertex color
				if ( bExportColor )
				{
					int color_index = mesh.vcFace[findex].t[v_index[i]];
					vertex.m_Color = mesh.vertCol[color_index];
				}

				// texcoord
				if ( bExportUVs )
				{
					for(int k=1; k <num_uv; k++)
					{	
						int index = k;
						if( mesh.getNumMapVerts(index) )
						{
							UVVert = mesh.mapVerts(index);
							UVFace = mesh.mapFaces(index);
							int uv_index = UVFace[findex].t[vindex];
							vertex.m_UV[k-1].x = UVVert[ uv_index ].x;
							vertex.m_UV[k-1].y = 1.0 - UVVert[ uv_index ].y;
						}
					}
				}

				face.m_iVertexIndex[i] = pVertexArray->AddVertex(vertex);
			}
			FaceArray.push_back(face);

			num_vertices_in_array = pVertexArray->m_VertexArray.size();
		}

		int num_materials = MaterialGroup.size();

		pVertexArray->m_IndexArray_TriList.resize(num_faces_in_array * 3);
		pVertexArray->m_BatchList.resize(num_materials);

		int batch_faces_processed = 0;
		int batch_index = 0;
		std::map<int, std::vector<int>>::iterator item;

		for ( item = MaterialGroup.begin(); item!=MaterialGroup.end(); item++, batch_index++ )
		{
			std::vector<int> &MaterialFaceList = item->second;
			sMeshBatch *pBatch = &pVertexArray->m_BatchList[batch_index];
			int num_mtlgroup_faces = MaterialFaceList.size();

			int MaterialID = -1;

			if ( bExportMaterial )
			{
				Mtl *pMaterial = pNodeMaterial;
				if ( bMultiSubMaterial )
				{
					int submtl_id = item->first;
					pMaterial = pNodeMaterial->GetSubMtl(submtl_id);
				}

				MaterialID = pExporter->SearchMaterial(pMaterial);
			}

			pBatch->m_iMaterialIndex = MaterialID;
			pBatch->m_iFaceArrayBegin = batch_faces_processed;
			pBatch->m_iNumPrimitives_TriList = num_mtlgroup_faces;
			pBatch->m_iIndexArrayBegin_TriList = pBatch->m_iFaceArrayBegin * 3;
			pBatch->m_iIndexArrayUsed_TriList = num_mtlgroup_faces * 3;

			for ( int f=0; f<num_mtlgroup_faces; f++ )
			{
				int base = (batch_faces_processed + f) * 3;
				int findex = MaterialFaceList[f];

				pVertexArray->m_IndexArray_TriList[base  ] = FaceArray[findex].m_iVertexIndex[0];
				pVertexArray->m_IndexArray_TriList[base+1] = FaceArray[findex].m_iVertexIndex[1];
				pVertexArray->m_IndexArray_TriList[base+2] = FaceArray[findex].m_iVertexIndex[2];
			}

			batch_faces_processed += num_mtlgroup_faces;
		}

		remain_faces -= num_faces_in_array;
		base_face_index += num_faces_in_array;
	}

	if ( bExportUVs && bExportTangentSpace )
	{
		// tangent & binormal
		for ( int v=0; v<m_iNumVertexArrays; v++ )
		{
			sMeshVertexArray *pVertexArray = &m_VertexArray[v];
			int faces = pVertexArray->m_IndexArray_TriList.size()/3;

			for ( int i=0; i<faces; i++ )
			{
				int i0 = pVertexArray->m_IndexArray_TriList[i*3  ];
				int i1 = pVertexArray->m_IndexArray_TriList[i*3+1];
				int i2 = pVertexArray->m_IndexArray_TriList[i*3+2];

				sMeshVertex *pV0 = &pVertexArray->m_VertexArray[i0];
				sMeshVertex *pV1 = &pVertexArray->m_VertexArray[i1];
				sMeshVertex *pV2 = &pVertexArray->m_VertexArray[i2];

				Point3 Q1 = pV1->m_Position - pV0->m_Position;
				Point3 Q2 = pV2->m_Position - pV0->m_Position;

				Point3 S = pV1->m_UV[0] - pV0->m_UV[0];
				Point3 T = pV2->m_UV[0] - pV0->m_UV[0];

				float d = S.x * T.y - S.y * T.x;

				Point3 Tangent = (T.y * Q1 - T.x * Q2)/d;
				Point3 Binormal = (-S.y * Q1 + S.x * Q2)/d;

				Tangent = Tangent.Normalize();
				Binormal = Binormal.Normalize();

				pV0->m_Tangent += Tangent;
				pV1->m_Tangent += Tangent;
				pV2->m_Tangent += Tangent;

				pV0->m_BiNormal += Binormal;
				pV1->m_BiNormal += Binormal;
				pV2->m_BiNormal += Binormal;
			}
		}

		// average
		for ( int v=0; v<m_iNumVertexArrays; v++ )
		{
			sMeshVertexArray *pVertexArray = &m_VertexArray[v];
			int vsize = pVertexArray->m_VertexArray.size();

			for ( int i=0; i<vsize; i++ )
			{
				sMeshVertex *pV = &pVertexArray->m_VertexArray[i];
				pV->m_Tangent = pV->m_Tangent.Normalize();
				pV->m_BiNormal = pV->m_BiNormal.Normalize();
			}
		}
	}

	return true;
}