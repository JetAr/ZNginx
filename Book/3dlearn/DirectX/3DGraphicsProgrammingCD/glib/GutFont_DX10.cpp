#ifdef _ENABLE_DX10_

#include "Gut.h"
#include "GutDX10.h"
#include "GutTexture_DX10.h"
#include "GutFont_DX10.h"

CGutFontDX10::CGutFontDX10(void)
{
	m_pFontTexture = NULL;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;

	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pVertexLayout = NULL;
	m_pShaderConstant = NULL;
	m_pRasterizerState = NULL;
}

CGutFontDX10::~CGutFontDX10()
{
	SAFE_RELEASE(m_pFontTexture);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pShaderConstant);
	SAFE_RELEASE(m_pRasterizerState);
}

bool CGutFontDX10::LoadShader(const char *filename)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Vertex Shader
	m_pVertexShader = GutLoadVertexShaderDX10_HLSL(filename, "VS", "vs_4_0", &pVSCode);
	if ( NULL==m_pVertexShader )
		return false;
	// 載入Pixel Shader
	m_pPixelShader = GutLoadPixelShaderDX10_HLSL(filename, "PS", "ps_4_0");
	if ( NULL==m_pPixelShader )
		return false;

	// Shader常數的記憶體空間
	{
		D3D10_BUFFER_DESC desc;
		//
		desc.ByteWidth = sizeof(Matrix4x4);
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		if ( D3D_OK != pDevice->CreateBuffer( &desc, NULL, &m_pShaderConstant ) )
			return false;
	}

	int max_characters = m_iNumRows * m_iNumColumns;

	// vertex buffer
	{
		D3D10_BUFFER_DESC desc;

		desc.ByteWidth = max_characters * sizeof(_FontVertex) * 4;
		desc.Usage = D3D10_USAGE_DYNAMIC ;
		desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		if ( D3D_OK != pDevice->CreateBuffer( &desc, NULL, &m_pVertexBuffer ) )
			return false;
	}
	// index buffer
	{
		D3D10_BUFFER_DESC desc;

		desc.ByteWidth = max_characters * 6 * sizeof(short);
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA subDesc;
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = m_pIndexArray;

		if ( D3D_OK != pDevice->CreateBuffer( &desc, &subDesc, &m_pIndexBuffer ) )
			return false;
	}
	// rasterizer state物件
	{
		D3D10_RASTERIZER_DESC desc;
		GutSetDX10DefaultRasterizerDesc(desc);

		desc.CullMode = D3D10_CULL_NONE;

		if ( D3D_OK != pDevice->CreateRasterizerState(&desc, &m_pRasterizerState) )
			return false;
	}
	// 設定Vertex資料格式
	{
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};
		//
		if ( D3D_OK != pDevice->CreateInputLayout(layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &m_pVertexLayout ) )
			return false;
	}

	return true;
}

void CGutFontDX10::BuildMesh(void)
{
	CGutFont::BuildMesh();

	// 更新shader參數
	_FontVertex *pVertices;
	m_pVertexBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pVertices );
	memcpy(pVertices, m_pVertexArray, sizeof(_FontVertex) * m_iNumCharacters * 4);
	m_pVertexBuffer->Unmap();
}

bool CGutFontDX10::LoadTexture(const char *filename)
{
	m_pFontTexture = GutLoadTexture_DX10(filename);
	return m_pFontTexture ? true : false;
}

void CGutFontDX10::Render(void)
{
	UINT stride = sizeof(_FontVertex);
	UINT offset = 0;
	//
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	// 使用平行視角	
	Matrix4x4 proj_matrix = GutMatrixOrthoRH_DirectX(m_fWidth, m_fHeight, 0.0f, 1.0f);
	Matrix4x4 view_matrix; view_matrix.Identity();
	view_matrix[3].Set(-m_fWidth/2.0f, -m_fHeight/2.0f, 0.0f, 1.0f);
	Matrix4x4 vp_matrix = view_matrix * proj_matrix;
	// 更新shader參數
	Matrix4x4 *pConstData;
	m_pShaderConstant->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = vp_matrix;
	m_pShaderConstant->Unmap();
	// 
	pDevice->RSSetState(m_pRasterizerState);
	// 設定Shader
	pDevice->VSSetShader(m_pVertexShader);
	pDevice->PSSetShader(m_pPixelShader);
	// 設定vertex shader讀取參數的記憶體位罝
	pDevice->VSSetConstantBuffers(0, 1, &m_pShaderConstant);
	// 套用字型貼圖
	pDevice->PSSetShaderResources(0, 1, &m_pFontTexture);
	// 設定vertex資料格式
	pDevice->IASetInputLayout(m_pVertexLayout);
	// 設定vertex buffer
	pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// 設定三角形頂點索引值資料排列是triangle list
	pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 畫出文字
	pDevice->DrawIndexed(m_iNumCharacters*6, 0, 0);
}




