#include <string.h>

#include "Gut.h"
#include "GutModel.h"

char *CGutModel::m_pszTexturePath = NULL;

void sVertexDecl::SetVertexFormat(int PosElems, int NormalElems, int ColorElems, int Tex0Elems, int Tex1Elems, int Tex2Elems, int Tex3Elems)
{
	m_iVertexSize = 0;

	if ( PosElems )
	{
		m_iPositionOffset = m_iVertexSize;
		m_iNumPositionElements = PosElems;
		m_iVertexSize += PosElems * 4;
	}

	if ( NormalElems )
	{
		m_iNormalOffset = m_iVertexSize;
		m_iNumNormalElements = NormalElems;
		m_iVertexSize += NormalElems * 4;
	}

	if ( ColorElems )
	{
		m_iColorOffset = m_iVertexSize;
		m_iNumColorElements = ColorElems;
		m_iVertexSize += ColorElems * 4;
	}

	if ( Tex0Elems )
	{
		m_iTexcoordOffset[0] = m_iVertexSize;
		m_iNumTexcoordElements[0] = Tex0Elems;
		m_iVertexSize += Tex0Elems * 4;
	}
	if ( Tex1Elems )
	{
		m_iTexcoordOffset[1] = m_iVertexSize;
		m_iNumTexcoordElements[1] = Tex1Elems;
		m_iVertexSize += Tex1Elems * 4;
	}
	if ( Tex2Elems )
	{
		m_iTexcoordOffset[2] = m_iVertexSize;
		m_iNumTexcoordElements[2] = Tex2Elems;
		m_iVertexSize += Tex2Elems * 4;
	}
	if ( Tex3Elems )
	{
		m_iTexcoordOffset[3] = m_iVertexSize;
		m_iNumTexcoordElements[3] = Tex3Elems;
		m_iVertexSize += Tex3Elems * 4;
	}
}

sModelMaterial::sModelMaterial(void)
{
	memset(this, 0, sizeof(sModelMaterial));
}

