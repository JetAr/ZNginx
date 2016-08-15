#include <windows.h>

#include "glew.h"
#include "wglew.h"
#include <GL/gl.h>

#include "Gut.h"
#include "GutOpenGL.h"
#include "GutModel_OpenGL.h"
#include "GutTexture_OpenGL.h"

void *sModelMaterial_OpenGL::operator new[](size_t size)
{
	sModelMaterial_OpenGL *pArray = (sModelMaterial_OpenGL *)GutAllocate16BytesAlignedMemory(size);
	return pArray;
}

void sModelMaterial_OpenGL::operator delete[](void *p)
{
	GutRelease16BytesAlignedMemory(p);
}

void sModelMaterial_OpenGL::Submit(sModelVertexChunk_OpenGL *pVChunk)
{
	glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &m_Emissive[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &m_Ambient[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &m_Diffuse[0]);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &m_Specular[0]);
	glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m_fShininess);

	if ( m_bCullFace )
	{
		glEnable(GL_CULL_FACE);
	}
	else
	{
		glDisable(GL_CULL_FACE);
	}

	if ( m_bBlend )
	{
		glEnable(GL_BLEND);
		glBlendFunc(m_SrcBlend, m_DestBlend);
	}
	else
	{
		glDisable(GL_BLEND);
	}

	static GLuint texblend[] = 
	{
		GL_MODULATE,
		GL_MODULATE,
		GL_ADD
	};

	int num_textures = 0;

	for ( int t=0; t<MAX_NUM_TEXTURES; t++ )
	{
		GLuint stage = GL_TEXTURE0_ARB + t;
		int m = m_MapChannel[t];
		if ( m < -2 )
			break;

		glActiveTexture(stage);
		glClientActiveTexture(stage);

		if ( m_Textures[t] > 0 )
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, m_Textures[t]);
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texblend[t]);
			num_textures++;
		}
		else
		{
			//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texblend[t]);
			//glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}

		if ( m >=0 )
		{
			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_Q);

			if ( pVChunk )
			{
				sVertexDecl *pVertexDecl = &pVChunk->m_VertexDecl;
				if ( m_Textures[t] && pVertexDecl->m_iTexcoordOffset[m] >= 0 )
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(pVertexDecl->m_iNumTexcoordElements[m], GL_FLOAT, pVertexDecl->m_iVertexSize, (GLvoid *) pVertexDecl->m_iTexcoordOffset[m]);
				}
				else
				{
					glDisableClientState(GL_TEXTURE_COORD_ARRAY);
				}
			}
			else
			{
				glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			}
		}
		else
		{
			switch(m)
			{
			case -1: // sphere map
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);
				// 自動產生spheremap貼圖座標
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				break;
			case -2: // cube map
				// S/T/R分別代表貼圖座標的X/Y/Z軸
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);
				// 以光線的反射向量來做為貼圖座標
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			default:
				// 由外部控制
				break;
			}
		}
	}

	glActiveTexture(GL_TEXTURE0_ARB);
	glClientActiveTexture(GL_TEXTURE0_ARB);
}

void CGutModel_OpenGL::Release(void)
{
	if ( m_pMeshArray )
	{
		delete [] m_pMeshArray;
		m_pMeshArray = NULL;
	}

	if ( m_pMaterialArray )
	{
		for ( int i=0; i<m_iNumMaterials; i++ )
		{
		}

		delete [] m_pMaterialArray;
		m_pMaterialArray = NULL;
	}
}

