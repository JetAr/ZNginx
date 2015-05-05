// SerialPortVxD.h : main header file for the SERIALPORTVXD application
//

#if !defined(AFX_SERIALPORTVXD_H__962EE1C6_E5F6_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SERIALPORTVXD_H__962EE1C6_E5F6_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CSerialPortVxDApp:
// See SerialPortVxD.cpp for the implementation of this class
//

class CSerialPortVxDApp : public CWinApp
{
public:
	CSerialPortVxDApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPortVxDApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CSerialPortVxDApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTVXD_H__962EE1C6_E5F6_11D8_870F_00E04C3F78CA__INCLUDED_)
