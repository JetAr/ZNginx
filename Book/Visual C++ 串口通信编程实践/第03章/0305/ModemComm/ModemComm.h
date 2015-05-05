// ModemComm.h : main header file for the MODEMCOMM application
//

#if !defined(AFX_MODEMCOMM_H__1DDDBC13_D6D3_40A6_B6EF_9FF922A8B78E__INCLUDED_)
#define AFX_MODEMCOMM_H__1DDDBC13_D6D3_40A6_B6EF_9FF922A8B78E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CModemCommApp:
// See ModemComm.cpp for the implementation of this class
//

class CModemCommApp : public CWinApp
{
public:
	CModemCommApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModemCommApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CModemCommApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODEMCOMM_H__1DDDBC13_D6D3_40A6_B6EF_9FF922A8B78E__INCLUDED_)
