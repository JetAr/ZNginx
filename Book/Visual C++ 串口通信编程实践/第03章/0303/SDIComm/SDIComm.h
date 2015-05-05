// SDIComm.h : main header file for the SDICOMM application
//

#if !defined(AFX_SDICOMM_H__CB8F5C3C_DA5B_48FF_9F1C_5239D041E135__INCLUDED_)
#define AFX_SDICOMM_H__CB8F5C3C_DA5B_48FF_9F1C_5239D041E135__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSDICommApp:
// See SDIComm.cpp for the implementation of this class
//

class CSDICommApp : public CWinApp
{
public:
	CSDICommApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDICommApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSDICommApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICOMM_H__CB8F5C3C_DA5B_48FF_9F1C_5239D041E135__INCLUDED_)
