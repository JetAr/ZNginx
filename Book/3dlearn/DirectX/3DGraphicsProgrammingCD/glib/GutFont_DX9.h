#ifndef _GUTFONT_DX9_
#define _GUTFONT_DX9_

#include <d3d9.h>

#include "GutFont.h"
#include "GutFontUniCodeWin32.h"

class CGutFontDX9 : public CGutFont
{
private:
	LPDIRECT3DTEXTURE9 m_pFontTexture;

public:
	CGutFontDX9(void);
	~CGutFontDX9();

	virtual bool LoadTexture(const char *filename);
	virtual void Render(void);
};

class CGutFontUniCodeDX9 : public CGutFontUniCode
{
private:
	LPDIRECT3DTEXTURE9 m_pFontTexture;

public:
	CGutFontUniCodeDX9(void);
	~CGutFontUniCodeDX9();

	virtual bool AccessTexture(WCHAR c, int &x, int &y);
	virtual bool CreateTexture(int w, int h);
	virtual void Render(void);
};

#endif // _GUTFONT_DX9_