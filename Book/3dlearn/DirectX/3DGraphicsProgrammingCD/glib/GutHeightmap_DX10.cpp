#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutDX10.h"
#include "GutModel_DX10.h"
#include "GutHeightmap_DX10.h"

CGutHeightmapDX10::CGutHeightmapDX10(void)
{
	m_pShaderConstant[0] = NULL;
	m_pShaderConstant[1] = NULL;
	m_pShaderConstant[2] = NULL;

	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pVertexLayout = NULL;
}

CGutHeightmapDX10::~CGutHeightmapDX10()
{
	Release();
}

void CGutHeightmapDX10::Release(void)
{
	SAFE_RELEASE(m_pShaderConstant[0]);
	SAFE_RELEASE(m_pShaderConstant[1]);
	SAFE_RELEASE(m_pShaderConstant[2]);

	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);

}

bool CGutHeightmapDX10::LoadShader(const char *filename)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Shader
	{
		D3D10_SHADER_MACRO macros[] =
		{
			"_LIGHTING", "1",
			NULL, NULL,
		};

		GutSetHLSLShaderMacrosDX10(macros);

		m_pVertexShader = GutLoadVertexShaderDX10_HLSL("../../shaders/gmodel_dx10.hlsl", 
			"VS", "vs_4_0", &pVSCode);
		m_pPixelShader = GutLoadPixelShaderDX10_HLSL("../../shaders/gmodel_dx10.hlsl", 
			"PS", "ps_4_0");

		if ( m_pVertexShader==NULL || m_pPixelShader==NULL )
			return false;

		GutSetHLSLShaderMacrosDX10(NULL);
	}
	// 設定頂點資料格式
	{
		//
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
		};
		//
		if ( D3D_OK != pDevice->CreateInputLayout(layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), 
			pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &m_pVertexLayout ) )
			return false;
	}
	// Shader常數的記憶體空間
	{
		m_pShaderConstant[0] = GutCreateShaderConstant_DX10(sizeof(sMatrixConstants));
		m_pShaderConstant[1] = GutCreateShaderConstant_DX10(sizeof(sMaterialConstants));
		m_pShaderConstant[2] = GutCreateShaderConstant_DX10(sizeof(sLightConstants));

		if ( m_pShaderConstant[0]==NULL || 
			m_pShaderConstant[1]==NULL ||
			m_pShaderConstant[2]==NULL )
			return false;
	}
	return true;
}

bool CGutHeightmapDX10::BuildMesh(int x_grids, int y_grids)
{
	CGutHeightmap::BuildMesh(x_grids, y_grids);

	// 模型記憶體空間
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	m_pVertexBuffer = GutCreateVertexBuffer_DX10(m_iNumVertices * sizeof(sTerrainVertex), m_pVertexArray);
	m_pIndexBuffer = GutCreateIndexBuffer_DX10(m_iNumIndices * sizeof(short), m_pIndexArray);

	if (m_pVertexBuffer==NULL || m_pIndexBuffer==NULL )
		return false;

	return true;
}

void CGutHeightmapDX10::Render(Matrix4x4 &world_matrix, Matrix4x4 &view_matrix, Matrix4x4 &proj_matrix)
{
	if ( m_iNumFaces==0 )
		return;
	// 取得Direct3D10裝置
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	// 設定頂點資料格式
	UINT stride = sizeof(sTerrainVertex);
	UINT offset = 0;
	pDevice->IASetInputLayout(m_pVertexLayout);
	pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDevice->VSSetShader(m_pVertexShader);
	pDevice->PSSetShader(m_pPixelShader);
	pDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	//
	{
		sMatrixConstants *pMatrix;
		m_pShaderConstant[0]->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **)&pMatrix);
		pMatrix->wvp_matrix = world_matrix * view_matrix * proj_matrix;
		pMatrix->view_matrix = view_matrix;
		pMatrix->world_matrix = world_matrix;
		pMatrix->normal_matrix = world_matrix;
		pMatrix->texture_matrix[0].Identity();
		m_pShaderConstant[0]->Unmap();
	}
	{
		sMaterialConstants *pMtl;
		m_pShaderConstant[1]->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **)&pMtl);
		pMtl->vEmissive.Set(0.0f);
		pMtl->vAmbient.Set(0.1f);
		pMtl->vDiffuse.Set(1.0f);
		pMtl->vSpecular.Set(1.0f);
		m_pShaderConstant[1]->Unmap();
	}
	{
		/*
		sLightConstants *pLights;
		m_pShaderConstant[2]->Map(D3D10_MAP_WRITE_DISCARD, NULL, (void **)&pLights);
		pLights->m_Lights[0].m_vPosition = light->m_vPosition;
		pLights->m_Lights[0].m_vDirection = light->m_vDirection;
		pLights->m_Lights[0].m_vDiffuse = light->m_vDiffuse;
		pLights->m_Lights[0].m_vSpecular = light->m_vSpecular;
		m_pShaderConstant[2]->Unmap();
		*/
	}
	//
	pDevice->VSSetConstantBuffers(0, 3, m_pShaderConstant);
	pDevice->PSSetConstantBuffers(0, 3, m_pShaderConstant);
	pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	// 畫出所有的文字
	pDevice->DrawIndexed(m_iNumIndices, 0, 0);
}

#endif