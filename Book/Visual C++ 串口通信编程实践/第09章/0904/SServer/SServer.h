// SServer.h : main header file for the SSERVER application
//

#if !defined(AFX_SSERVER_H__BFEB8164_42F6_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SSERVER_H__BFEB8164_42F6_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSServerApp:
// See SServer.cpp for the implementation of this class
//

class CSServerApp : public CWinApp
{
public:
	CSServerApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSServerApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSServerApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SSERVER_H__BFEB8164_42F6_11D8_870F_00E04C3F78CA__INCLUDED_)