void sModelMesh::Load_ASCII(FILE *pFile)
{
	const int BufferSize = 256;
	char szBuffer[BufferSize];
	char sz_Tag[BufferSize];
	char sz_Content[BufferSize];
	int i,j;
	fgets(szBuffer, BufferSize, pFile); // {

	fgets(szBuffer, BufferSize, pFile); // matrix
	fgets(szBuffer, BufferSize, pFile); // {
	for ( int r=0; r<4; r++ )
	{
		fgets(szBuffer, BufferSize, pFile); // x,y,z
		sscanf(szBuffer, "%f,%f,%f", &m_Matrix[r][0], &m_Matrix[r][1], &m_Matrix[r][2]);
	}
	fgets(szBuffer, BufferSize, pFile); // }

	fgets(szBuffer, BufferSize, pFile); // buffers n
	sscanf(szBuffer, "%s %d", sz_Tag, &m_iNumVertexChunks);

	m_iNumFaces = 0;
	m_iNumVertices = 0;

	m_vMin.Set(FLT_MAX);
	m_vMax.Set(-FLT_MAX);

	if ( m_iNumVertexChunks )
	{
		m_pVertexChunks = new sModelVertexChunk[m_iNumVertexChunks];
		for ( i=0; i<m_iNumVertexChunks; i++ )
		{
			fgets(szBuffer, BufferSize, pFile); // {

			sModelVertexChunk *pBuffer = m_pVertexChunks + i;
			fgets(szBuffer, BufferSize, pFile); // vertices n
			sscanf(szBuffer, "%s %d", sz_Tag, &pBuffer->m_iNumVertices);
			fgets(szBuffer, BufferSize, pFile); // format 
			sscanf(szBuffer, "%s %s", sz_Tag, sz_Content); 
			fgets(szBuffer, BufferSize, pFile); // {

			m_iNumVertices += pBuffer->m_iNumVertices;

			bool bVertex = false;
			bool bNormal = false;
			bool bColor = false;
			bool bTangentSpace = false;
			int  iNumUVs = 0;
			int  strLen = strlen(sz_Content);

			for ( int s=0; s<strLen; s++ )
			{
				switch(sz_Content[s])
				{
				case 'v':
					bVertex = true;
					break;
				case 'n':
					bNormal = true;
					break;
				case 'c':
					bColor = true;
					break;
				case 't':
					iNumUVs = sz_Content[++s] - '0';
					break;
				case '_':
					break;
				case 'p':
				case 'q':
					bTangentSpace = true;
					break;
				default:
					break;
				}
			}

			int vsize = 0;
			if ( bVertex )
			{
				pBuffer->m_VertexDecl.m_iPositionOffset = vsize;
				pBuffer->m_VertexDecl.m_iNumPositionElements = 3;
				vsize += 4*3;
			}

			if ( bNormal )
			{
				pBuffer->m_VertexDecl.m_iNormalOffset = vsize;
				pBuffer->m_VertexDecl.m_iNumNormalElements = 3;
				vsize += 4*3;
			}

			if ( bColor )
			{
				pBuffer->m_VertexDecl.m_iColorOffset = vsize;
				pBuffer->m_VertexDecl.m_iNumColorElements = 4;
				vsize += 4;
			}

			for ( j=0; j<iNumUVs; j++ )
			{
				pBuffer->m_VertexDecl.m_iTexcoordOffset[j] = vsize;
				pBuffer->m_VertexDecl.m_iNumTexcoordElements[j] = 2;
				vsize += 4*2;
			}

			if ( bTangentSpace )
			{
				pBuffer->m_VertexDecl.m_iTangentOffset = vsize;
				pBuffer->m_VertexDecl.m_iNumTangentElements = 3;
				vsize += 4 * 3;

				pBuffer->m_VertexDecl.m_iBiNormalOffset = vsize;
				pBuffer->m_VertexDecl.m_iBiNormalElements = 3;
				vsize += 4 * 3;
			}

			if ( vsize==0 )
			{
				int a=0;
			}

			pBuffer->m_VertexDecl.m_iVertexSize = vsize;

			if ( pBuffer->m_iNumVertices )
			{
				pBuffer->m_pVertexArray = new sModelVertex[pBuffer->m_iNumVertices];
			}

			printf(".");

			for ( j=0; j<pBuffer->m_iNumVertices; j++ )
			{
				sModelVertex *pVertex = pBuffer->m_pVertexArray + j;

				Vector4 &position = pVertex->m_Position;
				Vector4 &normal = pVertex->m_Normal;
				Vector4 &color = pVertex->m_Color;
				Vector4 *pUV = pVertex->m_Texcoord;
				Vector4 &tangent = pVertex->m_Tangent;
				Vector4 &binormal = pVertex->m_BiNormal;

				fgets(szBuffer, BufferSize, pFile);
				char *pLoc = szBuffer;

				if ( bVertex )
				{
					pLoc = strstr(pLoc, " ");
					sscanf(pLoc, "%f,%f,%f", &position[0], &position[1], &position[2]);
					position[3] = 1.0f;

					m_vMin = VectorMin(m_vMin, position);
					m_vMax = VectorMax(m_vMax, position);
				}

				if ( bNormal )
				{
					pLoc = strstr(pLoc+1, " ");
					pLoc = strstr(pLoc+1, " ");
					sscanf(pLoc, "%f,%f,%f", &normal[0], &normal[1], &normal[2]);
					normal[3] = 1.0f;
				}

				if ( bColor )
				{
					pLoc = strstr(pLoc+1, " ");
					pLoc = strstr(pLoc+1, " ");
					sscanf(pLoc, "%f,%f,%f,%f", &color[0], &color[1], &color[2], &color[3]);
				}

				for ( int t=0; t<iNumUVs && t<MAX_TEXCOORDS; t++ )
				{
					pLoc = strstr(pLoc+1, " ");
					pLoc = strstr(pLoc+1, " ");
					sscanf(pLoc, "%f,%f", &pUV[t][0], &pUV[t][1]);
				}

				if ( bTangentSpace )
				{
					pLoc = strstr(pLoc+1, " ");
					pLoc = strstr(pLoc+1, " ");
					sscanf(pLoc, "%f,%f,%f", &tangent[0], &tangent[1], &tangent[2]);

					pLoc = strstr(pLoc+1, " ");
					pLoc = strstr(pLoc+1, " ");
					sscanf(pLoc, "%f,%f,%f", &binormal[0], &binormal[1], &binormal[2]);
				}

				if ( (j & 0xff)==0 )
					printf(".");
			}

			fgets(szBuffer, BufferSize, pFile); // }

			fgets(szBuffer, BufferSize, pFile); // triangle_list_indices n
			sscanf(szBuffer, "%s %d", sz_Tag, &pBuffer->m_iNumIndices);
			if ( pBuffer->m_iNumIndices )
				pBuffer->m_pIndexArray = new unsigned short[pBuffer->m_iNumIndices];
			fgets(szBuffer, BufferSize, pFile); // {
			for ( j=0; j<pBuffer->m_iNumIndices/3; j++ )
			{
				int a,b,c;
				fgets(szBuffer, BufferSize, pFile);
				sscanf(szBuffer, "%d %d %d", &a, &b, &c);
				unsigned short *p = pBuffer->m_pIndexArray + j*3;
				p[0] = a; p[1] = b; p[2] = c;
			}
			fgets(szBuffer, BufferSize, pFile); // }

			fgets(szBuffer, BufferSize, pFile); // batches n
			sscanf(szBuffer, "%s %d", sz_Tag, &pBuffer->m_iNumBatches);
			if ( pBuffer->m_iNumBatches )
				pBuffer->m_pBatchArray = new sModelBatch[pBuffer->m_iNumBatches];
			fgets(szBuffer, BufferSize, pFile); // {
			for ( j=0; j<pBuffer->m_iNumBatches; j++ )
			{
				sModelBatch *pBatch = pBuffer->m_pBatchArray + j;

				fgets(szBuffer, BufferSize, pFile); // material n
				sscanf(szBuffer, "%s %d", sz_Tag, &pBatch->m_iMaterialID);
				fgets(szBuffer, BufferSize, pFile); // faces n
				sscanf(szBuffer, "%s %d", sz_Tag, &pBatch->m_iNumPrimitives);
				fgets(szBuffer, BufferSize, pFile); // index_begin n
				sscanf(szBuffer, "%s %d", sz_Tag, &pBatch->m_iIndexArrayBegin);

				pBatch->m_iNumIndices = pBatch->m_iNumPrimitives * 3;
				pBatch->m_iIndexArrayEnd = pBatch->m_iIndexArrayBegin + pBatch->m_iNumIndices;

				m_iNumFaces += pBatch->m_iNumPrimitives;
			}
			fgets(szBuffer, BufferSize, pFile); // }

			fgets(szBuffer, BufferSize, pFile); // }
		}
	}

	printf(".\n");
	fgets(szBuffer, BufferSize, pFile); // }
}

