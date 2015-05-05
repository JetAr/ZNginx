// SCommTest.h : main header file for the SCOMMTEST application
//

#if !defined(AFX_SCOMMTEST_H__22F2B144_69C2_11D5_870E_00E04C3F78CA__INCLUDED_)
#define AFX_SCOMMTEST_H__22F2B144_69C2_11D5_870E_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSCommTestApp:
// See SCommTest.cpp for the implementation of this class
//

class CSCommTestApp : public CWinApp
{
public:
	CSCommTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCommTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSCommTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCOMMTEST_H__22F2B144_69C2_11D5_870E_00E04C3F78CA__INCLUDED_)
