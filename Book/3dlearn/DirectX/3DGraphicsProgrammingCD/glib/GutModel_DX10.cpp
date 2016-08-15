#ifdef _ENABLE_DX10_

#include "GutDX10.h"
#include "GutModel_DX10.h"
#include "GutTexture_DX10.h"

ID3D10VertexShader	*CGutModel_DX10::s_pVertexShader[SHADER_PERMUTATIONS];
ID3D10PixelShader	*CGutModel_DX10::s_pPixelShader[SHADER_PERMUTATIONS];
//
ID3D10Blob			*CGutModel_DX10::s_pVertexShaderCode[SHADER_PERMUTATIONS];
// shader constants
ID3D10Buffer		*CGutModel_DX10::s_pMatrixConstantBuffer = NULL;
ID3D10Buffer		*CGutModel_DX10::s_pMtlConstantBuffer = NULL;
ID3D10Buffer		*CGutModel_DX10::s_pLightConstantBuffer = NULL;

ID3D10BlendState	*CGutModel_DX10::s_pBlend_Add = NULL;
ID3D10BlendState	*CGutModel_DX10::s_pBlend_Blend = NULL;
ID3D10BlendState	*CGutModel_DX10::s_pBlend_Replace = NULL;

ID3D10RasterizerState *CGutModel_DX10::s_pCull_CW = NULL;
ID3D10RasterizerState *CGutModel_DX10::s_pCull_CCW = NULL;
ID3D10RasterizerState *CGutModel_DX10::s_pCull_NONE = NULL;

ID3D10RasterizerState *CGutModel_DX10::s_pCull = NULL;
ID3D10RasterizerState *CGutModel_DX10::s_pNoCull = NULL;

int CGutModel_DX10::s_shader_overwrite = -1;

sModelMaterial_DX10 *CGutModel_DX10::s_pMaterialOverwrite = NULL;

sLightConstants CGutModel_DX10::s_light; 
bool CGutModel_DX10::s_bLighting = false;

// fixed pipeline matrix
Matrix4x4 CGutModel_DX10::s_proj_matrix;
Matrix4x4 CGutModel_DX10::s_view_matrix;
Matrix4x4 CGutModel_DX10::s_inv_view_matrix;
Matrix4x4 CGutModel_DX10::s_world_matrix;
Matrix4x4 CGutModel_DX10::s_wvp_matrix;
Matrix4x4 CGutModel_DX10::s_texture_matrix[4];
Matrix4x4 CGutModel_DX10::s_light_wvp_matrix;
Matrix4x4 CGutModel_DX10::s_light_wv_matrix;

ID3D10ShaderResourceView *CGutModel_DX10::s_pTextureOverwrite[3] = {NULL, NULL, NULL};

bool CGutModel_DX10::s_bCullCW = true;