CGutFontUniCodeDX10::CGutFontUniCodeDX10(void)
{
	m_pFontTexture2D = NULL;
	m_pFontTexture = NULL;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;

	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pVertexLayout = NULL;
	m_pShaderConstant = NULL;
	m_pRasterizerState = NULL;
}

CGutFontUniCodeDX10::~CGutFontUniCodeDX10()
{
	SAFE_RELEASE(m_pFontTexture2D);
	SAFE_RELEASE(m_pFontTexture);
	SAFE_RELEASE(m_pVertexBuffer);
	SAFE_RELEASE(m_pIndexBuffer);

	SAFE_RELEASE(m_pVertexShader);
	SAFE_RELEASE(m_pPixelShader);
	SAFE_RELEASE(m_pVertexLayout);
	SAFE_RELEASE(m_pShaderConstant);
	SAFE_RELEASE(m_pRasterizerState);
}

bool CGutFontUniCodeDX10::LoadShader(const char *filename)
{
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	ID3D10Blob *pVSCode = NULL;

	// 載入Vertex Shader
	m_pVertexShader = GutLoadVertexShaderDX10_HLSL(filename, "VS", "vs_4_0", &pVSCode);
	if ( NULL==m_pVertexShader )
		return false;
	// 載入Pixel Shader
	m_pPixelShader = GutLoadPixelShaderDX10_HLSL(filename, "PS", "ps_4_0");
	if ( NULL==m_pPixelShader )
		return false;

	// Shader常數的記憶體空間
	{
		D3D10_BUFFER_DESC desc;
		//
		desc.ByteWidth = sizeof(Matrix4x4);
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		if ( D3D_OK != pDevice->CreateBuffer( &desc, NULL, &m_pShaderConstant ) )
			return false;
	}

	int max_characters = m_iNumRows * m_iNumColumns;

	// vertex buffer
	{
		D3D10_BUFFER_DESC desc;

		desc.ByteWidth = max_characters * sizeof(_FontVertex) * 4;
		desc.Usage = D3D10_USAGE_DYNAMIC ;
		desc.BindFlags = D3D10_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		if ( D3D_OK != pDevice->CreateBuffer( &desc, NULL, &m_pVertexBuffer ) )
			return false;
	}
	// index buffer
	{
		D3D10_BUFFER_DESC desc;

		desc.ByteWidth = max_characters * 6 * sizeof(short);
		desc.Usage = D3D10_USAGE_DYNAMIC;
		desc.BindFlags = D3D10_BIND_INDEX_BUFFER;
		desc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;

		D3D10_SUBRESOURCE_DATA subDesc;
		ZeroMemory(&subDesc, sizeof(subDesc));
		subDesc.pSysMem = m_pIndexArray;

		if ( D3D_OK != pDevice->CreateBuffer( &desc, &subDesc, &m_pIndexBuffer ) )
			return false;
	}
	// rasterizer state物件
	{
		D3D10_RASTERIZER_DESC desc;
		GutSetDX10DefaultRasterizerDesc(desc);

		desc.CullMode = D3D10_CULL_NONE;

		if ( D3D_OK != pDevice->CreateRasterizerState(&desc, &m_pRasterizerState) )
			return false;
	}
	// 設定Vertex資料格式
	{
		D3D10_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0 }
		};
		//
		if ( D3D_OK != pDevice->CreateInputLayout(layout, sizeof(layout)/sizeof(D3D10_INPUT_ELEMENT_DESC), pVSCode->GetBufferPointer(), pVSCode->GetBufferSize(), &m_pVertexLayout ) )
			return false;
	}

	return true;
}

void CGutFontUniCodeDX10::BuildMesh(void)
{
	CGutFont::BuildMesh();

	// 更新shader參數
	_FontVertex *pVertices;
	m_pVertexBuffer->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pVertices );
	memcpy(pVertices, m_pVertexArray, sizeof(_FontVertex) * m_iNumCharacters * 4);
	m_pVertexBuffer->Unmap();
}

