#if !defined(AFX_PARTICLETESTCTLPPG_H__3A4B6271_9EA6_11D3_AB75_00805FC73D05__INCLUDED_)
#define AFX_PARTICLETESTCTLPPG_H__3A4B6271_9EA6_11D3_AB75_00805FC73D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ParticleTestCtlPpg.h : Declaration of the CParticleTestCtlPropPage property page class.

////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlPropPage : See ParticleTestCtlPpg.cpp.cpp for implementation.

class CParticleTestCtlPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CParticleTestCtlPropPage)
	DECLARE_OLECREATE_EX(CParticleTestCtlPropPage)

// Constructor
public:
	CParticleTestCtlPropPage();

// Dialog Data
	//{{AFX_DATA(CParticleTestCtlPropPage)
	enum { IDD = IDD_PROPPAGE_PARTICLETESTCTL };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	//{{AFX_MSG(CParticleTestCtlPropPage)
		// NOTE - ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLETESTCTLPPG_H__3A4B6271_9EA6_11D3_AB75_00805FC73D05__INCLUDED)
