#ifndef _GUTFONT_OPENGL_
#define _GUTFONT_OPENGL_

#include <GL/gl.h>

#include "GutFont.h"
#include "GutFontUnicodeWin32.h"

class CGutFontOpenGL : public CGutFont
{
private:
	GLuint m_FontTexture;

public:
	CGutFontOpenGL(void);
	~CGutFontOpenGL();

	virtual bool LoadTexture(const char *filename);
	virtual void Render(void);
};

class CGutFontUniCodeOpenGL : public CGutFontUniCode
{
private:
	GLuint m_FontTexture;

public:
	CGutFontUniCodeOpenGL(void);
	~CGutFontUniCodeOpenGL();

	virtual bool AccessTexture(WCHAR c, int &x, int &y);
	virtual bool CreateTexture(int w, int h);
	virtual void Render(void);
};

#endif // _GUTFONT_DX9_