void sModelMesh::Load_BINARY(FILE *pFile)
{

}

CGutModel::CGutModel(void)
{
	//memset(&m_VertexDecl, 0xffffffff, sizeof(m_VertexDecl));

	m_iNumMaterials = 0;
	m_iNumMeshes = 0;

	m_pMaterialArray = NULL;
	m_pMeshArray = NULL;

	m_pFile = NULL;
}

CGutModel::~CGutModel()
{
	Release();
}

void CGutModel::Release(void)
{
	if ( m_pMaterialArray )
	{
		delete [] m_pMaterialArray;
		m_pMaterialArray = NULL;
	}

	if ( m_pMeshArray )
	{
		delete [] m_pMeshArray;
		m_pMeshArray = NULL;
	}
}

bool CGutModel::LoadMaterial_ASCII(void)
{
	char szBuffer[128];
	char szTag[64];
	char szContent[64];

	fgets(szBuffer, 128, m_pFile); // {
	fgets(szBuffer, 128, m_pFile); // materials n
	sscanf(szBuffer, "%s %d", szTag, &m_iNumMaterials);

	if ( m_iNumMaterials )
	{
		m_pMaterialArray = new sModelMaterial[m_iNumMaterials];

		for ( int i=0; i<m_iNumMaterials; i++ )
		{
			sModelMaterial *pMtl = m_pMaterialArray + i;

			fgets(szBuffer, 128, m_pFile); // {

			// lighting
			fgets(szBuffer, 128, m_pFile); // material name
			fgets(szBuffer, 128, m_pFile); // emissive
			Vector4 *pV = &pMtl->m_Emissive; pV->w = 1.0f;
			sscanf(szBuffer, "%s %s %f,%f,%f", szTag, szTag, &pV->x, &pV->y, &pV->z);

			fgets(szBuffer, 128, m_pFile); // ambient
			pV = &pMtl->m_Ambient; pV->w = 1.0f;
			sscanf(szBuffer, "%s %s %f,%f,%f", szTag, szTag, &pV->x, &pV->y, &pV->z);

			fgets(szBuffer, 128, m_pFile); // diffuse
			pV = &pMtl->m_Diffuse; pV->w = 1.0f;
			sscanf(szBuffer, "%s %s %f,%f,%f", szTag, szTag, &pV->x, &pV->y, &pV->z);

			fgets(szBuffer, 128, m_pFile); // specular
			pV = &pMtl->m_Specular; pV->w = 1.0f;
			sscanf(szBuffer, "%s %s %f,%f,%f", szTag, szTag, &pV->x, &pV->y, &pV->z);

			fgets(szBuffer, 128, m_pFile); // shininess
			sscanf(szBuffer, "%s %s %f", szTag, szTag, &pMtl->m_fShininess);
			pMtl->m_fShininess *= 128.0f;

			fgets(szBuffer, 128, m_pFile); // blendmode
			sscanf(szBuffer, "%s %s %s", szTag, szTag, pMtl->m_szBlendMode);

			fgets(szBuffer, 128, m_pFile); // cullface
			sscanf(szBuffer, "%s %s %s", szTag, szTag, szContent);

			if ( !strcmp(szContent, "on") )
				pMtl->m_bCullFace = true;
			else
				pMtl->m_bCullFace = false;

			// textures
			fgets(szBuffer, 128, m_pFile); // diffuseMap
			sscanf(szBuffer, "%s %s %s %s %s %d", szTag, szTag, szContent, szTag, szTag, &pMtl->m_MapChannel[0]);
			// eliminate '\"'
			strcpy(pMtl->m_szTexture[0], &szContent[1]);
			pMtl->m_szTexture[0][ strlen(pMtl->m_szTexture[0]) - 1 ] = '\0';
			if ( !strcmp(pMtl->m_szTexture[0], "None") )
				pMtl->m_szTexture[0][0] = '\0';

			fgets(szBuffer, 128, m_pFile); // lightMap
			sscanf(szBuffer, "%s %s %s %s %s %d", szTag, szTag, szContent, szTag, szTag, &pMtl->m_MapChannel[1]);
			// eliminate '\"'
			strcpy(pMtl->m_szTexture[1], &szContent[1]);
			pMtl->m_szTexture[1][ strlen(pMtl->m_szTexture[1]) - 1 ] = '\0';
			if ( !strcmp(pMtl->m_szTexture[1], "None") )
				pMtl->m_szTexture[1][0] = '\0';

			fgets(szBuffer, 128, m_pFile); // environmentMap
			sscanf(szBuffer, "%s %s %s", szTag, szTag, szContent);
			// eliminate '\"'
			strcpy(pMtl->m_szTexture[2], &szContent[1]);
			pMtl->m_szTexture[2][ strlen(pMtl->m_szTexture[2]) - 1 ] = '\0';
			if ( !strcmp(pMtl->m_szTexture[2], "None") )
				pMtl->m_szTexture[2][0] = '\0';
			else
				//pMtl->m_MapChannel[2] = MAP_SPHEREMAP; // spheremap
				pMtl->m_MapChannel[2] = MAP_CUBEMAP; // cubemap

			fgets(szBuffer, 128, m_pFile); // }
		}
	}

	fgets(szBuffer, 128, m_pFile); // }

	return true;
}

