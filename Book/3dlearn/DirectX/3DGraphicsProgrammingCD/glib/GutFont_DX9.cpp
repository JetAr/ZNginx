#include "Gut.h"
#include "GutDX9.h"
#include "GutTexture_DX9.h"
#include "GutModel_DX9.h"
#include "GutFont_DX9.h"

CGutFontDX9::CGutFontDX9(void)
{
	m_pFontTexture = NULL;
}

CGutFontDX9::~CGutFontDX9()
{
	SAFE_RELEASE(m_pFontTexture);
}

bool CGutFontDX9::LoadTexture(const char *filename)
{
	m_pFontTexture = GutLoadTexture_DX9(filename);
	return m_pFontTexture ? true : false;
}

void CGutFontDX9::Render(void)
{
	if ( m_iNumCharacters==0 )
		return;

	LPDIRECT3DDEVICE9 pDevice = GutGetGraphicsDeviceDX9();
	// 套用字型貼圖
	sModelMaterial_DX9 mtl;
	mtl.m_bCullFace = false;
	mtl.m_pTextures[0] = m_pFontTexture;
	mtl.Submit();
	// 開啟Alpha Test
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 128);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	// 使用平行視角鏡頭
	Matrix4x4 proj_matrix = GutMatrixOrthoRH_DirectX(m_fWidth, m_fHeight, 0.0f, 1.0f);
	Matrix4x4 view_matrix; view_matrix.Identity();
	Matrix4x4 ident_matrix; ident_matrix.Identity();
	view_matrix[3].Set(-m_fWidth/2.0f, -m_fHeight/2.0f, 0.0f, 1.0f);
	// 設定轉換矩陣
	pDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&proj_matrix);
	pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&view_matrix);
	pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&ident_matrix);
	// 設定頂點資料格式
	pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 
	// 畫出所有的文字
	pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
		m_iNumCharacters*4, m_iNumCharacters*2, 
		m_pIndexArray, D3DFMT_INDEX16, 
		m_pVertexArray, sizeof(_FontVertex));
}


CGutFontUniCodeDX9::CGutFontUniCodeDX9(void)
{
	m_pFontTexture = NULL;
}

CGutFontUniCodeDX9::~CGutFontUniCodeDX9()
{
	SAFE_RELEASE(m_pFontTexture);
}

bool CGutFontUniCodeDX9::CreateTexture(int w, int h)
{
	LPDIRECT3DDEVICE9 pDevice = GutGetGraphicsDeviceDX9();

	m_iTextureW = w;
	m_iTextureH = h;

	pDevice->CreateTexture(w, h, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &m_pFontTexture, NULL);
	//pDevice->CreateTexture(w, h, 1, 0, D3DFMT_A8L8, D3DPOOL_MANAGED, &m_pFontTexture, NULL);

	return m_pFontTexture ? true : false;
}

bool CGutFontUniCodeDX9::AccessTexture(WCHAR c, int &x, int &y)
{
	bool bUpdateTexture = CGutFontUniCode::AccessTexture(c, x, y);

	if ( bUpdateTexture )
	{
		float tX = (float)x / (float)m_iLayoutW;
		float tY = (float)y / (float)m_iLayoutH;
		float tW = 1.0f/(float)m_iLayoutW;
		float tH = 1.0f/(float)m_iLayoutH;

		D3DLOCKED_RECT locked;

		RECT rect;
		rect.left = tX * m_iTextureW;
		rect.right = rect.left + tW * m_iTextureW;
		rect.top = tY * m_iTextureH;
		rect.bottom = rect.top + tH * m_iTextureH;

		m_pFontTexture->LockRect(0, &locked, &rect, 0);

		if ( locked.pBits )
		{
			unsigned char *buffer = (unsigned char *)locked.pBits;

			for ( int y=rect.top; y<rect.bottom; y++ )
			{
				int py = y - rect.top;
				for ( int x=rect.left; x<rect.right; x++ )
				{
					int px = x - rect.left;
					COLORREF rgb = GetPixel(m_MemDC, px, py);
					int base = px*4;
					//int base = px*2;
					int grayscale = GetRValue(rgb) + GetGValue(rgb) + GetBValue(rgb);
					grayscale /= 3;

					buffer[base] = grayscale;
					buffer[base+1] = grayscale;
					buffer[base+2] = grayscale;
					buffer[base+3] = grayscale;
				}
				buffer += locked.Pitch;
			}
		}

		m_pFontTexture->UnlockRect(0);
		m_pFontTexture->AddDirtyRect(&rect);
	}

	return true;
}

void CGutFontUniCodeDX9::Render(void)
{
	if ( m_iNumCharacters==0 )
		return;

	LPDIRECT3DDEVICE9 pDevice = GutGetGraphicsDeviceDX9();
	// 套用字型貼圖
	sModelMaterial_DX9 mtl;
	mtl.m_bCullFace = false;
	mtl.m_pTextures[0] = m_pFontTexture;
	mtl.Submit();
	// 開啟Alpha Test
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 128);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
	// 使用平行視角鏡頭
	Matrix4x4 proj_matrix = GutMatrixOrthoRH_DirectX(m_fWidth, m_fHeight, 0.0f, 1.0f);
	Matrix4x4 view_matrix; view_matrix.Identity();
	Matrix4x4 ident_matrix; ident_matrix.Identity();
	view_matrix[3].Set(-m_fWidth/2.0f, -m_fHeight/2.0f, 0.0f, 1.0f);
	// 設定轉換矩陣
	pDevice->SetTransform(D3DTS_PROJECTION, (D3DMATRIX*)&proj_matrix);
	pDevice->SetTransform(D3DTS_VIEW, (D3DMATRIX*)&view_matrix);
	pDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&ident_matrix);
	// 設定頂點資料格式
	pDevice->SetFVF(D3DFVF_XYZ|D3DFVF_TEX1); 
	// 畫出所有的文字
	pDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, 
		m_iNumCharacters*4, m_iNumCharacters*2, 
		m_pIndexArray, D3DFMT_INDEX16, 
		m_pVertexArray, sizeof(_FontVertex));
}

