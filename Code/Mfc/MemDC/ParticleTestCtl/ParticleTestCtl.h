#if !defined(AFX_PARTICLETESTCTL_H__3A4B6267_9EA6_11D3_AB75_00805FC73D05__INCLUDED_)
#define AFX_PARTICLETESTCTL_H__3A4B6267_9EA6_11D3_AB75_00805FC73D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ParticleTestCtl.h : main header file for PARTICLETESTCTL.DLL

#if !defined( __AFXCTL_H__ )
	#error include 'afxctl.h' before including this file
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlApp : See ParticleTestCtl.cpp for implementation.

class CParticleTestCtlApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLETESTCTL_H__3A4B6267_9EA6_11D3_AB75_00805FC73D05__INCLUDED)
