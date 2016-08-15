#include <windows.h>

#include "glew.h"
#include "wglew.h"
#include <GL/gl.h>

#include "Gut.h"
#include "GutOpenGL.h"
#include "GutModel_OpenGL.h"
#include "GutTexture_OpenGL.h"

sModelMaterial_OpenGL *CGutModel_OpenGL::s_pMaterialOverwrite = NULL;
GLuint CGutModel_OpenGL::s_ShaderOverwrite = 0;
GLuint CGutModel_OpenGL::s_TextureOverwrite[MAX_NUM_TEXTURES] = {0,0,0};
GLuint CGutModel_OpenGL::s_MapOverwrite[MAX_NUM_TEXTURES] = {MAP_NOOVERWRITE, MAP_NOOVERWRITE, MAP_NOOVERWRITE};

sModelVertexChunk_OpenGL::~sModelVertexChunk_OpenGL()
{
	if ( m_VertexBufferID )
	{
		glDeleteBuffers(1, &m_VertexBufferID);
		m_VertexBufferID = 0;
	}

	if ( m_IndexBufferID )
	{
		glDeleteBuffers(1, &m_IndexBufferID);
		m_IndexBufferID = 0;
	}

	if ( m_pVertexBuffer )
	{
		delete [] m_pVertexBuffer;
		m_pVertexBuffer = NULL;
	}
	if ( m_pIndexBuffer )
	{
		delete [] m_pIndexBuffer;
		m_pIndexBuffer = NULL;
	}
}

void sModelMaterial_OpenGL::Release(void)
{
	for ( int i=0; i<MAX_NUM_TEXTURES; i++ )
	{
		if ( m_Textures[i] )
		{
			glDeleteTextures(1, &m_Textures[i]);
			m_Textures[i] = 0;
		}
	}
}

