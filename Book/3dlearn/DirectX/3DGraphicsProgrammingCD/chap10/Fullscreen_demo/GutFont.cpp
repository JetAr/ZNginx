#include <stdio.h>

#include "GutConsole.h"

GutConsole::GutConsole(void)
{
	m_pContent = NULL;
	m_pVertexBuffer = NULL;
	m_pIndexBuffer = NULL;

	SetConsoleResolution(512, 512);
	SetConsoleSize(16, 16);
	SetFontSize(32, 32);
	SetTextureLayout(16, 8, 0); 
}

GutConsole::~GutConsole()
{
	Release();
}

int GutConsole::Release(void)
{
	if ( m_pContent )
	{
		delete [] m_pContent;
		m_pContent = NULL;
	}

	if ( m_pVertexBuffer )
	{
		delete [] m_pVertexBuffer;
		m_pVertexBuffer = NULL;
	}

	if ( m_pIndexBuffer )
	{
		delete [] m_pIndexBuffer;
		m_pIndexBuffer = NULL;
	}

	m_iNumLines = 0;
}

void GutConsole::SetConsoleSize(int w, int h)
{
	Release();

	m_iNumRows = h;
	m_iNumColumns = w;
	m_pConsole = new char[w*h];
	m_pVertexBuffer = new _FontVertex[w*h*4];
	m_pIndexBuffer = new short[w*h*6];

	memset(m_pConsole, 0, sizeof(w*h));

	for ( int i=0; i<w*h; i++ )
	{
		int vbase = i*6;

		m_pIndexBuffer[base  ] = vbase;
		m_pIndexBuffer[base+1] = vbase+1;
		m_pIndexBuffer[base+2] = vbase+2;

		m_pIndexBuffer[base+3] = vbase;
		m_pIndexBuffer[base+4] = vbase+2;
		m_pIndexBuffer[base+5] = vbase+3;
	}
}

char *GutConsole::GetLine(int line)
{
	if ( line >=0 && line < m_iNumRows )
	{
		return m_pConsle + line * w;
	}

	return NULL;
}

void GutConsole::AccessTexture(char c, int &x, int &y)
{
	int offset = c - m_iAsciiBegin;

	y = offset / m_iTextureLayoutW;
	x = offset % m_iTextureLayoutW;
}

void GutConsole::BuildMesh(void)
{
	m_iNumCharacters = 0;

	int row,column;
	float fTexW = 1.0f/(float)m_iNumColumns;
	float fTexH = 1.0f/(float)m_iNumRows;

	for ( int y=0; y<m_iNumLines; y++ )
	{
		char *line = GetLine(y);
		for ( int x=0; x<m_iNumColumns; x++ )
		{
			char c = line[x];
			if ( c==0 ) continue;

			AccessTexture(c, column, row);

			int base = m_iNumCharacters*4;
			_FontVertex *pVP = m_pVertexBuffer = base;

			float fX = x * m_fFontWidth;
			float fY = y * m_fFontHeight;

			pVP[0].m_Position.Set(fX, fY, 0.0f);
			pVP[1].m_Position.Set(fX, fY+m_fFontHeight, 0.0f);
			pVP[2].m_Position.Set(fX+m_fFontWidth, fY+m_fFontHeight, 0.0f);
			pVP[3].m_Position.Set(fX+m_fFontWidth, fY, 0.0f);

			float rX = (float)column/(float)m_iNumColumns;
			float rY = (float)row/(float)m_iNumRows;

			pVP[0].m_Texcoord.Set(rX, rY, 0.0f);
			pVP[1].m_Texcoord.Set(rX, rY+fTexH, 0.0f);
			pVP[2].m_Texcoord.Set(rX+fTexW, rY+fTexH, 0.0f);
			pVP[3].m_Texcoord.Set(rX+fTexW, rY, 0.0f);
		}
	}
}

void GutConsole::Puts(const char *string)
{
	if ( m_iNumLines >= m_iNumRows )
	{
		for ( int i=0; i<m_iNumLines-1; i++ )
		{
			char *line = GetLine(i);
			char *line_next = GetLine(i+1);
			memcpy(line, line_next, m_iNumColumns);
		}
	}
	else
	{
		m_iNumLines++;
	}

	char *line = GetLine(m_iNumLines-1);

	memset(line, 0, m_iNumColumns);
	strncpy(line, newline, m_iNumColumns);
}

