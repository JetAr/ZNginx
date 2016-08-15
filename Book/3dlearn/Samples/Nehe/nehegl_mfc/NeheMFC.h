// NeheMFC.h : main header file for the NEHEMFC application
//

#if !defined(AFX_NEHEMFC_H__623A202B_7393_41C3_B342_AF7D2D611C23__INCLUDED_)
#define AFX_NEHEMFC_H__623A202B_7393_41C3_B342_AF7D2D611C23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNeheMFCApp:
// See NeheMFC.cpp for the implementation of this class
//
#include "NeheWindow.h"
#include "Main.h"

#include "Render.h"

class CNeheMFCApp : public CWinApp
{
public:
	void ToggleFullScreen();
	void TerminateApplication ();
	void DestroyOpenGLWindow();
	void ReshapeGL (int width, int height);
	BOOL ChangeScreenResolution (int width, int height, int bitsPerPixel);
	BOOL RegisterWindowClass ();
	BOOL CreateOpenGLWindow();
	CNeheMFCApp();


	CNeheWindow	m_wndMain;
	Render		m_render;
	BOOL		m_isProgramLooping;
	BOOL		m_createFullScreen;

	int					m_Width;						// Width
	int					m_Height;						// Height
	int					m_bitsPerPixel;					// Bits Per Pixel
	BOOL				m_isFullScreen;					// FullScreen?
	BOOL				m_isVisible;

	CDC*				m_pDC;
	HGLRC				m_hRC;

	DWORD				m_lastTickCount;
	char				keyDown [256];

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNeheMFCApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNeheMFCApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEHEMFC_H__623A202B_7393_41C3_B342_AF7D2D611C23__INCLUDED_)