void sModelMaterial_OpenGL::Submit(sModelVertexChunk_OpenGL *pVChunk, UINT mask)
{
	bool bSubmitCullFace = mask & SUBMIT_CULLFACE;
	bool bSubmitMtl = mask & SUBMIT_MTL;
	bool bSubmitShader = mask & SUBMIT_SHADER;
	bool bSubmitTexture = mask & SUBMIT_TEXTURES;
	bool bSubmitBlendMode = mask & SUBMIT_BLEND;

	if ( bSubmitMtl )
	{
		glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, &m_Emissive[0]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, &m_Ambient[0]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, &m_Diffuse[0]);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, &m_Specular[0]);
		glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, m_fShininess);
	}

	if ( bSubmitCullFace )
	{
		if ( m_bCullFace )
		{
			glEnable(GL_CULL_FACE);
		}
		else
		{
			glDisable(GL_CULL_FACE);
		}
	}

	if ( bSubmitBlendMode )
	{
		if ( m_bBlend )
		{
			glEnable(GL_BLEND);
			glBlendFunc(m_SrcBlend, m_DestBlend);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	static GLuint texblend[] = 
	{
		GL_MODULATE,
		GL_ADD,
		GL_ADD
	};

	int tex_stage = 0;

	unsigned int vmem = 0;

	if ( pVChunk )
	{
		if ( !(glBindBuffer && pVChunk->m_VertexBufferID) )
		{
			// 沒有支援 Vertex/Index buffer object, 資料在主記體中.
			vmem = (unsigned int) pVChunk->m_pVertexBuffer;
		}
	}

	for ( int t=0; t<MAX_NUM_TEXTURES; t++ )
	{
		int map = CGutModel_OpenGL::s_MapOverwrite[t]==MAP_NOOVERWRITE ? m_MapChannel[t] : CGutModel_OpenGL::s_MapOverwrite[t];
		GLuint texid = CGutModel_OpenGL::s_TextureOverwrite[t] ? CGutModel_OpenGL::s_TextureOverwrite[t] : m_Textures[t];

		GLuint stage;

		if ( texid==0 )
		{
			stage = GL_TEXTURE0_ARB + t;

			glActiveTexture(stage);
			glClientActiveTexture(stage);
			glDisable(GL_TEXTURE_2D);
			glDisable(GL_TEXTURE_CUBE_MAP);
			glDisableClientState(GL_TEXTURE_COORD_ARRAY);
			continue;
		}

		stage = GL_TEXTURE0_ARB + tex_stage;

		glActiveTexture(stage);
		glClientActiveTexture(stage);

		switch(map)
		{
		default: // pass though UV
			if ( bSubmitTexture )
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texid);

				if ( bSubmitShader && glUniform1i )
				{	// only a hack
					glUniform1i(tex_stage, tex_stage);
				}
			}
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texblend[t]);

			glDisable(GL_TEXTURE_GEN_S);
			glDisable(GL_TEXTURE_GEN_T);
			glDisable(GL_TEXTURE_GEN_R);
			glDisable(GL_TEXTURE_GEN_Q);

			if ( pVChunk && map<MAX_NUM_TEXTURES && map>=0 )
			{
				sVertexDecl *pVertexDecl = &pVChunk->m_VertexDecl;
				if ( pVertexDecl->m_iTexcoordOffset[map] >= 0 )
				{
					glEnableClientState(GL_TEXTURE_COORD_ARRAY);
					glTexCoordPointer(pVertexDecl->m_iNumTexcoordElements[map], GL_FLOAT, pVertexDecl->m_iVertexSize, (GLvoid *) (vmem + pVertexDecl->m_iTexcoordOffset[map]) );
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
			break;

		case MAP_SPHEREMAP: // sphere map
			if ( bSubmitTexture )
			{
				glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, texid);

				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texblend[t]);

				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);
				// 自動產生spheremap貼圖座標
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);
			}
			break;

		case MAP_CUBEMAP: // cube map
			if ( bSubmitTexture )
			{
				glEnable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, texid);

				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texblend[t]);

				// S/T/R分別代表貼圖座標的X/Y/Z軸
				glEnable(GL_TEXTURE_GEN_S);
				glEnable(GL_TEXTURE_GEN_T);
				glEnable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_GEN_Q);
				// 以光線的反射向量來做為貼圖座標
				glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
				glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
			}
			break;
		}

		tex_stage++;
	}

	if ( pVChunk && pVChunk->m_VertexDecl.m_iTangentOffset>=0 )
	{
		// 為了簡化, tangent space 向量固定放在 texcoord1 & texcoord2

		sVertexDecl *pVertexDecl = &pVChunk->m_VertexDecl;

		glClientActiveTexture(GL_TEXTURE1_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3, GL_FLOAT, pVertexDecl->m_iVertexSize, (GLvoid *) (vmem + pVertexDecl->m_iTangentOffset) );

		glClientActiveTexture(GL_TEXTURE2_ARB);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);
		glTexCoordPointer(3, GL_FLOAT, pVertexDecl->m_iVertexSize, (GLvoid *) (vmem + pVertexDecl->m_iBiNormalOffset) );
	}

	if ( tex_stage==0 )
	{
		glColor4fv(&m_Diffuse[0]);
	}
	else
	{
		glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	}

	GLuint shader = CGutModel_OpenGL::s_ShaderOverwrite;
	if ( shader && bSubmitShader && glUseProgram )
	{
		glUseProgram(shader);
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
			m_pMaterialArray[i].Release();
		}

		delete [] m_pMaterialArray;
		m_pMaterialArray = NULL;
	}
}

