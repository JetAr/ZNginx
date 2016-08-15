#include "GutDX9.h"
#include "GutModel.h"
#include "GutModel_DX9.h"
#include "GutTexture_DX9.h"

static DWORD g_cull_face = D3DCULL_CW;

int CGutModel_DX9::s_MapOverwrite[3] = {0xff, 0xff, 0xff};
DWORD CGutModel_DX9::s_CullFace = D3DCULL_CW;
sModelMaterial_DX9 *CGutModel_DX9::s_pMaterialOverwrite = NULL;
LPDIRECT3DBASETEXTURE9  CGutModel_DX9::s_pTextureOverwrite[3] = {NULL, NULL, NULL};
LPDIRECT3DPIXELSHADER9  CGutModel_DX9::s_pPixelShaderOverwrite = NULL;
LPDIRECT3DVERTEXSHADER9 CGutModel_DX9::s_pVertexShaderOverwrite = NULL;

void GutSetDX9BackFace(DWORD cullface)
{
	CGutModel_DX9::SetCullMode(cullface);	
}

void sModelMaterial_DX9::Submit(int mask)
{
	bool bSubmitCullFace = mask & SUBMIT_CULLFACE;
	bool bSubmitMtl = mask & SUBMIT_MTL;
	bool bSubmitShader = mask & SUBMIT_SHADER;
	bool bSubmitTexture = mask & SUBMIT_TEXTURES;
	bool bSubmitBlend = mask & SUBMIT_BLEND;

	LPDIRECT3DDEVICE9 pDevice = GutGetGraphicsDeviceDX9();

	LPDIRECT3DVERTEXSHADER9 pVertexShader = CGutModel_DX9::s_pVertexShaderOverwrite ? 
		CGutModel_DX9::s_pVertexShaderOverwrite : m_pVertexShader;
	LPDIRECT3DPIXELSHADER9 pPixelShader = CGutModel_DX9::s_pPixelShaderOverwrite ?
		CGutModel_DX9::s_pPixelShaderOverwrite : m_pPixelShader;

	if ( bSubmitShader )
	{
		pDevice->SetVertexShader(pVertexShader);
		pDevice->SetPixelShader(pPixelShader);
	}


	if ( bSubmitCullFace )
	{
		pDevice->SetRenderState(D3DRS_CULLMODE, 
			m_bCullFace ? CGutModel_DX9::s_CullFace : D3DCULL_NONE);
	}

	if ( bSubmitBlend )
	{
		pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, m_bBlend);

		if ( m_bBlend )
		{
			pDevice->SetRenderState(D3DRS_SRCBLEND, m_SrcBlend);
			pDevice->SetRenderState(D3DRS_DESTBLEND, m_DestBlend);
		}
	}

	if ( bSubmitMtl )
	{
		if ( NULL==pPixelShader )
		{
			pDevice->SetMaterial(&m_Material);
		}
	}

	static DWORD texblend[] = 
	{
		D3DTOP_MODULATE,
		D3DTOP_MODULATE,
		D3DTOP_ADD
	};

	int tex_stage = 0;

	for ( int t=0; t<MAX_NUM_TEXTURES; t++ )
	{
		LPDIRECT3DBASETEXTURE9 pTex = NULL;

		if ( CGutModel_DX9::s_pTextureOverwrite[t] )
		{
			pTex = CGutModel_DX9::s_pTextureOverwrite[t];
		}
		else
		{
			pTex = m_pTextures[t];
		}

		if ( bSubmitTexture )
		{
			pDevice->SetTexture(tex_stage, pTex);
		}

		if ( pTex==NULL )
		{
			pDevice->SetTextureStageState(t, D3DTSS_COLOROP, D3DTOP_DISABLE);
			continue;
		}

		if ( bSubmitMtl )
		{
			// 沒有使用Pixel Shader時才需要套用multi-texture混色設定.
			if ( NULL==pPixelShader )
			{
				pDevice->SetTextureStageState(tex_stage, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				pDevice->SetTextureStageState(tex_stage, D3DTSS_COLORARG2, D3DTA_CURRENT);
				pDevice->SetTextureStageState(tex_stage, D3DTSS_COLOROP, texblend[t]);

				int MapChannel = CGutModel_DX9::s_MapOverwrite[t]==0xff ? m_MapChannel[t] : CGutModel_DX9::s_MapOverwrite[t];

				switch(MapChannel)
				{
					// sphere map
				case MAP_SPHEREMAP:
					pDevice->SetTextureStageState(tex_stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
					pDevice->SetTextureStageState(tex_stage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_SPHEREMAP);
					break;
					// cube map
				case MAP_CUBEMAP:
					pDevice->SetTextureStageState(tex_stage, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
					//pDevice->SetTextureStageState(tex_stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
					break;
					// pass through
				default:
					pDevice->SetTextureStageState(tex_stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
					pDevice->SetTextureStageState(tex_stage,  D3DTSS_TEXCOORDINDEX, MapChannel);
					break;
				}
			}
			else
			{
				pDevice->SetTextureStageState(tex_stage, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
				pDevice->SetTextureStageState(tex_stage, D3DTSS_TEXCOORDINDEX, tex_stage);
			}
		}

		tex_stage++;
	}

	// no textures
	if ( 0==tex_stage && NULL==pPixelShader && bSubmitMtl )
	{
		DWORD lighting;
		pDevice->GetRenderState(D3DRS_LIGHTING, &lighting);

		if ( lighting )
		{
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
		}
		else
		{
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
			pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);

			int r = m_Material.Diffuse.r * 255;
			int g = m_Material.Diffuse.g * 255;
			int b = m_Material.Diffuse.b * 255;
			int a = m_Material.Diffuse.a * 255;

			pDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_RGBA(r,g,b,a));
		}
	}
}

LPDIRECT3DVERTEXDECLARATION9 sModelVertexChunk_DX9::CreateVertexDecl(sVertexDecl *pVertexDecl)
{
	return NULL;
}

DWORD sModelVertexChunk_DX9::CreateFVF(sVertexDecl *pVertexDecl)
{
	DWORD fvf = 0;

	if ( pVertexDecl->m_iPositionOffset >= 0 )
	{
		fvf |= D3DFVF_XYZ;
	}

	if ( pVertexDecl->m_iNormalOffset >= 0 )
	{
		fvf |= D3DFVF_NORMAL;
	}

	if ( pVertexDecl->m_iColorOffset >= 0 )
	{
		fvf |= D3DFVF_DIFFUSE;
	}

	DWORD textable[] = {
		D3DFVF_TEX0, 
		D3DFVF_TEX1,
		D3DFVF_TEX2, 
		D3DFVF_TEX3,
		D3DFVF_TEX4, 
		D3DFVF_TEX5,
		D3DFVF_TEX6, 
		D3DFVF_TEX7,
		D3DFVF_TEX8 };

		int num_texcoords = 0;
		for ( int t=0; t<MAX_TEXCOORDS; t++ )
		{
			if ( pVertexDecl->m_iTexcoordOffset[t] >=0 )
			{
				num_texcoords++;
			}
		}

		if ( num_texcoords )
		{
			fvf |= textable[num_texcoords];
		}

		if ( pVertexDecl->m_iTangentOffset >=0 )
		{
			fvf |= D3DFVF_TEX2;
			fvf |= D3DFVF_TEXCOORDSIZE3(1);
			fvf |= D3DFVF_TEXCOORDSIZE3(2);
		}
		else
		{
			fvf |= textable[num_texcoords];
		}

		m_FVF = fvf;

		return fvf;
}

void CGutModel_DX9::Release(void)
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


bool CGutModel_DX9::ConvertToDX9Model(CGutModel *pModel)
{
	if ( pModel->m_iNumMeshes==0 )
		return false;

	printf("Building model for Direct3D9\n");

	LPDIRECT3DDEVICE9 pDevice = GutGetGraphicsDeviceDX9();

	int i, j;

	m_iNumMaterials = pModel->m_iNumMaterials;
	char szTextureName[256];

	if ( m_iNumMaterials )
	{
		m_pMaterialArray = new sModelMaterial_DX9[m_iNumMaterials];

		for ( i=0; i<m_iNumMaterials; i++ )
		{
			sModelMaterial_DX9 *target = m_pMaterialArray + i;
			sModelMaterial *source = pModel->m_pMaterialArray + i;

			D3DMATERIAL9 *pMtl = &m_pMaterialArray[i].m_Material;

			pMtl->Ambient = *(D3DCOLORVALUE *) &source->m_Ambient;
			pMtl->Diffuse = *(D3DCOLORVALUE *) &source->m_Diffuse;
			pMtl->Specular = *(D3DCOLORVALUE *) &source->m_Specular;
			pMtl->Emissive = *(D3DCOLORVALUE *) &source->m_Emissive;
			pMtl->Power = source->m_fShininess;

			target->m_bCullFace = source->m_bCullFace;

			if ( !strcmp(source->m_szBlendMode, "replace") )
			{
				target->m_bBlend = FALSE;
			}
			else
			{
				target->m_bBlend = TRUE;

				if ( !strcmp(source->m_szBlendMode, "blend") )
				{
					target->m_SrcBlend = D3DBLEND_SRCALPHA;
					target->m_DestBlend = D3DBLEND_INVSRCALPHA;
				}
				else if ( !strcmp(source->m_szBlendMode, "add") )
				{
					target->m_SrcBlend = D3DBLEND_ONE;
					target->m_DestBlend = D3DBLEND_ONE;
				}
				else
				{
					target->m_SrcBlend = D3DBLEND_ONE;
					target->m_DestBlend = D3DBLEND_ZERO;
					target->m_bBlend = FALSE;
				}
			}

			for ( j=0; j<MAX_NUM_TEXTURES; j++ )
			{
				if ( source->m_szTexture[j][0] )
				{
					sprintf(szTextureName, "%s%s", CGutModel::GetTexturePath(), source->m_szTexture[j]);
					if ( source->m_MapChannel[j]==MAP_CUBEMAP )
					{
						target->m_pTextures[j] = GutLoadCubemapTexture_DX9(szTextureName);
					}
					else
					{
						target->m_pTextures[j] = GutLoadTexture_DX9(szTextureName);
					}
				}
				else
				{
					target->m_pTextures[j] = NULL;
				}
				target->m_MapChannel[j] = source->m_MapChannel[j];
			}

		}
	}

	m_iNumMeshes = pModel->m_iNumMeshes;
	m_pMeshArray = new sModelMesh_DX9[m_iNumMeshes];

	void *vbuffer_pointer, *ibuffer_pointer;

	for ( i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh *pMeshSource = pModel->m_pMeshArray + i;
		sModelMesh_DX9 *pMesh = m_pMeshArray + i;

		pMesh->m_iNumVertexChunks = pMeshSource->m_iNumVertexChunks;
		pMesh->m_pVertexChunk = new sModelVertexChunk_DX9[pMesh->m_iNumVertexChunks];

		for ( j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk *pVertexChunkTarget = &pMeshSource->m_pVertexChunks[j];
			sModelVertexChunk_DX9 *pVertexChunk = pMesh->m_pVertexChunk + j;
			sVertexDecl *pVertexDecl = &pVertexChunkTarget->m_VertexDecl;

			pVertexChunk->m_iVertexSize = pVertexChunkTarget->m_VertexDecl.m_iVertexSize;
			pVertexChunk->CreateFVF(pVertexDecl);

			pVertexChunk->m_iNumVertices = pVertexChunkTarget->m_iNumVertices;
			int vbuffer_size = pVertexChunk->m_iNumVertices * pVertexChunk->m_iVertexSize;
			// 配置一塊顯示卡記憶體
			pDevice->CreateVertexBuffer(vbuffer_size, 0, 0, D3DPOOL_MANAGED, &pVertexChunk->m_pVertexBuffer, NULL);
			if ( D3D_OK==pVertexChunk->m_pVertexBuffer->Lock(0, vbuffer_size, &vbuffer_pointer, 0) )
			{
				// 把頂點資料從主記憶體拷具到顯示卡上
				pVertexChunkTarget->OutputVertexBuffer(vbuffer_pointer);
				pVertexChunk->m_pVertexBuffer->Unlock();
			}
			else
			{
				return false;
			}

			pVertexChunk->m_iNumIndices = pVertexChunkTarget->m_iNumIndices;
			int ibuffer_size = pVertexChunk->m_iNumIndices * 2;
			// 配置一塊顯示卡記憶體
			pDevice->CreateIndexBuffer(ibuffer_size, 0, D3DFMT_INDEX16, D3DPOOL_MANAGED, &pVertexChunk->m_pIndexBuffer, NULL);
			if ( D3D_OK==pVertexChunk->m_pIndexBuffer->Lock(0, ibuffer_size, &ibuffer_pointer, 0) )
			{
				// 把索引資料從主記憶體拷具到顯示卡上
				memcpy(ibuffer_pointer, pVertexChunkTarget->m_pIndexArray, ibuffer_size);
				pVertexChunk->m_pIndexBuffer->Unlock();
			}
			else
			{
				return false;
			}

			// 
			pVertexChunk->m_iNumBatches = pVertexChunkTarget->m_iNumBatches;
			if ( pVertexChunk->m_iNumBatches )
			{
				pVertexChunk->m_pBatchArray = new sModelBatch[pVertexChunk->m_iNumBatches];
				memcpy(pVertexChunk->m_pBatchArray, pVertexChunkTarget->m_pBatchArray, sizeof(sModelBatch) * pVertexChunk->m_iNumBatches);
			}
		}
	}

	return true;
}

// submit meshes to GPU, do not apply material
void CGutModel_DX9::RenderMesh(bool submitmaterial)
{
	Render(submitmaterial ? 0xff : 0);
}

void CGutModel_DX9::Render(int mask)
{
	LPDIRECT3DDEVICE9 pDevice = GutGetGraphicsDeviceDX9();

	bool bSubMtl = mask ? true : false;

	for ( int i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh_DX9 *pMesh = m_pMeshArray + i;

		for ( int j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk_DX9 *pVertexChunk = pMesh->m_pVertexChunk + j;
			pDevice->SetFVF(pVertexChunk->m_FVF);

			pDevice->SetStreamSource(0, pVertexChunk->m_pVertexBuffer, 0, pVertexChunk->m_iVertexSize);
			pDevice->SetIndices(pVertexChunk->m_pIndexBuffer);

			for (int l=0; l<pVertexChunk->m_iNumBatches; l++ )
			{
				sModelBatch *pBatch = pVertexChunk->m_pBatchArray + l;

				if ( bSubMtl )
				{
					if ( s_pMaterialOverwrite )
					{
						s_pMaterialOverwrite->Submit(mask);
					}
					else if ( m_pMaterialArray )
					{
						if ( pBatch->m_iMaterialID>=0 )
							m_pMaterialArray[pBatch->m_iMaterialID].Submit(mask);
						else
							m_pMaterialArray[0].Submit(mask);
					}
				}

				pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, pVertexChunk->m_iNumVertices, pBatch->m_iIndexArrayBegin, pBatch->m_iNumPrimitives);
			}
		}
	}
}