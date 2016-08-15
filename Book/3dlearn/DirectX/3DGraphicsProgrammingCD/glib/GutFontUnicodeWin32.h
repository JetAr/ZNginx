#ifndef _GUT_FONTUNICODE_
#define _GUT_FONTUNICODE_

#include <windows.h>
#include <gdiplus.h>
#include <map>

#include "GutFont.h"

struct sCharInfo
{
	WCHAR m_Code;
	UINT  m_ReferencedFrame;
};

class CGutFontUniCode : public CGutFont
{
protected:
	HDC m_MemDC;
	HBITMAP m_Bitmap;
	HFONT m_Font;
	UINT  m_frameCount;
	int m_iTextureW, m_iTextureH;

	std::map<WCHAR, int> m_Map;
	sCharInfo *m_pMap;

	virtual bool AccessTexture(WCHAR c, int &x, int &y);
	virtual void Release(void);

public:
	CGutFontUniCode(void);
	~CGutFontUniCode();
	bool Initialize(void);
	bool CreateTexture(int w, int h);
};

#endif