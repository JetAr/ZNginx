// NNTest.h : main header file for the NNTEST application
//

#if !defined(AFX_NNTEST_H__9282EE61_53DA_4065_92A2_5055C22E47E0__INCLUDED_)
#define AFX_NNTEST_H__9282EE61_53DA_4065_92A2_5055C22E47E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CNNTestApp:
// See NNTest.cpp for the implementation of this class
//

class CNNTestApp : public CWinApp
{
public:
	CNNTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNNTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CNNTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NNTEST_H__9282EE61_53DA_4065_92A2_5055C22E47E0__INCLUDED_)
