// ParticleTestView.h : interface of the CParticleTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLETESTVIEW_H__FE68199F_9DE8_11D3_AB75_00805FC73D05__INCLUDED_)
#define AFX_PARTICLETESTVIEW_H__FE68199F_9DE8_11D3_AB75_00805FC73D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>

class CParticle;

class CParticleContainer {
private:
	CArray<CParticle*, CParticle*> m_array;
public:
	int GetSize() const;
	CParticle* GetAt(int index);
	CParticle* operator[](int index) {return GetAt(index);}
	void RemoveAt(int index);
	void Add(CParticle* value);
	void Update(const CRect& bounds);
	void Draw(CDC* pDC, const CRect& bounds);
	void Clean();
};

class CParticle {
protected:
	CPoint		m_loc;
	COLORREF	m_color;
	CPoint		m_velocity;
public:
	CParticle(CPoint loc, CPoint velocity, COLORREF color) :
		m_color(color), m_loc(loc), m_velocity(velocity) {}
	virtual ~CParticle() {}
	virtual BOOL UpdateTick(const CRect& bounds, CParticle* parent, CParticleContainer& decayList) = 0;
	virtual BOOL Draw(CDC* pDC, const CRect& bounds) = 0;
};

class CParentParticle : public CParticle
{
public:	
	CPen*	m_pen;
	long	m_updateTick;
public:
	CParentParticle(CPoint loc, CPoint velocity, COLORREF color);
	~CParentParticle();
	BOOL UpdateTick(const CRect& bounds, CParticle* parent, CParticleContainer& decayList);
	BOOL Draw(CDC* pDC, const CRect& bounds);
};

class CChildParticle : public CParticle
{
protected:
	long		m_updateTick;
	COLORREF	m_drawColor;
public:
	CChildParticle(CPoint loc, CPoint velocity, COLORREF color);
	~CChildParticle();
	BOOL UpdateTick(const CRect& bounds, CParticle* parent, CParticleContainer& decayList);
	BOOL Draw(CDC* pDC, const CRect& bounds);
};

class CParticleTestView : public CView
{
protected: // create from serialization only
	CParticleTestView();
	DECLARE_DYNCREATE(CParticleTestView)

// Attributes
public:
	CParticleTestDoc* GetDocument();
	CParticleContainer	m_container;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	virtual void OnInitialUpdate();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	BOOL m_flickerFlag;
	long m_mapmode;
	virtual ~CParticleTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CParticleTestView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUpdateMapmodeMmText(CCmdUI* pCmdUI);
	afx_msg void OnMapmodeMmText();
	afx_msg void OnMapmodeLoenglish();
	afx_msg void OnUpdateMapmodeLoenglish(CCmdUI* pCmdUI);
	afx_msg void OnMapmodeMmhighenglish();
	afx_msg void OnUpdateMapmodeMmhighenglish(CCmdUI* pCmdUI);
	afx_msg void OnFlickerOff();
	afx_msg void OnUpdateFlickerOff(CCmdUI* pCmdUI);
	afx_msg void OnFlickerOn();
	afx_msg void OnUpdateFlickerOn(CCmdUI* pCmdUI);
	afx_msg void OnMapmodeHimetric();
	afx_msg void OnUpdateMapmodeHimetric(CCmdUI* pCmdUI);
	afx_msg void OnMapmodeLometric();
	afx_msg void OnUpdateMapmodeLometric(CCmdUI* pCmdUI);
	afx_msg void OnMapmodeTwips();
	afx_msg void OnUpdateMapmodeTwips(CCmdUI* pCmdUI);
	afx_msg void OnMapmodeAnisotripic();
	afx_msg void OnUpdateMapmodeAnisotripic(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ParticleTestView.cpp
inline CParticleTestDoc* CParticleTestView::GetDocument()
   { return (CParticleTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLETESTVIEW_H__FE68199F_9DE8_11D3_AB75_00805FC73D05__INCLUDED_)
