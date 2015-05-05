// CommSocket.h : main header file for the COMMSOCKET application
//

#if !defined(AFX_COMMSOCKET_H__488DC7F7_0A36_4097_BA51_1AF140FBAD01__INCLUDED_)
#define AFX_COMMSOCKET_H__488DC7F7_0A36_4097_BA51_1AF140FBAD01__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCommSocketApp:
// See CommSocket.cpp for the implementation of this class
//

class CCommSocketApp : public CWinApp
{
public:
	CCommSocketApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommSocketApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCommSocketApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMSOCKET_H__488DC7F7_0A36_4097_BA51_1AF140FBAD01__INCLUDED_)
