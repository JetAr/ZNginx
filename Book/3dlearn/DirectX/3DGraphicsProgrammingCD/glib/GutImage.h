#ifndef _GUTIMAGE_
#define _GUTIMAGE_

#include <vector>

struct sImageInfo
{
	int m_iWidth, m_iHeight;
	bool m_bProcedure;

	sImageInfo(void)
	{
		m_iWidth = m_iHeight = 0;
		m_bProcedure = false;
	}

	sImageInfo(int w, int h, bool bProcedure)
	{
		m_iWidth = w;
		m_iHeight = h;
		m_bProcedure = bProcedure;
	}
};

class GutImage
{
public:
	GutImage(void) {}
	virtual ~GutImage(void) {}
	virtual int Load(const char *szFileName) = 0;
	virtual int GetBPP(void) = 0;
	virtual int GetWidth(void) = 0;
	virtual int GetHeight(void) = 0;
	virtual unsigned char *GetImg(void) = 0;
	virtual bool DownSampling(void) = 0;

protected:
};

#endif // _GUTIMAGE_