bool CGutModel_OpenGL::ConvertToOpenGLModel(CGutModel *pModel)
{
	if ( pModel->m_iNumMeshes==0 )
		return false;

	printf("Building model for OpenGL\n");

	int i,j,k;

	m_iNumMaterials = pModel->m_iNumMaterials;
	char szTextureName[256];

	if ( m_iNumMaterials )
	{
		m_pMaterialArray = new sModelMaterial_OpenGL[m_iNumMaterials];
		if ( NULL==m_pMaterialArray )
			return false;

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
					if ( source->m_MapChannel[j]==MAP_CUBEMAP )
					{
						target->m_Textures[j] = GutLoadCubemapTexture_OpenGL(szTextureName);
					}
					else
					{
						target->m_Textures[j] = GutLoadTexture_OpenGL(szTextureName);
					}
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
	if ( NULL==m_pMeshArray )
		return false;

	// test
	// glGenBuffers = NULL;
	// glBindBuffer = NULL;

	void *vbuffer_pointer, *ibuffer_pointer;

	for ( i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh *pMeshSource = pModel->m_pMeshArray + i;
		sModelMesh_OpenGL *pMesh = m_pMeshArray + i;

		pMesh->m_iNumVertexChunks = pMeshSource->m_iNumVertexChunks;
		pMesh->m_pVertexChunk = new sModelVertexChunk_OpenGL[pMesh->m_iNumVertexChunks];
		if ( NULL==pMesh->m_pVertexChunk )
			return false;

		for ( j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk *pVertexChunkTarget = &pMeshSource->m_pVertexChunks[j];
			sModelVertexChunk_OpenGL *pVertexChunk = pMesh->m_pVertexChunk + j;
			pVertexChunk->m_VertexDecl = pVertexChunkTarget->m_VertexDecl;

			pVertexChunk->m_iNumVertices = pVertexChunkTarget->m_iNumVertices;
			int vbuffer_size = pVertexChunk->m_iNumVertices * pVertexChunk->m_VertexDecl.m_iVertexSize;

			pVertexChunk->m_iNumIndices = pVertexChunkTarget->m_iNumIndices;
			int ibuffer_size = pVertexChunk->m_iNumIndices * 2;

			// allocate opengl vertex buffer object
			if ( glGenBuffers && glBindBuffer )
			{
				// create opengl vertex buffer object
				glGenBuffers(1, &pVertexChunk->m_VertexBufferID);
				glBindBuffer(GL_ARRAY_BUFFER, pVertexChunk->m_VertexBufferID);
				glBufferData(GL_ARRAY_BUFFER, vbuffer_size, NULL, GL_STATIC_COPY);

				// copy vertex array
				vbuffer_pointer = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				pVertexChunkTarget->OutputVertexBuffer(vbuffer_pointer);
				glUnmapBuffer(GL_ARRAY_BUFFER);

				// create opengl index buffer object
				glGenBuffers(1, &pVertexChunk->m_IndexBufferID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pVertexChunk->m_IndexBufferID);
				glBufferData(GL_ELEMENT_ARRAY_BUFFER, ibuffer_size, NULL, GL_STATIC_COPY);

				// copy index array
				ibuffer_pointer = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
				memcpy(ibuffer_pointer, pVertexChunkTarget->m_pIndexArray, ibuffer_size);
				glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			}
			else
			{
				// copy vertex array
				pVertexChunk->m_pVertexBuffer = new unsigned char [vbuffer_size];
				pVertexChunkTarget->OutputVertexBuffer(pVertexChunk->m_pVertexBuffer);

				// copy index array
				pVertexChunk->m_pIndexBuffer = new unsigned short[pVertexChunk->m_iNumIndices];
				memcpy(pVertexChunk->m_pIndexBuffer, pVertexChunkTarget->m_pIndexArray, ibuffer_size);
			}

			pVertexChunk->m_iNumBatches = pVertexChunkTarget->m_iNumBatches;
			if ( pVertexChunk->m_iNumBatches )
			{
				pVertexChunk->m_pBatchArray = new sModelBatch[pVertexChunk->m_iNumBatches];
				if ( NULL==pVertexChunk->m_pBatchArray )
					return false;
				memcpy(pVertexChunk->m_pBatchArray, pVertexChunkTarget->m_pBatchArray, 
					sizeof(sModelBatch) * pVertexChunk->m_iNumBatches);
			}
		}
	}

	if ( glBindBuffer )
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	return true;
}

void CGutModel_OpenGL::RenderMesh(bool bSubmitMaterial)
{
	Render(bSubmitMaterial ? 0xff : 0);
}

void CGutModel_OpenGL::Render(UINT mask)
{
	glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);

	bool bSubMtl = mask ? true : false;

	for ( int i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh_OpenGL *pMesh = m_pMeshArray + i;

		for ( int j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk_OpenGL *pVertexChunk = pMesh->m_pVertexChunk + j;
			sVertexDecl *pVertexDecl = &pVertexChunk->m_VertexDecl;

			unsigned int vmem = 0;
			unsigned int imem = 0;

			if ( glBindBuffer && pVertexChunk->m_VertexBufferID && pVertexChunk->m_IndexBufferID )
			{
				// 有支援 Vertex/Index buffer object
				glBindBuffer(GL_ARRAY_BUFFER, pVertexChunk->m_VertexBufferID);
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pVertexChunk->m_IndexBufferID);
			}
			else
			{
				// 沒有支援 Vertex/Index buffer object, 資料在主記體中.
				vmem = (unsigned int) pVertexChunk->m_pVertexBuffer;
				imem = (unsigned int) pVertexChunk->m_pIndexBuffer;
			}

			if ( pVertexDecl->m_iPositionOffset >= 0 )
			{
				glEnableClientState(GL_VERTEX_ARRAY);
				glVertexPointer(pVertexDecl->m_iNumPositionElements, GL_FLOAT, pVertexDecl->m_iVertexSize, (GLvoid *) (vmem + pVertexDecl->m_iPositionOffset) );
			}
			else
			{
				glDisableClientState(GL_VERTEX_ARRAY);
			}

			if ( pVertexDecl->m_iNormalOffset >= 0 )
			{
				glEnableClientState(GL_NORMAL_ARRAY);
				glNormalPointer(GL_FLOAT, pVertexDecl->m_iVertexSize, (GLvoid *) (vmem + pVertexDecl->m_iNormalOffset) );
			}
			else
			{
				glDisableClientState(GL_NORMAL_ARRAY);
			}

			if ( pVertexDecl->m_iColorOffset >= 0 )
			{
				glEnableClientState(GL_COLOR_ARRAY);
				glColorPointer(pVertexDecl->m_iNumColorElements, GL_UNSIGNED_BYTE, pVertexDecl->m_iVertexSize, (GLvoid *) (vmem + pVertexDecl->m_iColorOffset) );
			}
			else
			{
				glDisableClientState(GL_COLOR_ARRAY);
			}

			for (int l=0; l<pVertexChunk->m_iNumBatches; l++ )
			{
				sModelBatch *pBatch = pVertexChunk->m_pBatchArray + l;

				if ( bSubMtl )
				{
					if ( s_pMaterialOverwrite )
					{
						s_pMaterialOverwrite->Submit(pVertexChunk);
					}
					else if ( m_pMaterialArray )
					{
						if ( pBatch->m_iMaterialID >=0 )
						{
							m_pMaterialArray[pBatch->m_iMaterialID].Submit(pVertexChunk, mask);
						}
						else
						{
							m_pMaterialArray[0].Submit(pVertexChunk, mask);
						}
					}

				}

				// OpenGL 1.1+
				unsigned int *ip = (unsigned int *) (imem + pBatch->m_iIndexArrayBegin * 2);
				glDrawElements(GL_TRIANGLES, pBatch->m_iNumPrimitives * 3, GL_UNSIGNED_SHORT, ip);

				// OpenGL 1.2+
				//glDrawRangeElements(GL_TRIANGLES, pBatch->m_iIndexArrayBegin, pBatch->m_iIndexArrayEnd, 
				//	pBatch->m_iNumIndices, GL_UNSIGNED_SHORT, 0);
			}

		}
	}

	if ( glBindBuffer )
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	glPopClientAttrib();
}