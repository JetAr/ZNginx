// ParticleTestCtlPpg.cpp : Implementation of the CParticleTestCtlPropPage property page class.

#include "stdafx.h"
#include "ParticleTestCtl.h"
#include "ParticleTestCtlPpg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CParticleTestCtlPropPage, COlePropertyPage)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CParticleTestCtlPropPage, COlePropertyPage)
	//{{AFX_MSG_MAP(CParticleTestCtlPropPage)
	// NOTE - ClassWizard will add and remove message map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CParticleTestCtlPropPage, "PARTICLETESTCTL.ParticleTestCtlPropPage.1",
	0x3a4b6262, 0x9ea6, 0x11d3, 0xab, 0x75, 0, 0x80, 0x5f, 0xc7, 0x3d, 0x5)


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlPropPage::CParticleTestCtlPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CParticleTestCtlPropPage

BOOL CParticleTestCtlPropPage::CParticleTestCtlPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_PARTICLETESTCTL_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlPropPage::CParticleTestCtlPropPage - Constructor

CParticleTestCtlPropPage::CParticleTestCtlPropPage() :
	COlePropertyPage(IDD, IDS_PARTICLETESTCTL_PPG_CAPTION)
{
	//{{AFX_DATA_INIT(CParticleTestCtlPropPage)
	// NOTE: ClassWizard will add member initialization here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_INIT
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlPropPage::DoDataExchange - Moves data between page and properties

void CParticleTestCtlPropPage::DoDataExchange(CDataExchange* pDX)
{
	//{{AFX_DATA_MAP(CParticleTestCtlPropPage)
	// NOTE: ClassWizard will add DDP, DDX, and DDV calls here
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA_MAP
	DDP_PostProcessing(pDX);
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlPropPage message handlers
