// OgreMFC.h : main header file for the OgreMFC application
//
#pragma once

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// COgreMFCApp:
// See OgreMFC.cpp for the implementation of this class
//

class COgreMFCApp : public CWinApp
{
public:
	COgreMFCApp();
	~COgreMFCApp();


	// Overrides
public:
	virtual BOOL InitInstance();

	inline Ogre::Root *GetOgreRoot() {return mRoot;}

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
	Ogre::Root         *mRoot;

};

extern COgreMFCApp theApp;