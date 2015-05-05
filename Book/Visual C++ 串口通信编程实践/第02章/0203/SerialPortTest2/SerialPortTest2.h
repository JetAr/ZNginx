// SerialPortTest2.h : main header file for the SERIALPORTTEST2 application
//

#if !defined(AFX_SERIALPORTTEST2_H__C0B3B644_5574_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SERIALPORTTEST2_H__C0B3B644_5574_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2App:
// See SerialPortTest2.cpp for the implementation of this class
//

class CSerialPortTest2App : public CWinApp
{
public:
	CSerialPortTest2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPortTest2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CSerialPortTest2App)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTTEST2_H__C0B3B644_5574_11D8_870F_00E04C3F78CA__INCLUDED_)