void sModelVertexChunk_DX10::CreateVertexDecl(sVertexDecl *pVertexDecl)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();

	D3D10_INPUT_ELEMENT_DESC layout[20];
	ZeroMemory(layout, sizeof(layout));

	int num_items = 0;
	int offset = 0;

	if ( pVertexDecl->m_iPositionOffset >= 0 )
	{
		layout[num_items].SemanticName = "POSITION";
		layout[num_items].SemanticIndex = 0;
		layout[num_items].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout[num_items].InputSlot = 0;
		layout[num_items].AlignedByteOffset = offset;
		layout[num_items].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[num_items].InstanceDataStepRate = 0;

		num_items++;
		offset += 12;
	}

	if ( pVertexDecl->m_iNormalOffset >= 0 )
	{
		layout[num_items].SemanticName = "NORMAL";
		layout[num_items].SemanticIndex = 0;
		layout[num_items].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout[num_items].InputSlot = 0;
		layout[num_items].AlignedByteOffset = offset;
		layout[num_items].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[num_items].InstanceDataStepRate = 0;

		num_items++;
		offset += 12;
	}

	if ( pVertexDecl->m_iColorOffset >= 0 )
	{
		layout[num_items].SemanticName = "COLOR";
		layout[num_items].SemanticIndex = 0;
		layout[num_items].Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		layout[num_items].InputSlot = 0;
		layout[num_items].AlignedByteOffset = offset;
		layout[num_items].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[num_items].InstanceDataStepRate = 0;

		num_items++;
		offset += 4;
	}

	int num_texcoords = 0;
	char buffer[MAX_TEXCOORDS][16];

	for ( int t=0; t<MAX_TEXCOORDS; t++ )
	{
		if ( pVertexDecl->m_iTexcoordOffset[t] >=0 )
		{
			layout[num_items].SemanticName = "TEXCOORD";
			layout[num_items].SemanticIndex = t;
			layout[num_items].Format = DXGI_FORMAT_R32G32_FLOAT;
			layout[num_items].InputSlot = 0;
			layout[num_items].AlignedByteOffset = offset;
			layout[num_items].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
			layout[num_items].InstanceDataStepRate = 0;

			num_texcoords++;
			num_items++;
			offset += 8;
		}
	}

	if ( pVertexDecl->m_iTangentOffset>=0 )
	{
		layout[num_items].SemanticName = "TANGENT";
		layout[num_items].SemanticIndex = 0;
		layout[num_items].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout[num_items].InputSlot = 0;
		layout[num_items].AlignedByteOffset = offset;
		layout[num_items].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[num_items].InstanceDataStepRate = 0;

		num_items++;
		offset += 12;

		layout[num_items].SemanticName = "BINORMAL";
		layout[num_items].SemanticIndex = 0;
		layout[num_items].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		layout[num_items].InputSlot = 0;
		layout[num_items].AlignedByteOffset = offset;
		layout[num_items].InputSlotClass = D3D10_INPUT_PER_VERTEX_DATA;
		layout[num_items].InstanceDataStepRate = 0;

		num_items++;
		offset += 12;
	}

	for ( int i=0; i<SHADER_PERMUTATIONS; i++ )
	{
		if ( CGutModel_DX10::s_pVertexShaderCode[i] )
		{
			pDevice->CreateInputLayout( layout, num_items, 
				CGutModel_DX10::s_pVertexShaderCode[i]->GetBufferPointer(), 
				CGutModel_DX10::s_pVertexShaderCode[i]->GetBufferSize(), &m_pVertexDecl[i] );
		}
	}
}

