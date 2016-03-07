// ParticleTestCtlCtl.cpp : Implementation of the CParticleTestCtlCtrl ActiveX Control class.

#include "stdafx.h"
#include "ParticleTestCtl.h"
#include "ParticleTestCtlCtl.h"
#include "ParticleTestCtlPpg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CParticleTestCtlCtrl, COleControl)


/////////////////////////////////////////////////////////////////////////////
// Message map

BEGIN_MESSAGE_MAP(CParticleTestCtlCtrl, COleControl)
	//{{AFX_MSG_MAP(CParticleTestCtlCtrl)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Dispatch map

BEGIN_DISPATCH_MAP(CParticleTestCtlCtrl, COleControl)
	//{{AFX_DISPATCH_MAP(CParticleTestCtlCtrl)
	DISP_PROPERTY_NOTIFY(CParticleTestCtlCtrl, "FlickerFlag", m_flickerFlag, OnFlickerFlagChanged, VT_BOOL)
	//}}AFX_DISPATCH_MAP
	DISP_FUNCTION_ID(CParticleTestCtlCtrl, "AboutBox", DISPID_ABOUTBOX, AboutBox, VT_EMPTY, VTS_NONE)
END_DISPATCH_MAP()


/////////////////////////////////////////////////////////////////////////////
// Event map

BEGIN_EVENT_MAP(CParticleTestCtlCtrl, COleControl)
	//{{AFX_EVENT_MAP(CParticleTestCtlCtrl)
	// NOTE - ClassWizard will add and remove event map entries
	//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_EVENT_MAP
END_EVENT_MAP()


/////////////////////////////////////////////////////////////////////////////
// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CParticleTestCtlCtrl, 1)
	PROPPAGEID(CParticleTestCtlPropPage::guid)
END_PROPPAGEIDS(CParticleTestCtlCtrl)


/////////////////////////////////////////////////////////////////////////////
// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CParticleTestCtlCtrl, "PARTICLETESTCTL.ParticleTestCtlCtrl.1",
	0x3a4b6261, 0x9ea6, 0x11d3, 0xab, 0x75, 0, 0x80, 0x5f, 0xc7, 0x3d, 0x5)


/////////////////////////////////////////////////////////////////////////////
// Type library ID and version

IMPLEMENT_OLETYPELIB(CParticleTestCtlCtrl, _tlid, _wVerMajor, _wVerMinor)


/////////////////////////////////////////////////////////////////////////////
// Interface IDs

const IID BASED_CODE IID_DParticleTestCtl =
		{ 0x3a4b625f, 0x9ea6, 0x11d3, { 0xab, 0x75, 0, 0x80, 0x5f, 0xc7, 0x3d, 0x5 } };
const IID BASED_CODE IID_DParticleTestCtlEvents =
		{ 0x3a4b6260, 0x9ea6, 0x11d3, { 0xab, 0x75, 0, 0x80, 0x5f, 0xc7, 0x3d, 0x5 } };


/////////////////////////////////////////////////////////////////////////////
// Control type information

static const DWORD BASED_CODE _dwParticleTestCtlOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CParticleTestCtlCtrl, IDS_PARTICLETESTCTL, _dwParticleTestCtlOleMisc)


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl::CParticleTestCtlCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CParticleTestCtlCtrl

BOOL CParticleTestCtlCtrl::CParticleTestCtlCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_PARTICLETESTCTL,
			IDB_PARTICLETESTCTL,
			afxRegApartmentThreading,
			_dwParticleTestCtlOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl::CParticleTestCtlCtrl - Constructor

