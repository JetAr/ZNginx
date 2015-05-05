// RemoteCon.h : main header file for the REMOTECON application
//

#if !defined(AFX_REMOTECON_H__BCEBEEA2_34DB_4D6F_B41A_AF44AC57EE1A__INCLUDED_)
#define AFX_REMOTECON_H__BCEBEEA2_34DB_4D6F_B41A_AF44AC57EE1A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CRemoteConApp:
// See RemoteCon.cpp for the implementation of this class
//

class CRemoteConApp : public CWinApp
{
public:
	CRemoteConApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteConApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CRemoteConApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTECON_H__BCEBEEA2_34DB_4D6F_B41A_AF44AC57EE1A__INCLUDED_)