bool CGutModel_OpenGL::ConvertToOpenGLModel(CGutModel *pModel, void (*CopyVertexBuffer)(void *target, sModelVertex *source, int num_vertices) )
{
	if ( pModel->m_iNumMeshes==0 )
		return false;
	int i,j,k;

	m_iNumMeshes = pModel->m_iNumMeshes;
	m_pMeshArray = new sModelMesh_OpenGL[m_iNumMeshes];

	void *vbuffer_pointer, *ibuffer_pointer;

	m_iNumMaterials = pModel->m_iNumMaterials;

	char szTextureName[256];

	if ( m_iNumMaterials )
	{
		m_pMaterialArray = new sModelMaterial_OpenGL[m_iNumMaterials];

		for ( i=0; i<m_iNumMaterials; i++ )
		{
			sModelMaterial_OpenGL *target = m_pMaterialArray + i;
			sModelMaterial *source = pModel->m_pMaterialArray + i;

			target->m_bCullFace = source->m_bCullFace;

			target->m_Emissive = source->m_Emissive;
			target->m_Ambient = source->m_Ambient;
			target->m_Diffuse = source->m_Diffuse;
			target->m_Specular = source->m_Specular;
			target->m_fShininess = source->m_fShininess;

			for ( j=0; j<MAX_NUM_TEXTURES; j++ )
			{
				if ( source->m_szTexture[j][0] )
				{
					sprintf(szTextureName, "%s%s", CGutModel::GetTexturePath(), source->m_szTexture[j]);
					target->m_Textures[j] = GutLoadTexture_OpenGL(szTextureName);
				}
				else
				{
					target->m_Textures[j] = 0;
				}
				target->m_MapChannel[j] = source->m_MapChannel[j];
			}
		}
	}

	for ( i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh *pMeshSource = pModel->m_pMeshArray + i;
		sModelMesh_OpenGL *pMesh = m_pMeshArray + i;

		pMesh->m_iNumVertexChunks = pMeshSource->m_iNumVertexChunks;
		pMesh->m_pVertexChunk = new sModelVertexChunk_OpenGL[pMesh->m_iNumVertexChunks];

		for ( j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk *pVertexChunkTarget = &pMeshSource->m_pVertexChunks[j];
			sModelVertexChunk_OpenGL *pVertexChunk = pMesh->m_pVertexChunk + j;
			pVertexChunk->m_VertexDecl = pVertexChunkTarget->m_VertexDecl;

			pVertexChunk->m_iNumVertices = pVertexChunkTarget->m_iNumVertices;
			int vbuffer_size = pVertexChunk->m_iNumVertices * 24;
			// allocate opengl vertex buffer object
			glGenBuffers(1, &pVertexChunk->m_VertexBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, pVertexChunk->m_VertexBufferID);
			glBufferData(GL_ARRAY_BUFFER, vbuffer_size, NULL, GL_STATIC_COPY);
			vbuffer_pointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			CopyVertexBuffer(vbuffer_pointer, pVertexChunkTarget->m_pVertexArray, pVertexChunk->m_iNumVertices);
			glUnmapBuffer(GL_ARRAY_BUFFER);

			pVertexChunk->m_iNumIndices = pVertexChunkTarget->m_iNumIndices;
			int ibuffer_size = pVertexChunk->m_iNumIndices * 2;
			// allocate opengl index buffer object
			glGenBuffers(1, &pVertexChunk->m_IndexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pVertexChunk->m_IndexBufferID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibuffer_size, NULL, GL_STATIC_COPY);
			ibuffer_pointer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
			memcpy(ibuffer_pointer, pVertexChunkTarget->m_pIndexArray, ibuffer_size);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

			pVertexChunk->m_iNumBatches = pVertexChunkTarget->m_iNumBatches;
			if ( pVertexChunk->m_iNumBatches )
			{
				pVertexChunk->m_pBatchArray = new sModelBatch[pVertexChunk->m_iNumBatches];
				memcpy(pVertexChunk->m_pBatchArray, pVertexChunkTarget->m_pBatchArray, 
					sizeof(sModelBatch) * pVertexChunk->m_iNumBatches);
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool CGutModel_OpenGL::ConvertToOpenGLModel(CGutModel *pModel)
{
	if ( pModel->m_iNumMeshes==0 )
		return false;
	int i,j,k;

	m_iNumMaterials = pModel->m_iNumMaterials;
	char szTextureName[256];

	if ( m_iNumMaterials )
	{
		m_pMaterialArray = new sModelMaterial_OpenGL[m_iNumMaterials];

		sModelMaterial_OpenGL *target = m_pMaterialArray;
		sModelMaterial *source = pModel->m_pMaterialArray;

		for ( i=0; i<m_iNumMaterials; i++, target++, source++ )
		{

			target->m_bCullFace = source->m_bCullFace;

			target->m_Emissive = source->m_Emissive;
			target->m_Ambient = source->m_Ambient;
			target->m_Diffuse = source->m_Diffuse;
			target->m_Specular = source->m_Specular;
			target->m_fShininess = source->m_fShininess;

			if ( !strcmp(source->m_szBlendMode, "replace") )
			{
				target->m_bBlend = false;
			}
			else
			{
				target->m_bBlend = true;

				if ( !strcmp(source->m_szBlendMode, "blend") )
				{
					target->m_SrcBlend = GL_SRC_ALPHA;
					target->m_DestBlend = GL_ONE_MINUS_SRC_ALPHA;
				}
				else if ( !strcmp(source->m_szBlendMode, "subtrace") )
				{
					target->m_SrcBlend = GL_ONE;
					target->m_DestBlend = GL_ONE;
				}
				else if ( !strcmp(source->m_szBlendMode, "add") )
				{
					target->m_SrcBlend = GL_ONE;
					target->m_DestBlend = GL_ONE;
				}
				else
				{
					target->m_SrcBlend = GL_ONE;
					target->m_DestBlend = GL_ZERO;
					target->m_bBlend = false;
				}
			}

			for ( j=0; j<MAX_NUM_TEXTURES; j++ )
			{
				if ( source->m_szTexture[j][0] )
				{
					sprintf(szTextureName, "%s%s", CGutModel::GetTexturePath(), source->m_szTexture[j]);
					target->m_Textures[j] = GutLoadTexture_OpenGL(szTextureName);
				}
				else
				{
					target->m_Textures[j] = 0;
				}
				target->m_MapChannel[j] = source->m_MapChannel[j];
			}

		}
	}

	m_iNumMeshes = pModel->m_iNumMeshes;
	m_pMeshArray = new sModelMesh_OpenGL[m_iNumMeshes];


	void *vbuffer_pointer, *ibuffer_pointer;

	for ( i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh *pMeshSource = pModel->m_pMeshArray + i;
		sModelMesh_OpenGL *pMesh = m_pMeshArray + i;

		pMesh->m_iNumVertexChunks = pMeshSource->m_iNumVertexChunks;
		pMesh->m_pVertexChunk = new sModelVertexChunk_OpenGL[pMesh->m_iNumVertexChunks];

		for ( j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk *pVertexChunkTarget = &pMeshSource->m_pVertexChunks[j];
			sModelVertexChunk_OpenGL *pVertexChunk = pMesh->m_pVertexChunk + j;
			pVertexChunk->m_VertexDecl = pVertexChunkTarget->m_VertexDecl;

			pVertexChunk->m_iNumVertices = pVertexChunkTarget->m_iNumVertices;
			int vbuffer_size = pVertexChunk->m_iNumVertices * pVertexChunk->m_VertexDecl.m_iVertexSize;
			// allocate opengl vertex buffer object
			glGenBuffers(1, &pVertexChunk->m_VertexBufferID);
			glBindBuffer(GL_ARRAY_BUFFER, pVertexChunk->m_VertexBufferID);
			glBufferData(GL_ARRAY_BUFFER, vbuffer_size, NULL, GL_STATIC_COPY);
			vbuffer_pointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
			pVertexChunkTarget->OutputVertexBuffer(vbuffer_pointer);
			glUnmapBuffer(GL_ARRAY_BUFFER);

			pVertexChunk->m_iNumIndices = pVertexChunkTarget->m_iNumIndices;
			int ibuffer_size = pVertexChunk->m_iNumIndices * 2;
			// allocate opengl index buffer object
			glGenBuffers(1, &pVertexChunk->m_IndexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pVertexChunk->m_IndexBufferID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibuffer_size, NULL, GL_STATIC_COPY);
			ibuffer_pointer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
			memcpy(ibuffer_pointer, pVertexChunkTarget->m_pIndexArray, ibuffer_size);
			glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);

			pVertexChunk->m_iNumBatches = pVertexChunkTarget->m_iNumBatches;
			if ( pVertexChunk->m_iNumBatches )
			{
				pVertexChunk->m_pBatchArray = new sModelBatch[pVertexChunk->m_iNumBatches];
				memcpy(pVertexChunk->m_pBatchArray, pVertexChunkTarget->m_pBatchArray, 
					sizeof(sModelBatch) * pVertexChunk->m_iNumBatches);
			}
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CGutModel_OpenGL::RenderMesh(bool bSubmitMaterial)
{
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	for ( int i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh_OpenGL *pMesh = m_pMeshArray + i;

		for ( int j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk_OpenGL *pVertexChunk = pMesh->m_pVertexChunk + j;
			sVertexDecl *pVertexDecl = &pVertexChunk->m_VertexDecl;

			glBindBuffer(GL_ARRAY_BUFFER, pVertexChunk->m_VertexBufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pVertexChunk->m_IndexBufferID);

			if ( pVertexDecl->m_iPositionOffset >= 0 )
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(pVertexDecl->m_iNumPositionElements, GL_FLOAT, pVertexDecl->m_iVertexSize, (GLvoid *) pVertexDecl->m_iPositionOffset);
			}
			else
			{
				glDisableClientState(GL_VERTEX_ARRAY);
			}

			if ( pVertexDecl->m_iNormalOffset >= 0 )
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT, pVertexDecl->m_iVertexSize, (GLvoid *) pVertexDecl->m_iNormalOffset);
			}
			else
			{
				glDisableClientState(GL_NORMAL_ARRAY);
			}

			if ( pVertexDecl->m_iColorOffset >= 0 )
			{
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(pVertexDecl->m_iNumColorElements, GL_UNSIGNED_BYTE, pVertexDecl->m_iVertexSize, (GLvoid *) pVertexDecl->m_iColorOffset);
			}
			else
			{
				glDisableClientState(GL_COLOR_ARRAY);
			}

			for (int l=0; l<pVertexChunk->m_iNumBatches; l++ )
			{
				sModelBatch *pBatch = pVertexChunk->m_pBatchArray + l;

				if ( bSubmitMaterial && pBatch->m_iMaterialID >= 0 )
				{
					m_pMaterialArray[pBatch->m_iMaterialID].Submit(pVertexChunk);
				}

				// OpenGL 1.1+
				//int *ip = (int *) (pBatch->m_iIndexArrayBegin * 2);
				//glDrawElements(GL_TRIANGLES, pBatch->m_iNumPrimitives * 3, GL_UNSIGNED_SHORT, ip);

				// OpenGL 1.2+
				glDrawRangeElements(GL_TRIANGLES, pBatch->m_iIndexArrayBegin, pBatch->m_iIndexArrayEnd, 
					pBatch->m_iNumIndices, GL_UNSIGNED_SHORT, 0);
			}

		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopClientAttrib();
}