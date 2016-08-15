#ifndef _GUT_FONT_
#define _GUT_FONT_

#include "Vector4.h"

struct _FontVertex_Temp
{
	Vector4 m_Position;
	Vector4 m_Texcoord;
};

// Direct3D9 的 Fixed Pipeline 對頂點資料格式比較挑
struct _FontVertex
{
	float m_Position[3];
	float m_Texcoord[2];
};

class CGutFont
{
protected:
	// 
	float m_fWidth, m_fHeight;
	//
	float m_fFontWidth, m_fFontHeight;
	float m_fAdvanceX, m_fAdvanceY;
	//
	int   m_iLayoutW, m_iLayoutH;
	int   m_iAsciiBegin;
	//
	int   m_iNumRows, m_iNumColumns;
	//
	int	  m_iNumLines;
	//
	int   m_iNumCharacters;
	int   m_iNumCharactersSet;
	//
	WCHAR *m_pContent;
	//
	_FontVertex *m_pVertexArray;
	//_FontVertexDX9 *m_pVertexArrayDX9;
	//
	short *m_pIndexArray;

	WCHAR *GetLine(int line);

public:
	CGutFont(void);
	~CGutFont();

	void SetConsoleResolution(float width, float height) { m_fWidth = width; m_fHeight = height; }
	void SetConsoleSize(int w, int h);
	void SetFontTextureLayout(int w, int h, int ascii_begin);
	void SetFontSize(float width, float height) { m_fFontWidth = width; m_fFontHeight = height; }
	void SetFontAdvance(float x, float y) { m_fAdvanceX = x; m_fAdvanceY = y; }

	void Clear(void);
	void Puts(const char *pString, bool bBuildMesh = true);
	void Puts(const WCHAR *pString, bool bBuildMesh = true);
	void Puts(int row, const char *pString, bool bBuildMesh = true);
	void Puts(int row, const WCHAR *pString, bool bBuildMesh = true);

	virtual void BuildMesh(void);
	virtual bool LoadTexture(const char *filename) { return false;}
	virtual void Render(void) {}
	virtual bool AccessTexture(WCHAR c, int &x, int &y);
	virtual void Release(void);
};

#endif