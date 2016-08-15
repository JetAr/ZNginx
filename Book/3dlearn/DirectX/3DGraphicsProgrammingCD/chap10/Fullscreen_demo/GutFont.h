#ifndef _GUT_FONT_
#define _GUT_FONT_

#include "Vector.h"

struct _FontVertex
{
	Vector4 m_Position;
	Vector4 m_Texcoord;
};

class GutConsole
{
protected:
	// 
	float m_fWidth, m_fHeight;
	//
	float m_fFontWidth, m_fFontHeight;
	//
	int   m_iLayoutW, m_iLayoutH;
	int   m_iAsciiBegin;
	//
	int   m_iNumRows, m_iNumColumns;
	//
	int	  m_iNumLines;
	//
	int   m_iNumCharacters;
	//
	char *pContent;
	//
	_FontVertex *m_pVertexBuffer;
	//
	short *m_pIndexBuffer;

	void Release(void);
	char *GetLine(int line);
	void AccessTexture(char c, int &x, int &y);
	void BuildMesh(void);

public:
	GutConsole(void);
	~GutConsole();

	void SetConsoleResolution(float width, float height) { m_fWidth = width; m_fHeight = height; }
	void SetConsoleSize(int w, int h);
	void SetFontSize(float width, float height) { m_fFontWidth = width; m_fFontHeight = height; }
	void SetFontTextureLayout(int w, int h, int ascii_begin);

	void Clear(void);
	void Puts(const char *string);

	virtual bool LoadTexture(const char *filename) {};
	virtual bool Render(void) {};
};

#endif