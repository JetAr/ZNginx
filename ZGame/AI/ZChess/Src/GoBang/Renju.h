// Renju.h : main header file for the RENJU application
//

#if !defined(AFX_RENJU_H__A773B361_DB46_46A9_9128_B2576FEFD6F8__INCLUDED_)
#define AFX_RENJU_H__A773B361_DB46_46A9_9128_B2576FEFD6F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "Define.h"

/////////////////////////////////////////////////////////////////////////////
// CRenjuApp:
// See Renju.cpp for the implementation of this class
//

class CRenjuApp : public CWinApp
{
public:
    CRenjuApp();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRenjuApp)
public:
    virtual BOOL InitInstance();
    //}}AFX_VIRTUAL

// Implementation

    //{{AFX_MSG(CRenjuApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENJU_H__A773B361_DB46_46A9_9128_B2576FEFD6F8__INCLUDED_)
