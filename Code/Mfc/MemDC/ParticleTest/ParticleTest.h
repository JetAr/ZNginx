// ParticleTest.h : main header file for the PARTICLETEST application
//

#if !defined(AFX_PARTICLETEST_H__FE681996_9DE8_11D3_AB75_00805FC73D05__INCLUDED_)
#define AFX_PARTICLETEST_H__FE681996_9DE8_11D3_AB75_00805FC73D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CParticleTestApp:
// See ParticleTest.cpp for the implementation of this class
//

class CParticleTestApp : public CWinApp
{
public:
	CParticleTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation
	//{{AFX_MSG(CParticleTestApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLETEST_H__FE681996_9DE8_11D3_AB75_00805FC73D05__INCLUDED_)
