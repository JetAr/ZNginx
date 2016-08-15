// MFC_app.h : main header file for the MFC_app application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CMFC_appApp:
// See MFC_app.cpp for the implementation of this class
//

class CMFC_appApp : public CWinApp
{
public:
	CMFC_appApp();


	// Overrides
public:
	virtual BOOL InitInstance();

	// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMFC_appApp theApp;