void sModelMaterial_DX10::Submit(ID3D10InputLayout *layout[], UINT mask)
{
	bool bSubmitCullFace = mask & SUBMIT_CULLFACE;
	bool bSubmitMtl = mask & SUBMIT_MTL;
	bool bSubmitShader = mask & SUBMIT_SHADER;
	bool bSubmitTexture = mask & SUBMIT_TEXTURES;
	bool bSubmitBlend = mask & SUBMIT_BLEND;

	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	float blendFactor[4] = {0,0,0,0};

	if ( bSubmitCullFace )
	{
		if ( CGutModel_DX10::s_pCull && CGutModel_DX10::s_pNoCull )
		{
			pDevice->RSSetState(m_bCullFace ? CGutModel_DX10::s_pCull : CGutModel_DX10::s_pNoCull);
		}
		else
		{
			if ( m_bCullFace )
			{
				pDevice->RSSetState(CGutModel_DX10::s_bCullCW ? CGutModel_DX10::s_pCull_CW : CGutModel_DX10::s_pCull_CCW);
			}
			else
			{
				pDevice->RSSetState(CGutModel_DX10::s_pCull_NONE);
			}
		}
	}

	if ( bSubmitBlend )
		pDevice->OMSetBlendState(m_pBlendState, blendFactor, 0xffffffff);

	ID3D10ShaderResourceView *pViews[3] = {NULL, NULL, NULL};

	bool bTextures[3];
	int texStage = 0;
	int shader_index = CGutModel_DX10::s_shader_overwrite >= 0 ? 
		CGutModel_DX10::s_shader_overwrite : 0;

	if ( bSubmitTexture )
	{
		for ( int i=0; i<3; i++ )
		{
			ID3D10ShaderResourceView *pView = NULL;

			if ( CGutModel_DX10::s_pTextureOverwrite[i] )
			{
				pView = CGutModel_DX10::s_pTextureOverwrite[i];
			}
			else
			{
				pView = m_pTextures[i];
			}

			if ( pView )
			{
				pViews[texStage++] = pView;
				if ( CGutModel_DX10::s_shader_overwrite<0 )
					shader_index |= 0x02<<i;
			}
		}

		if ( texStage )
			pDevice->PSSetShaderResources(0, texStage, pViews);
	}

	if ( bSubmitShader )
	{
		if ( CGutModel_DX10::s_bLighting && CGutModel_DX10::s_shader_overwrite<0 )
		{
			shader_index |= 16;
		}

		pDevice->VSSetShader(CGutModel_DX10::s_pVertexShader[shader_index]);
		pDevice->PSSetShader(CGutModel_DX10::s_pPixelShader[shader_index]);
		pDevice->GSSetShader(NULL);

		pDevice->IASetInputLayout(layout[shader_index]);

		sMaterialConstants *pMtlData = NULL;
		CGutModel_DX10::s_pMtlConstantBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pMtlData );

		pMtlData->vEmissive = m_vEmissive;
		pMtlData->vAmbient = m_vAmbient;
		pMtlData->vDiffuse = m_vDiffuse;
		pMtlData->vSpecular = m_vSpecular;
		pMtlData->vPower = m_fPower;

		CGutModel_DX10::s_pMtlConstantBuffer->Unmap();

		if ( CGutModel_DX10::s_bLighting )
		{
			sLightConstants *pLightData = NULL;
			CGutModel_DX10::s_pLightConstantBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pLightData );

			pLightData->m_vGlobalAmbientLight = CGutModel_DX10::s_light.m_vGlobalAmbientLight;

			for ( int i=0; i<LIGHT_MAX_LIGHTS; i++ )
			{
				sGutLight *pSource = &CGutModel_DX10::s_light.m_Lights[i];
				sGutLight *pDest = &pLightData->m_Lights[i];
				*pDest = *pSource;
				// 
				pDest->m_vAmbient = pSource->m_vAmbient * m_vAmbient;
				pDest->m_vDiffuse = pSource->m_vDiffuse * m_vDiffuse;
				pDest->m_vSpecular = pSource->m_vSpecular * m_vSpecular;
			}

			CGutModel_DX10::s_pLightConstantBuffer->Unmap();
		}
	}
}

bool CGutModel_DX10::SetDefaultShader(ID3D10VertexShader *pVS[], ID3D10Blob *pCode[], ID3D10PixelShader *pPS[], int shaders)
{
	for ( int i=0; i<shaders && i<SHADER_PERMUTATIONS; i++ )
	{
		s_pVertexShader[i] = pVS[i];
		s_pVertexShaderCode[i] = pCode[i];
		s_pPixelShader[i] = pPS[i];
	}

	return true;
}

bool CGutModel_DX10::SetDefaultShader(ID3D10VertexShader *pVS, ID3D10Blob *pCode, ID3D10PixelShader *pPS)
{
	for ( int i=0; i<SHADER_PERMUTATIONS; i++ )
	{
		s_pVertexShader[i] = pVS;
		s_pVertexShaderCode[i] = pCode;
		s_pPixelShader[i] = pPS;
	}

	return true;
}

