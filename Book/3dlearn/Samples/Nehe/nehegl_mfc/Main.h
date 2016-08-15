// Main.h: interface for the CMain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAIN_H__05A74DD2_C37A_4085_A563_7ED88F625CB7__INCLUDED_)
#define AFX_MAIN_H__05A74DD2_C37A_4085_A563_7ED88F625CB7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <gl\glaux.h>		// Header File For The Glaux Library

class CMain  
{
public:
	
	virtual BOOL Initialize();
	virtual void Deinitialize();
	virtual void Update (DWORD milliseconds);
	virtual void Draw();

public:
	BOOL KeyPressed( int nCode );
		
	

	CMain();
	virtual ~CMain();

	void DrawAxis();

	float angle,rot1,rot2;

	int LoadGLTextures();

	AUX_RGBImageRec *LoadBMP(char *Filename);

	GLuint	texture[1];			// Storage For One Texture ( NEW )


};

#endif // !defined(AFX_MAIN_H__05A74DD2_C37A_4085_A563_7ED88F625CB7__INCLUDED_)
