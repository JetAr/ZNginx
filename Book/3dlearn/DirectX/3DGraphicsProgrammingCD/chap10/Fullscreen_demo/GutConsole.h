#ifndef _GUT_FONT_
#define _GUT_FONT_

struct _VertexVT
{
	float m_Position[3];
	float m_Tex[2];
};

class GutConsole
{
public:

public:
	virtual bool LoadTexture(void);
	void SetConsoleResolution(float width, float height);
	bool SetPosition(float x, float y);
	int  Print(const char *fmt, ...);
};

#endif