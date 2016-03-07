// ParticleTestView.cpp : implementation of the CParticleTestView class
//

#include "stdafx.h"
#include "ParticleTest.h"

#include "ParticleTestDoc.h"
#include "ParticleTestView.h"


#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParticleTestView

IMPLEMENT_DYNCREATE(CParticleTestView, CView)

BEGIN_MESSAGE_MAP(CParticleTestView, CView)
	//{{AFX_MSG_MAP(CParticleTestView)
	ON_WM_ERASEBKGND()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_MAPMODE_MM_TEXT, OnUpdateMapmodeMmText)
	ON_COMMAND(ID_MAPMODE_MM_TEXT, OnMapmodeMmText)
	ON_COMMAND(ID_MAPMODE_LOENGLISH, OnMapmodeLoenglish)
	ON_UPDATE_COMMAND_UI(ID_MAPMODE_LOENGLISH, OnUpdateMapmodeLoenglish)
	ON_COMMAND(ID_MAPMODE_MMHIGHENGLISH, OnMapmodeMmhighenglish)
	ON_UPDATE_COMMAND_UI(ID_MAPMODE_MMHIGHENGLISH, OnUpdateMapmodeMmhighenglish)
	ON_COMMAND(ID_FLICKER_OFF, OnFlickerOff)
	ON_UPDATE_COMMAND_UI(ID_FLICKER_OFF, OnUpdateFlickerOff)
	ON_COMMAND(ID_FLICKER_ON, OnFlickerOn)
	ON_UPDATE_COMMAND_UI(ID_FLICKER_ON, OnUpdateFlickerOn)
	ON_COMMAND(ID_MAPMODE_HIMETRIC, OnMapmodeHimetric)
	ON_UPDATE_COMMAND_UI(ID_MAPMODE_HIMETRIC, OnUpdateMapmodeHimetric)
	ON_COMMAND(ID_MAPMODE_LOMETRIC, OnMapmodeLometric)
	ON_UPDATE_COMMAND_UI(ID_MAPMODE_LOMETRIC, OnUpdateMapmodeLometric)
	ON_COMMAND(ID_MAPMODE_TWIPS, OnMapmodeTwips)
	ON_UPDATE_COMMAND_UI(ID_MAPMODE_TWIPS, OnUpdateMapmodeTwips)
	ON_COMMAND(ID_MAPMODE_ANISOTRIPIC, OnMapmodeAnisotripic)
	ON_UPDATE_COMMAND_UI(ID_MAPMODE_ANISOTRIPIC, OnUpdateMapmodeAnisotripic)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleTestView construction/destruction

CParticleTestView::CParticleTestView()
{
	m_mapmode = MM_TEXT;
	m_flickerFlag = FALSE;
}

CParticleTestView::~CParticleTestView()
{
	m_container.Clean();
}

BOOL CParticleTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CParticleTestView drawing

CString MapMode(long mapmode)
{
	switch (mapmode) {
	case MM_TEXT: return _T("MM_TEXT");
	case MM_TWIPS: return _T("MM_TWIPS");
	case MM_LOENGLISH: return _T("MM_LOENGLISH");
	case MM_HIENGLISH: return _T("MM_HIENGLISH");
	case MM_LOMETRIC: return _T("MM_LOMETRIC");
	case MM_HIMETRIC: return _T("MM_HIMETRIC");
	case MM_ANISOTROPIC: return _T("MM_ANISOTROPIC");
	}
	return _T("???");
}

void CParticleTestView::OnDraw(CDC* dc)
{
	CRect bounds;
	CRect lBounds;
	GetClientRect(&bounds);
	lBounds = bounds;

	if (m_flickerFlag) {
		dc->DPtoLP(&lBounds);
		dc->FillSolidRect(lBounds, dc->GetBkColor());
		m_container.Draw(dc, bounds);
		dc->SetTextColor(RGB(255, 255, 255));
		dc->SetBkMode(TRANSPARENT);
		dc->SetTextAlign(TA_LEFT | TA_TOP);
		dc->TextOut(0, 0, MapMode(dc->GetMapMode()));
	} else {
		CMemDC pDC(dc);
		m_container.Draw(pDC, bounds);
		pDC->SetTextColor(RGB(255, 255, 255));
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextAlign(TA_LEFT | TA_TOP);
		pDC->TextOut(0, 0, MapMode(pDC->GetMapMode()));
	}
}

/////////////////////////////////////////////////////////////////////////////
// CParticleTestView diagnostics

#ifdef _DEBUG
void CParticleTestView::AssertValid() const
{
	CView::AssertValid();
}

void CParticleTestView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CParticleTestDoc* CParticleTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CParticleTestDoc)));
	return (CParticleTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParticleTestView message handlers

BOOL CParticleTestView::OnEraseBkgnd(CDC* pDC) 
{
	if (m_flickerFlag) {
		return CView::OnEraseBkgnd(pDC);
	}
	return FALSE;
}

void CParticleTestView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	pDC->SetBkColor(RGB(0, 0, 0));
	pDC->SetMapMode(m_mapmode);
	CView::OnPrepareDC(pDC, pInfo);
}

void CParticleTestView::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == 1) {
		CRect client;
		GetClientRect(&client);
		m_container.Update(client);
		Invalidate();
	}
}