CParticleTestCtlCtrl::CParticleTestCtlCtrl()
{
	InitializeIIDs(&IID_DParticleTestCtl, &IID_DParticleTestCtlEvents);

	// TODO: Initialize your control's instance data here.
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl::~CParticleTestCtlCtrl - Destructor

CParticleTestCtlCtrl::~CParticleTestCtlCtrl()
{
	m_container.Clean();
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl::OnDraw - Drawing function

void CParticleTestCtlCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	m_bounds = rcBounds;

	if (m_container.GetSize() == 0) {
		Initialize(rcBounds);
		SetTimer(1, 33, NULL);
	} 

	if (m_flickerFlag) {
		pdc->FillSolidRect(rcBounds, RGB(0, 0, 0));
		m_container.Draw(pdc, rcBounds);
	} else {
		CMemDC pDC(pdc, &rcBounds);
		pDC->FillSolidRect(rcBounds, RGB(0, 0, 0));
		m_container.Draw(pDC, rcBounds);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl::DoPropExchange - Persistence support

void CParticleTestCtlCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);
	PX_Bool(pPX, _T("FlickerFlag"), m_flickerFlag, FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl::OnResetState - Reset control to default state

void CParticleTestCtlCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl::AboutBox - Display an "About" box to the user

void CParticleTestCtlCtrl::AboutBox()
{
	CDialog dlgAbout(IDD_ABOUTBOX_PARTICLETESTCTL);
	dlgAbout.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl message handlers

BOOL CParticleTestCtlCtrl::OnEraseBkgnd(CDC* pDC) 
{
	if (m_flickerFlag)
		return COleControl::OnEraseBkgnd(pDC);

	return FALSE;
}

void CParticleTestCtlCtrl::OnFlickerFlagChanged() 
{
	InvalidateControl();
}

void CParticleTestCtlCtrl::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1) {
		m_container.Update(m_bounds);
		InvalidateControl();
	}
}

BOOL CParticleTestCtlCtrl::DestroyWindow() 
{
	KillTimer(1);
	return COleControl::DestroyWindow();
}

int CParticleContainer::GetSize() const {return m_array.GetSize();}
CParticle* CParticleContainer::GetAt(int index) {return m_array.GetAt(index);}
void CParticleContainer::RemoveAt(int index) {m_array.RemoveAt(index);}
void CParticleContainer::Add(CParticle* value) {m_array.Add(value);}
void CParticleContainer::Clean()
{
	for (int i = GetSize()-1; i >= 0; i--) {
		if (GetAt(i) != NULL) delete GetAt(i);
		RemoveAt(i);
	}
}
void CParticleContainer::Update(const CRect& bounds) 
{
	CParticleContainer container;
	int size = GetSize();

	for (int i = 0; i < size; i++) {
		CParticle* pParticle = GetAt(i);
		if (pParticle != NULL && pParticle->UpdateTick(bounds, NULL, container)) {
			RemoveAt(i);
			delete pParticle;
			size = GetSize();
			continue;
		}
	}

	size = container.GetSize();

	for (i = 0; i < size; i++) {
		Add(container.GetAt(i));
	}
}

void CParticleContainer::Draw(CDC* pDC, const CRect& bounds) 
{
	for (int i = 0; i < GetSize(); i++) {
		CParticle* pParticle = GetAt(i);
		if (pParticle != NULL) pParticle->Draw(pDC, bounds);
	}
}


CParentParticle::CParentParticle(CPoint loc, CPoint velocity, COLORREF color) : CParticle(loc, velocity, color)
{
	m_pen = new CPen(PS_SOLID, 1, color);
	m_updateTick = 0;
}
CParentParticle::~CParentParticle() 
{
	if (m_pen != NULL) delete m_pen;
}

double Random()
{
	static double sum = 0.0;
	static double count = 0.0;

	double retval = ((double) rand())/((double) RAND_MAX)-0.5;
	sum += retval;
	count += 1.0;
	return retval;
}

BOOL CParentParticle::UpdateTick(const CRect& bounds, CParticle* parent, CParticleContainer& decayList) 
{
	CPoint prev = m_loc;

	m_loc.x += m_velocity.x;
	m_loc.y += m_velocity.y;

	if (m_loc.x >= bounds.right) {
		m_loc.x = m_loc.x - bounds.right;
	}

	if (m_loc.x < bounds.left) {
		m_loc.x = bounds.right - (m_loc.x - bounds.left);
	}

	if (m_loc.y >= bounds.bottom) {
		m_loc.y = m_loc.y - bounds.bottom;
	}

	if (m_loc.y < bounds.top) {
		m_loc.y = bounds.bottom - (m_loc.y - bounds.top);
	}

	m_updateTick++;

	if (m_updateTick % 10 == 0) {
		m_velocity.x += (long) (5.0*Random());
		m_velocity.y += (long) (5.0*Random());
		if (m_velocity.x > 4) m_velocity.x = 4;
		if (m_velocity.x < -4) m_velocity.x = -4;
		if (m_velocity.y > 4) m_velocity.y = 4;
		if (m_velocity.y < -4) m_velocity.y = -4;
	}

	for (int i = 0; i < rand()%3; i++) {
		decayList.Add(new CChildParticle(m_loc, CPoint(0, 0), m_color));
	}

	return FALSE;
}

BOOL CParentParticle::Draw(CDC* pDC, const CRect& bounds) 
{
	CPoint lpPoint = m_loc;

	pDC->DPtoLP(&lpPoint);
	CPen* pOldPen = pDC->SelectObject(m_pen);
	pDC->MoveTo(lpPoint);
	lpPoint = CPoint(m_loc.x+m_velocity.x, m_loc.y+m_velocity.y);
	pDC->DPtoLP(&lpPoint);
	pDC->LineTo(lpPoint);
	pDC->SelectObject(pOldPen);
	return TRUE;
}


CChildParticle::CChildParticle(CPoint loc, CPoint velocity, COLORREF color) : CParticle(loc, velocity, color)
{
	m_updateTick = 0;
	m_drawColor = color;
}

CChildParticle::~CChildParticle() {}

BOOL CChildParticle::UpdateTick(const CRect& bounds, CParticle* parent, CParticleContainer& decayList) 
{
	const long decay = 400;
	const long colorDecay = 275;
	
	m_updateTick++;

	m_drawColor = RGB(GetRValue(m_color)*(colorDecay-m_updateTick)/colorDecay,
						GetGValue(m_color)*(colorDecay-m_updateTick)/colorDecay,
						GetBValue(m_color)*(colorDecay-m_updateTick)/colorDecay);

	m_loc.x += (long) (Random()*2.5);
	m_loc.y += (m_loc.y + 600)/decay;

	return !bounds.PtInRect(m_loc) || m_drawColor == RGB(0, 0, 0);
}

BOOL CChildParticle::Draw(CDC* pDC, const CRect& bounds) 
{
	CPoint lpPoint;
	CPen pen(PS_SOLID, 1, m_drawColor);
	CPen* pOldPen = pDC->SelectObject(&pen);
	lpPoint = m_loc;
	pDC->DPtoLP(&lpPoint);
	pDC->MoveTo(lpPoint);
	lpPoint = CPoint(m_loc.x+m_velocity.x+1, m_loc.y+m_velocity.y+1);
	pDC->DPtoLP(&lpPoint);
	pDC->LineTo(lpPoint);
	pDC->SelectObject(pOldPen);
	return TRUE;
}

void CParticleTestCtlCtrl::Initialize(const CRect &bounds)
{
	m_container.Clean();

	m_container.Add(new CParentParticle(
					CPoint(bounds.left+bounds.Width()/2, bounds.top+bounds.Height()/2), 
					CPoint(2, -3), 
					RGB(255, 0, 0)));

	m_container.Add(new CParentParticle(
					CPoint(bounds.left+bounds.Width()/2, bounds.top+bounds.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(0, 255, 0)));

	m_container.Add(new CParentParticle(
					CPoint(bounds.left+bounds.Width()/2, bounds.top+bounds.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(100, 100, 255)));

	m_container.Add(new CParentParticle(
					CPoint(bounds.left+bounds.Width()/2, bounds.top+bounds.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(250, 200, 255)));

	m_container.Add(new CParentParticle(
					CPoint(bounds.left+bounds.Width()/2, bounds.top+bounds.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(243, 236, 288)));

	m_container.Add(new CParentParticle(
					CPoint(bounds.left+bounds.Width()/2, bounds.top+bounds.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(255, 255, 255)));
}