bool CGutModel_DX10::CreateSharedResource(void)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();

	// 配置Shader讀取參數的記憶體空間
	s_pMatrixConstantBuffer = GutCreateShaderConstant_DX10(sizeof(sMatrixConstants));
	s_pMtlConstantBuffer = GutCreateShaderConstant_DX10(sizeof(sMaterialConstants));

	memset(&s_light, 0, sizeof(sLightConstants));
	s_pLightConstantBuffer = GutCreateShaderConstant_DX10(sizeof(sLightConstants), &s_light);

	{
		D3D10_BLEND_DESC desc;
		GutSetDX10DefaultBlendDesc(desc);

		pDevice->CreateBlendState(&desc, &CGutModel_DX10::s_pBlend_Replace);

		desc.BlendEnable[0] = TRUE;
		desc.SrcBlend = D3D10_BLEND_ONE;
		desc.DestBlend = D3D10_BLEND_ONE;

		pDevice->CreateBlendState(&desc, &CGutModel_DX10::s_pBlend_Add);

		desc.SrcBlend = D3D10_BLEND_SRC_ALPHA;
		desc.DestBlend = D3D10_BLEND_INV_SRC_ALPHA;

		pDevice->CreateBlendState(&desc, &CGutModel_DX10::s_pBlend_Blend);
	}

	{
		D3D10_RASTERIZER_DESC desc;
		GutSetDX10DefaultRasterizerDesc(desc);

		desc.CullMode = D3D10_CULL_NONE;
		desc.FrontCounterClockwise = TRUE;

		if ( D3D_OK != pDevice->CreateRasterizerState(&desc, &s_pCull_NONE) )
			return false;

		desc.CullMode = D3D10_CULL_BACK;
		desc.FrontCounterClockwise = TRUE;

		if ( D3D_OK != pDevice->CreateRasterizerState(&desc, &s_pCull_CW) )
			return false;

		desc.CullMode = D3D10_CULL_BACK;
		desc.FrontCounterClockwise = FALSE;

		if ( D3D_OK != pDevice->CreateRasterizerState(&desc, &s_pCull_CCW) )
			return false;

		//s_pCull = s_pCull_CW;
		//s_pNoCull = s_pCull_NONE;
	}

	// matrix
	{
		s_proj_matrix.Identity();
		s_view_matrix.Identity();
		s_inv_view_matrix.Identity();
		s_world_matrix.Identity();
		s_wvp_matrix.Identity();
		s_texture_matrix[0].Identity();
		s_texture_matrix[1].Identity();
		s_texture_matrix[2].Identity();
		s_texture_matrix[3].Identity();
	}

	return true;
}

bool CGutModel_DX10::LoadDefaultShader(const char *filename)
{
	D3D10_SHADER_MACRO macros[SHADER_FLAGS+1];
	ZeroMemory(macros, sizeof(macros));

	const int num_flags = SHADER_FLAGS;

	char macros_name[num_flags][16] =
	{
		"_VERTEXCOLOR",
		"_DIFFUSETEX0",
		"_DIFFUSETEX1",
		"_CUBEMAP",
		"_LIGHTING"
	};

	char macros_definition[num_flags][4];
	ZeroMemory(macros_definition, sizeof(macros_definition));

	for ( int m=0; m<num_flags; m++ )
	{
		macros[m].Name = macros_name[m];
		macros[m].Definition = macros_definition[m];
	}

	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();

	for ( int i=0; i<SHADER_PERMUTATIONS; i++ )
	{
		for ( int bit=0; bit<num_flags; bit++ )
		{
			bool set = ( i & 0x01<<bit );
			strcpy(macros_definition[bit], set ? "1" : "0");
		}

		GutSetHLSLShaderMacrosDX10(macros);

		s_pVertexShader[i] = GutLoadVertexShaderDX10_HLSL(filename, "VS", "vs_4_0", &s_pVertexShaderCode[i]);
		s_pPixelShader[i] = GutLoadPixelShaderDX10_HLSL(filename, "PS", "ps_4_0");
	}

	GutSetHLSLShaderMacrosDX10(NULL);

	CreateSharedResource();

	return true;
}

