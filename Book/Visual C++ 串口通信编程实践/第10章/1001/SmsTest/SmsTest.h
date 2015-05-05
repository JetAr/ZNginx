// SmsTest.h : main header file for the SMSTEST application
//

#if !defined(AFX_SMSTEST_H__4D023EAA_E45B_47AC_B628_0076990CF09D__INCLUDED_)
#define AFX_SMSTEST_H__4D023EAA_E45B_47AC_B628_0076990CF09D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

#include "SmsTraffic.h"

/////////////////////////////////////////////////////////////////////////////
// CSmsTestApp:
// See SmsTest.cpp for the implementation of this class
//

class CSmsTestApp : public CWinApp
{
public:
	CSmsTestApp();

	CString m_strPort;
	CString m_strRate;
	CString m_strSmsc;

	CSmsTraffic* m_pSmsTraffic;

	BOOL CheckSettings();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmsTestApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSmsTestApp)
	afx_msg void OnAppAbout();
	afx_msg void OnSettings();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CSmsTestApp theApp;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSTEST_H__4D023EAA_E45B_47AC_B628_0076990CF09D__INCLUDED_)