bool CGutModel::LoadMesh_ASCII(void)
{
	char szBuffer[128];
	char sz_Tag[64];

	fgets(szBuffer, 128, m_pFile); // {
	fgets(szBuffer, 128, m_pFile); // meshes n
	sscanf(szBuffer, "%s %d", sz_Tag, &m_iNumMeshes);

	m_iNumFaces = 0;
	m_iNumVertices = 0;

	m_pMeshArray = new sModelMesh[m_iNumMeshes];
	if ( m_pMeshArray==NULL )
		return false;

	m_vMin.Set(FLT_MAX);
	m_vMax.Set(-FLT_MAX);

	for ( int i=0; i<m_iNumMeshes; i++ )
	{
		printf("Loading mesh %d/%d\n", i+1, m_iNumMeshes);

		m_pMeshArray[i].Load_ASCII(m_pFile);

		m_iNumFaces += m_pMeshArray[i].m_iNumFaces;
		m_iNumVertices += m_pMeshArray[i].m_iNumVertices;

		m_vMin = VectorMin(m_vMin, m_pMeshArray[i].m_vMin);
		m_vMax = VectorMax(m_vMax, m_pMeshArray[i].m_vMax);
	}

	fgets(szBuffer, 128, m_pFile); // }

	return true;
}

bool CGutModel::Load_ASCII(const char *filename)
{
	m_pFile = fopen(filename, "rt");
	if ( m_pFile==NULL )
		return false;

	printf("Loading model %s\n", filename);

	char szBuffer[128];
	char szTag[64], szChunkID[64];

	fgets(szBuffer, 128, m_pFile); // version

	while(!feof(m_pFile))
	{
		fgets(szBuffer, 128, m_pFile);
		if ( feof(m_pFile) )
			break;
		sscanf(szBuffer, "%s %s", szTag, szChunkID);	
		if ( !strcmp(szTag, "Begin") )
		{
			if ( !strcmp(szChunkID, "Material") )
			{
				LoadMaterial_ASCII();
			}
			else if ( !strcmp(szChunkID, "Mesh") )
			{
				LoadMesh_ASCII();
			}
		}
	}

	m_vSize = m_vMax - m_vMin;
	printf("%d materials, %d vertices, %d faces, size (%6.3f,%6.3f,%6.3f)\n", 
		m_iNumMaterials, m_iNumVertices, m_iNumFaces,
		m_vSize[0], m_vSize[1], m_vSize[2]);

	return true;
}