void CGutModel_DX10::ReleaseDefaultShader(void)
{
	for ( int i=0; i<SHADER_PERMUTATIONS; i++ )
	{
		SAFE_RELEASE(s_pVertexShader[i]);
		SAFE_RELEASE(s_pPixelShader[i]);
		SAFE_RELEASE(s_pVertexShaderCode[i]);
	}

	SAFE_RELEASE(s_pMatrixConstantBuffer);
	SAFE_RELEASE(s_pMtlConstantBuffer);
	SAFE_RELEASE(s_pLightConstantBuffer);
	SAFE_RELEASE(s_pBlend_Add);
	SAFE_RELEASE(s_pBlend_Blend);
	SAFE_RELEASE(s_pBlend_Replace);
	SAFE_RELEASE(s_pCull_CW);
	SAFE_RELEASE(s_pCull_CCW);
	SAFE_RELEASE(s_pCull_NONE);
	SAFE_RELEASE(s_pCull);
	SAFE_RELEASE(s_pNoCull);
}


void CGutModel_DX10::Release(void)
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

bool CGutModel_DX10::ConvertToDX10Model(CGutModel *pModel)
{
	if ( pModel->m_iNumMeshes==0 )
		return false;

	printf("Building model for Direct3D10\n");

	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();

	int i, j;
	char szTextureName[256];

	m_iNumMaterials = pModel->m_iNumMaterials;

	if ( m_iNumMaterials )
	{
		m_pMaterialArray = new sModelMaterial_DX10[m_iNumMaterials];

		for ( i=0; i<m_iNumMaterials; i++ )
		{
			sModelMaterial_DX10 *target = m_pMaterialArray + i;
			sModelMaterial *source = pModel->m_pMaterialArray + i;

			target->m_vAmbient = source->m_Ambient;
			target->m_vDiffuse = source->m_Diffuse;
			target->m_vSpecular = source->m_Specular;
			target->m_vEmissive = source->m_Emissive;
			target->m_fPower = source->m_fShininess;

			target->m_bCullFace = source->m_bCullFace;

			if ( !strcmp(source->m_szBlendMode, "replace") )
			{
				target->m_pBlendState = CGutModel_DX10::s_pBlend_Replace;
			}
			else
			{
				if ( !strcmp(source->m_szBlendMode, "blend") )
				{
					target->m_pBlendState = CGutModel_DX10::s_pBlend_Blend;
				}
				else if ( !strcmp(source->m_szBlendMode, "add") )
				{
					target->m_pBlendState = CGutModel_DX10::s_pBlend_Add;
				}
				else // replace
				{
					target->m_pBlendState = CGutModel_DX10::s_pBlend_Replace;
				}
			}

			target->m_iNumTextures = 0;

			for ( j=0; j<MAX_NUM_TEXTURES; j++ )
			{
				if ( source->m_szTexture[j][0] )
				{
					sprintf(szTextureName, "%s%s", CGutModel::GetTexturePath(), source->m_szTexture[j]);
					if ( source->m_MapChannel[j]==MAP_CUBEMAP )
					{
						target->m_pTextures[j] = GutLoadCubemapTexture_DX10(szTextureName);
					}
					else
					{
						target->m_pTextures[j] = GutLoadTexture_DX10(szTextureName);
					}
					target->m_iNumTextures++;
				}
				else
				{
					target->m_pTextures[j] = NULL;
				}
			}
		}
	}

	m_iNumMeshes = pModel->m_iNumMeshes;
	m_pMeshArray = new sModelMesh_DX10[m_iNumMeshes];

	void *vbuffer_pointer, *ibuffer_pointer;

	for ( i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh *pMeshSource = pModel->m_pMeshArray + i;
		sModelMesh_DX10 *pMesh = m_pMeshArray + i;

		pMesh->m_iNumVertexChunks = pMeshSource->m_iNumVertexChunks;
		pMesh->m_pVertexChunk = new sModelVertexChunk_DX10[pMesh->m_iNumVertexChunks];

		for ( j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk *pVertexChunkTarget = &pMeshSource->m_pVertexChunks[j];
			sModelVertexChunk_DX10 *pVertexChunk = pMesh->m_pVertexChunk + j;
			sVertexDecl *pVertexDecl = &pVertexChunkTarget->m_VertexDecl;

			pVertexChunk->m_iVertexSize = pVertexChunkTarget->m_VertexDecl.m_iVertexSize;
			pVertexChunk->CreateVertexDecl(pVertexDecl);

			pVertexChunk->m_iNumVertices = pVertexChunkTarget->m_iNumVertices;
			int vbuffer_size = pVertexChunk->m_iNumVertices * pVertexChunk->m_iVertexSize;

			D3D10_BUFFER_DESC cbDesc;
			ZeroMemory(&cbDesc, sizeof(cbDesc));

			cbDesc.ByteWidth = vbuffer_size;
			cbDesc.Usage = D3D10_USAGE_DYNAMIC;
			cbDesc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
			cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags = 0;

			// 配置顯示卡記憶體
			pDevice->CreateBuffer(&cbDesc, NULL, &pVertexChunk->m_pVertexBuffer);
			if ( D3D_OK==pVertexChunk->m_pVertexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &vbuffer_pointer) )
			{
				// 把頂點資料從主記憶體拷具到顯示卡上
				pVertexChunkTarget->OutputVertexBuffer(vbuffer_pointer);
				pVertexChunk->m_pVertexBuffer->Unmap();
			}
			else
			{
				return false;
			}

			pVertexChunk->m_iNumIndices = pVertexChunkTarget->m_iNumIndices;
			int ibuffer_size = pVertexChunk->m_iNumIndices * 2;

			ZeroMemory(&cbDesc, sizeof(cbDesc));

			cbDesc.ByteWidth = ibuffer_size;
			cbDesc.Usage = D3D10_USAGE_DYNAMIC;
			cbDesc.BindFlags = D3D10_BIND_INDEX_BUFFER;
			cbDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
			cbDesc.MiscFlags = 0;

			// 配置一塊顯示卡記憶體
			pDevice->CreateBuffer(&cbDesc, NULL, &pVertexChunk->m_pIndexBuffer);
			if ( D3D_OK==pVertexChunk->m_pIndexBuffer->Map(D3D10_MAP_WRITE_DISCARD, 0, &ibuffer_pointer) )
			{
				// 把索引資料從主記憶體拷具到顯示卡上
				memcpy(ibuffer_pointer, pVertexChunkTarget->m_pIndexArray, ibuffer_size);
				pVertexChunk->m_pIndexBuffer->Unmap();
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

void CGutModel_DX10::Render(UINT mask)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	UINT offset = 0;

	pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	bool bSubmitShader = mask & SUBMIT_SHADER;

	if ( bSubmitShader )
	{
		// 設定shader參數
		ID3D10Buffer *buffers[3] = {s_pMatrixConstantBuffer, s_pMtlConstantBuffer, s_pLightConstantBuffer};
		pDevice->VSSetConstantBuffers(0, 3, buffers);
		pDevice->PSSetConstantBuffers(0, 3, buffers);
		// 設定轉換矩陣
		sMatrixConstants *pConstData;
		s_pMatrixConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData->wvp_matrix = s_wvp_matrix;
		pConstData->view_matrix = s_view_matrix;
		pConstData->world_matrix = s_world_matrix;
		pConstData->inv_view_matrix = s_inv_view_matrix;
		pConstData->normal_matrix = s_world_matrix;
		pConstData->texture_matrix[0] = s_texture_matrix[0];
		pConstData->texture_matrix[1] = s_texture_matrix[1];
		pConstData->texture_matrix[2] = s_texture_matrix[2];
		pConstData->texture_matrix[3] = s_texture_matrix[3];
		pConstData->light_wvp_matrix = s_light_wvp_matrix;
		pConstData->light_wv_matrix = s_light_wv_matrix;
		s_pMatrixConstantBuffer->Unmap();
		// 設定光源
		sLightConstants *pLightData = NULL;
		CGutModel_DX10::s_pLightConstantBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pLightData );
		memcpy(pLightData, &s_light, sizeof(s_light));
		CGutModel_DX10::s_pLightConstantBuffer->Unmap();
	}

	for ( int i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh_DX10 *pMesh = m_pMeshArray + i;

		for ( int j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk_DX10 *pVertexChunk = pMesh->m_pVertexChunk + j;

			pDevice->IASetVertexBuffers(0, 1, &pVertexChunk->m_pVertexBuffer, &pVertexChunk->m_iVertexSize, &offset);
			pDevice->IASetIndexBuffer(pVertexChunk->m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			for (int l=0; l<pVertexChunk->m_iNumBatches; l++ )
			{
				sModelBatch *pBatch = pVertexChunk->m_pBatchArray + l;

				if ( mask )
				{
					if ( s_pMaterialOverwrite )
					{
						s_pMaterialOverwrite->Submit(pVertexChunk->m_pVertexDecl, mask);
					}
					else
					{
						if ( m_pMaterialArray && pBatch->m_iMaterialID>=0 )
						{
							m_pMaterialArray[pBatch->m_iMaterialID].Submit(pVertexChunk->m_pVertexDecl, mask);
						}
						else if ( m_iNumMaterials )
						{
							m_pMaterialArray[0].Submit(pVertexChunk->m_pVertexDecl, mask);
						}
					}
				}

				pDevice->DrawIndexed(pBatch->m_iNumIndices, pBatch->m_iIndexArrayBegin, 0);
			}
		}
	}

}

void CGutModel_DX10::RenderMesh(bool submit_mtl)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	UINT offset = 0;

	pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if ( submit_mtl )
	{
		// 設定shader參數
		ID3D10Buffer *buffers[3] = {s_pMatrixConstantBuffer, s_pMtlConstantBuffer, s_pLightConstantBuffer};
		pDevice->VSSetConstantBuffers(0, 3, buffers);
		pDevice->PSSetConstantBuffers(0, 3, buffers);
		// 設定轉換矩陣
		sMatrixConstants *pConstData;
		s_pMatrixConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData->wvp_matrix = s_wvp_matrix;
		pConstData->view_matrix = s_view_matrix;
		pConstData->world_matrix = s_world_matrix;
		pConstData->inv_view_matrix = s_inv_view_matrix;
		pConstData->normal_matrix = s_world_matrix;
		pConstData->texture_matrix[0] = s_texture_matrix[0];
		pConstData->texture_matrix[1] = s_texture_matrix[1];
		pConstData->texture_matrix[2] = s_texture_matrix[2];
		pConstData->texture_matrix[3] = s_texture_matrix[3];
		pConstData->light_wvp_matrix = s_light_wvp_matrix;
		pConstData->light_wv_matrix = s_light_wv_matrix;
		s_pMatrixConstantBuffer->Unmap();
		// 設定光源
		sLightConstants *pLightData = NULL;
		CGutModel_DX10::s_pLightConstantBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pLightData );
		memcpy(pLightData, &s_light, sizeof(s_light));
		CGutModel_DX10::s_pLightConstantBuffer->Unmap();
	}

	for ( int i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh_DX10 *pMesh = m_pMeshArray + i;

		for ( int j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk_DX10 *pVertexChunk = pMesh->m_pVertexChunk + j;

			pDevice->IASetVertexBuffers(0, 1, &pVertexChunk->m_pVertexBuffer, &pVertexChunk->m_iVertexSize, &offset);
			pDevice->IASetIndexBuffer(pVertexChunk->m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			for (int l=0; l<pVertexChunk->m_iNumBatches; l++ )
			{
				sModelBatch *pBatch = pVertexChunk->m_pBatchArray + l;

				if ( submit_mtl )
				{
					if ( s_pMaterialOverwrite )
					{
						s_pMaterialOverwrite->Submit(pVertexChunk->m_pVertexDecl);
					}
					else
					{
						if ( m_pMaterialArray && pBatch->m_iMaterialID>=0 )
						{
							m_pMaterialArray[pBatch->m_iMaterialID].Submit(pVertexChunk->m_pVertexDecl);
						}
					}
				}

				pDevice->DrawIndexed(pBatch->m_iNumIndices, pBatch->m_iIndexArrayBegin, 0);
			}
		}
	}

}

void CGutModel_DX10::RenderInstance(int num_instances, int instance_start, int mask)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	bool bSubmitMtl = (mask & SUBMIT_MTL) ? true : false;
	UINT offset = 0;

	pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	if ( bSubmitMtl )
	{
		// 設定shader參數
		ID3D10Buffer *buffers[3] = {s_pMatrixConstantBuffer, s_pMtlConstantBuffer, s_pLightConstantBuffer};
		pDevice->VSSetConstantBuffers(0, 3, buffers);
		pDevice->PSSetConstantBuffers(0, 3, buffers);
		// 設定轉換矩陣
		sMatrixConstants *pConstData;
		s_pMatrixConstantBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
		pConstData->wvp_matrix = s_wvp_matrix;
		pConstData->view_matrix = s_view_matrix;
		pConstData->world_matrix = s_world_matrix;
		pConstData->inv_view_matrix = s_inv_view_matrix;
		pConstData->normal_matrix = s_world_matrix;
		pConstData->texture_matrix[0] = s_texture_matrix[0];
		pConstData->texture_matrix[1] = s_texture_matrix[1];
		pConstData->texture_matrix[2] = s_texture_matrix[2];
		pConstData->texture_matrix[3] = s_texture_matrix[3];
		pConstData->light_wvp_matrix = s_light_wvp_matrix;
		pConstData->light_wv_matrix = s_light_wv_matrix;
		s_pMatrixConstantBuffer->Unmap();
		// 設定光源
		sLightConstants *pLightData = NULL;
		CGutModel_DX10::s_pLightConstantBuffer->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pLightData );
		memcpy(pLightData, &s_light, sizeof(s_light));
		CGutModel_DX10::s_pLightConstantBuffer->Unmap();
	}

	for ( int i=0; i<m_iNumMeshes; i++ )
	{
		sModelMesh_DX10 *pMesh = m_pMeshArray + i;

		for ( int j=0; j<pMesh->m_iNumVertexChunks; j++ )
		{
			sModelVertexChunk_DX10 *pVertexChunk = pMesh->m_pVertexChunk + j;

			pDevice->IASetVertexBuffers(0, 1, &pVertexChunk->m_pVertexBuffer, &pVertexChunk->m_iVertexSize, &offset);
			pDevice->IASetIndexBuffer(pVertexChunk->m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			for (int l=0; l<pVertexChunk->m_iNumBatches; l++ )
			{
				sModelBatch *pBatch = pVertexChunk->m_pBatchArray + l;

				if ( bSubmitMtl )
				{
					if ( s_pMaterialOverwrite )
					{
						s_pMaterialOverwrite->Submit(pVertexChunk->m_pVertexDecl);
					}
					else
					{
						if ( m_pMaterialArray && pBatch->m_iMaterialID>=0 )
						{
							m_pMaterialArray[pBatch->m_iMaterialID].Submit(pVertexChunk->m_pVertexDecl);
						}
					}
				}

				pDevice->DrawIndexedInstanced(pBatch->m_iNumIndices, num_instances, pBatch->m_iIndexArrayBegin, 0, instance_start);
			}
		}
	}

}

#endif // _ENABLE_DX10_
