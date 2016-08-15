#ifndef _GUTFONT_DX9_
#define _GUTFONT_DX9_

#include <d3d10.h>

#include "GutFont.h"
#include "GutFontUniCodeWin32.h"

class CGutFontDX10 : public CGutFont
{
private:
	ID3D10ShaderResourceView *m_pFontTexture;

	ID3D10VertexShader *m_pVertexShader;
	ID3D10PixelShader  *m_pPixelShader;
	ID3D10InputLayout  *m_pVertexLayout;
	ID3D10Buffer	   *m_pShaderConstant;
	ID3D10RasterizerState *m_pRasterizerState;

	ID3D10Buffer *m_pVertexBuffer;
	ID3D10Buffer *m_pIndexBuffer;

public:
	CGutFontDX10(void);
	~CGutFontDX10();

	bool LoadShader(const char *filename);

	virtual void BuildMesh(void);
	virtual bool LoadTexture(const char *filename);
	virtual void Render(void);
};

class CGutFontUniCodeDX10 : public CGutFontUniCode
{
private:
	ID3D10Texture2D *m_pFontTexture2D;
	ID3D10ShaderResourceView *m_pFontTexture;

	ID3D10VertexShader *m_pVertexShader;
	ID3D10PixelShader  *m_pPixelShader;
	ID3D10InputLayout  *m_pVertexLayout;
	ID3D10Buffer	   *m_pShaderConstant;
	ID3D10RasterizerState *m_pRasterizerState;

	ID3D10Buffer *m_pVertexBuffer;
	ID3D10Buffer *m_pIndexBuffer;

public:
	CGutFontUniCodeDX10(void);
	~CGutFontUniCodeDX10();

	bool LoadShader(const char *filename);

	virtual void BuildMesh(void);
	virtual bool CreateTexture(int w, int h);
	virtual bool AccessTexture(WCHAR c, int &x, int &y);
	virtual void Render(void);
};

#endif // _GUTFONT_DX9_