bool CGutModel::LoadMaterial_BINARY(void)
{
	return true;
}

bool CGutModel::LoadMesh_BINARY(void)
{
	return true;
}

bool CGutModel::Load_BINARY(const char *filename)
{
	return false;
}

void sModelVertexChunk::OutputVertexBuffer(void *pBuffer)
{
	unsigned char *p = (unsigned char *) pBuffer;
	float *fp = (float *) p;

	for ( int v=0; v<m_iNumVertices; v++ )
	{
		sModelVertex *pVertex = m_pVertexArray + v;

		int s = 0;

		if ( m_VertexDecl.m_iPositionOffset >= 0 )
		{
			for ( int i=0; i<m_VertexDecl.m_iNumPositionElements; i++ )
			{
				*fp++ = pVertex->m_Position[i];
				p+=4;
				s+=4;
			}
		}

		if ( m_VertexDecl.m_iNormalOffset >= 0 )
		{
			for ( int i=0; i<m_VertexDecl.m_iNumNormalElements; i++ )
			{
				*fp++ = pVertex->m_Normal[i];
				p+=4;
				s+=4;
			}
		}

		if ( m_VertexDecl.m_iColorOffset >= 0 )
		{
			for ( int i=0; i<m_VertexDecl.m_iNumColorElements; i++ )
			{
				*p++ = (unsigned char) (pVertex->m_Color[i]*255);
				s++;
			}
			fp++;
		}

		for ( int t=0; t<MAX_TEXCOORDS; t++ )
		{
			if ( m_VertexDecl.m_iTexcoordOffset[t] >= 0 )
			{
				for ( int i=0; i<m_VertexDecl.m_iNumTexcoordElements[t]; i++ )
				{
					*fp++ = pVertex->m_Texcoord[t][i];
					p+=4;
					s+=4;
				}
			}
		}

		if ( m_VertexDecl.m_iTangentOffset >=0 )
		{
			memcpy(fp, &pVertex->m_Tangent, 12);
			fp+=3;
			p+=12;
			s+=12;

			memcpy(fp, &pVertex->m_BiNormal, 12);
			fp+=3;
			p+=12;
			s+=12;
		}
	}
}

void CGutModel::SetTexturePath(const char *path)
{
	if ( m_pszTexturePath )
	{
		free(m_pszTexturePath);
	}

	m_pszTexturePath = strdup(path);
}

const sVertexDecl *CGutModel::GetVertexFormat(void)
{
	if ( m_pMeshArray && m_pMeshArray[0].m_pVertexChunks )
		return &m_pMeshArray[0].m_pVertexChunks[0].m_VertexDecl;
	return NULL;
}