bool CGutFontUniCodeDX10::AccessTexture(WCHAR c, int &x, int &y)
{
	bool bUpdateTexture = CGutFontUniCode::AccessTexture(c, x, y);

	if ( bUpdateTexture )
	{
		ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();

		float tX = (float)x / (float)m_iLayoutW;
		float tY = (float)y / (float)m_iLayoutH;
		float tW = 1.0f/(float)m_iLayoutW;
		float tH = 1.0f/(float)m_iLayoutH;

		int left = tX * m_iTextureW;
		int width = tW * m_iTextureW;
		int right = left + width;
		int top = tY * m_iTextureH;
		int height = tH * m_iTextureH;
		int bottom = top + height;

		unsigned char *pBuffer = new unsigned char[width*height];
		unsigned char *buffer = pBuffer;

		for ( int y=0; y<height; y++ )
		{
			for ( int x=0; x<width; x++ )
			{
				COLORREF rgb = GetPixel(m_MemDC, x, y);

				buffer[0] = GetRValue(rgb);

				buffer++;
			}
		}

		D3D10_BOX box;

		box.left = left;
		box.right = right;
		box.top = top;
		box.bottom = bottom;
		box.front = 0;
		box.back = 1;

		pDevice->UpdateSubresource(
			m_pFontTexture2D, 0, 
			&box, pBuffer, 
			width,
			width*height);

		delete [] pBuffer;

	}

	return true;
}

bool CGutFontUniCodeDX10::CreateTexture(int w, int h)
{
	m_iTextureW = w;
	m_iTextureH = h;

	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	//
	{
		D3D10_TEXTURE2D_DESC desc;
		ZeroMemory( &desc, sizeof(desc) );

		desc.Width =  w;
		desc.Height =  h;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.SampleDesc.Count = 1;
		desc.Format = DXGI_FORMAT_A8_UNORM;
		desc.Usage = D3D10_USAGE_DEFAULT;
		desc.BindFlags = D3D10_BIND_SHADER_RESOURCE;

		pDevice->CreateTexture2D( &desc, NULL, &m_pFontTexture2D );
	}
	//
	{
		D3D10_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));

		desc.Format = DXGI_FORMAT_A8_UNORM;
		desc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
		desc.Texture2D.MostDetailedMip = 0;
		desc.Texture2D.MipLevels = 1;

		pDevice->CreateShaderResourceView(m_pFontTexture2D, &desc, &m_pFontTexture);
	}

	return true;
}

void CGutFontUniCodeDX10::Render(void)
{
	if ( m_iNumCharacters==0 )
		return;

	UINT stride = sizeof(_FontVertex);
	UINT offset = 0;
	//
	ID3D10Device *pDevice = GutGetGraphicsDeviceDX10();
	// 使用平行視角	
	Matrix4x4 proj_matrix = GutMatrixOrthoRH_DirectX(m_fWidth, m_fHeight, 0.0f, 1.0f);
	Matrix4x4 view_matrix; view_matrix.Identity();
	view_matrix[3].Set(-m_fWidth/2.0f, -m_fHeight/2.0f, 0.0f, 1.0f);
	Matrix4x4 vp_matrix = view_matrix * proj_matrix;
	// 更新shader參數
	Matrix4x4 *pConstData;
	m_pShaderConstant->Map( D3D10_MAP_WRITE_DISCARD, NULL, (void **) &pConstData );
	*pConstData = vp_matrix;
	m_pShaderConstant->Unmap();
	// 
	pDevice->RSSetState(m_pRasterizerState);
	// 設定Shader
	pDevice->VSSetShader(m_pVertexShader);
	pDevice->PSSetShader(m_pPixelShader);
	// 設定vertex shader讀取參數的記憶體位罝
	pDevice->VSSetConstantBuffers(0, 1, &m_pShaderConstant);
	// 套用字型貼圖
	pDevice->PSSetShaderResources(0, 1, &m_pFontTexture);
	// 設定vertex資料格式
	pDevice->IASetInputLayout(m_pVertexLayout);
	// 設定vertex buffer
	pDevice->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
	pDevice->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	// 設定三角形頂點索引值資料排列是triangle list
	pDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// 畫出文字
	pDevice->DrawIndexed(m_iNumCharacters*6, 0, 0);
}

#endif // _ENABLE_DX10_