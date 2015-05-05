// DisplayCon.h : main header file for the DISPLAYCON application
//

#if !defined(AFX_DISPLAYCON_H__F56BD51C_3A95_4D70_9DBB_C3E9A101ABE8__INCLUDED_)
#define AFX_DISPLAYCON_H__F56BD51C_3A95_4D70_9DBB_C3E9A101ABE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDisplayConApp:
// See DisplayCon.cpp for the implementation of this class
//

class CDisplayConApp : public CWinApp
{
public:
	CDisplayConApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayConApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDisplayConApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYCON_H__F56BD51C_3A95_4D70_9DBB_C3E9A101ABE8__INCLUDED_)