bool CGutModel::CreateSphere(float radius, sVertexDecl *pVertexDecl, int stacks, int slices)
{
	Release();

	m_pMeshArray = new sModelMesh[1];
	sModelMesh *pMesh = m_pMeshArray;
	if ( NULL==pMesh )
		return false;

	m_pMeshArray[0].m_pVertexChunks = new sModelVertexChunk[1];
	sModelVertexChunk *pVertexChunk = m_pMeshArray[0].m_pVertexChunks;
	if ( NULL==pVertexChunk )
		return false;

	if ( pVertexDecl )
		pVertexChunk->m_VertexDecl = *pVertexDecl;

	int num_vertices = (stacks+1)*(slices+1);
	int num_triangles = stacks*slices*2;
	int num_indices = num_triangles * 3;

	m_pMeshArray[0].m_iNumVertexChunks = 1;
	pVertexChunk->m_pVertexArray = new sModelVertex[num_vertices];
	sModelVertex *pVertices = pVertexChunk->m_pVertexArray;
	if ( NULL==pVertices )
		return false;

	pVertexChunk->m_pBatchArray = new sModelBatch[1];
	sModelBatch *pBatch = pVertexChunk->m_pBatchArray;
	if ( NULL==pBatch )
		return false;

	m_iNumMeshes = 1;
	m_iNumFaces = num_triangles;
	m_iNumVertices = num_vertices;

	pMesh->m_iNumFaces = num_triangles;
	pMesh->m_iNumVertices = num_vertices;

	pBatch->m_iNumIndices = num_indices;
	pBatch->m_iNumPrimitives = num_triangles;
	pBatch->m_iNumVertices = num_vertices;
	pBatch->m_iIndexArrayBegin = 0;
	pBatch->m_iIndexArrayEnd = num_indices;

	pVertexChunk->m_iNumBatches = 1;
	pVertexChunk->m_iNumIndices = num_indices;
	pVertexChunk->m_iNumVertices = num_vertices;
	pVertexChunk->m_iNumPrimitives = num_triangles;

	Vector4 vDefaultColor(1.0f);
	Vector4 vRadius(radius);

	const float theta_start_degree = 0.0f;
	const float theta_end_degree = 360.0f;
	const float phi_start_degree = -90.0f;
	const float phi_end_degree = 90.0f;

	float ts = FastMath::DegToRad(theta_start_degree);
	float te = FastMath::DegToRad(theta_end_degree);
	float ps = FastMath::DegToRad(phi_start_degree);
	float pe = FastMath::DegToRad(phi_end_degree);

	float theta_total = te - ts;
	float phi_total = pe - ps;
	float theta_inc = theta_total/stacks;
	float phi_inc = phi_total/slices;

	float ty_start = 1.0f;
	float ty_step = -1.0f/(float)slices;
	float tx_start = 1.0f;
	float tx_step = -1.0f/(float)stacks;

	Vector4 vTexcoord(tx_start, ty_start, 0.0f);

	int i,j;
	int index = 0;
	float theta = ts;

	for ( i=0; i<=stacks; i++ )
	{
		float phi = ps;
		float sin_theta, cos_theta;
		FastMath::SinCos(theta, sin_theta, cos_theta);

		for ( j=0; j<=slices; j++, index++ )
		{
			float sin_phi, cos_phi;
			FastMath::SinCos(phi, sin_phi, cos_phi);
			Vector4 vNormal(cos_phi * cos_theta, sin_phi, cos_phi * sin_theta);
			// Position
			pVertices[index].m_Position = vRadius * vNormal;
			// Normal
			pVertices[index].m_Normal = vNormal;
			// Color
			pVertices[index].m_Color = vDefaultColor;
			// Texcoord
			pVertices[index].m_Texcoord[0] = vTexcoord;
			// inc phi
			phi += phi_inc;
			// inc texcoord
			vTexcoord[1] += ty_step;
		}
		// inc theta
		theta += theta_inc;
		// reset & inc texcoord
		vTexcoord[0] += tx_step;
		vTexcoord[1] = ty_start;
	}

	// build index array

	unsigned short *pIndices = new unsigned short[num_triangles*3];
	if ( pIndices==NULL )
	{
		delete [] pVertices;
		return false;
	}

	pVertexChunk->m_pIndexArray = pIndices;

	int base = 0;
	index = 0;

	// triangle list
	for ( i=0; i<stacks; i++ )
	{
		for ( j=0; j<slices; j++ )
		{
			pIndices[index++] = base;
			pIndices[index++] = base+1;
			pIndices[index++] = base+slices+1;

			pIndices[index++] = base+1;
			pIndices[index++] = base+slices+2;
			pIndices[index++] = base+slices+1;

			base++;
		}
		base++;
	}

	return true;
}