void CParticleTestView::OnInitialUpdate() 
{
	CRect client;
	CView::OnInitialUpdate();

	srand( (unsigned)time( NULL ) );
	
	SetTimer(1, 33, NULL);

	GetClientRect(&client);

	m_container.Clean();

	m_container.Add(new CParentParticle(
					CPoint(client.left+client.Width()/2, client.top+client.Height()/2), 
					CPoint(2, -3), 
					RGB(255, 0, 0)));

	m_container.Add(new CParentParticle(
					CPoint(client.left+client.Width()/2, client.top+client.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(0, 255, 0)));

	m_container.Add(new CParentParticle(
					CPoint(client.left+client.Width()/2, client.top+client.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(100, 100, 255)));

	m_container.Add(new CParentParticle(
					CPoint(client.left+client.Width()/2, client.top+client.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(250, 200, 255)));

	m_container.Add(new CParentParticle(
					CPoint(client.left+client.Width()/2, client.top+client.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(243, 236, 288)));

	m_container.Add(new CParentParticle(
					CPoint(client.left+client.Width()/2, client.top+client.Height()/2), 
					CPoint(rand()%3, rand()%7-3), 
					RGB(255, 255, 255)));
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

BOOL CParticleTestView::DestroyWindow() 
{
	KillTimer(1);
	return CView::DestroyWindow();
}

void CParticleTestView::OnUpdateMapmodeMmText(CCmdUI* pCmdUI) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	pCmdUI->SetCheck(dc.GetMapMode() == MM_TEXT);
}

void CParticleTestView::OnMapmodeMmText() 
{
	m_mapmode = MM_TEXT;

	CRect client;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	GetClientRect(&client);
	dc.DPtoLP(&client);
	Invalidate();
	TRACE("%s - bounds (%d, %d, %d, %d)\n", MapMode(dc.GetMapMode()), client.left, client.top, client.right, client.bottom);
}

void CParticleTestView::OnMapmodeLoenglish() 
{
	m_mapmode = MM_LOENGLISH;

	CRect client;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	GetClientRect(&client);
	dc.DPtoLP(&client);
	Invalidate();
	TRACE("%s - bounds (%d, %d, %d, %d)\n", MapMode(dc.GetMapMode()), client.left, client.top, client.right, client.bottom);
}

void CParticleTestView::OnUpdateMapmodeLoenglish(CCmdUI* pCmdUI) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	pCmdUI->SetCheck(dc.GetMapMode() == MM_LOENGLISH);
}

void CParticleTestView::OnMapmodeMmhighenglish() 
{
	m_mapmode = MM_HIENGLISH;

	CRect client;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	GetClientRect(&client);
	dc.DPtoLP(&client);
	Invalidate();
	TRACE("%s - bounds (%d, %d, %d, %d)\n", MapMode(dc.GetMapMode()), client.left, client.top, client.right, client.bottom);
}

void CParticleTestView::OnUpdateMapmodeMmhighenglish(CCmdUI* pCmdUI) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	pCmdUI->SetCheck(dc.GetMapMode() == MM_HIENGLISH);
}

void CParticleTestView::OnFlickerOff() 
{
	m_flickerFlag = FALSE;
	
}

void CParticleTestView::OnUpdateFlickerOff(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(!m_flickerFlag);
}

void CParticleTestView::OnFlickerOn() 
{
	m_flickerFlag = TRUE;
}

void CParticleTestView::OnUpdateFlickerOn(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_flickerFlag);
}

void CParticleTestView::OnMapmodeHimetric() 
{
	m_mapmode = MM_HIMETRIC;

	CRect client;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	GetClientRect(&client);
	dc.DPtoLP(&client);
	Invalidate();
	TRACE("%s - bounds (%d, %d, %d, %d)\n", MapMode(dc.GetMapMode()), client.left, client.top, client.right, client.bottom);
}

void CParticleTestView::OnUpdateMapmodeHimetric(CCmdUI* pCmdUI) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	pCmdUI->SetCheck(dc.GetMapMode() == MM_HIMETRIC);
}

void CParticleTestView::OnMapmodeLometric() 
{
	m_mapmode = MM_LOMETRIC;

	CRect client;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	GetClientRect(&client);
	dc.DPtoLP(&client);
	Invalidate();
	TRACE("%s - bounds (%d, %d, %d, %d)\n", MapMode(dc.GetMapMode()), client.left, client.top, client.right, client.bottom);
	
}

void CParticleTestView::OnUpdateMapmodeLometric(CCmdUI* pCmdUI) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	pCmdUI->SetCheck(dc.GetMapMode() == MM_LOMETRIC);
}

void CParticleTestView::OnMapmodeTwips() 
{
	m_mapmode = MM_TWIPS;

	CRect client;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	GetClientRect(&client);
	dc.DPtoLP(&client);
	Invalidate();
	TRACE("%s - bounds (%d, %d, %d, %d)\n", MapMode(dc.GetMapMode()), client.left, client.top, client.right, client.bottom);	
}

void CParticleTestView::OnUpdateMapmodeTwips(CCmdUI* pCmdUI) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	pCmdUI->SetCheck(dc.GetMapMode() == MM_TWIPS);
}

void CParticleTestView::OnMapmodeAnisotripic() 
{
	m_mapmode = MM_ANISOTROPIC;

	CRect client;
	CClientDC dc(this);
	OnPrepareDC(&dc);
	GetClientRect(&client);
	dc.DPtoLP(&client);
	Invalidate();
	TRACE("%s - bounds (%d, %d, %d, %d)\n", MapMode(dc.GetMapMode()), client.left, client.top, client.right, client.bottom);		
}

void CParticleTestView::OnUpdateMapmodeAnisotripic(CCmdUI* pCmdUI) 
{
	CClientDC dc(this);
	OnPrepareDC(&dc);
	pCmdUI->SetCheck(dc.GetMapMode() == MM_ANISOTROPIC);
}
