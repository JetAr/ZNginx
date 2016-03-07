#if !defined(AFX_PARTICLETESTCTLCTL_H__3A4B626F_9EA6_11D3_AB75_00805FC73D05__INCLUDED_)
#define AFX_PARTICLETESTCTLCTL_H__3A4B626F_9EA6_11D3_AB75_00805FC73D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// ParticleTestCtlCtl.h : Declaration of the CParticleTestCtlCtrl ActiveX Control class.

/////////////////////////////////////////////////////////////////////////////
// CParticleTestCtlCtrl : See ParticleTestCtlCtl.cpp for implementation.

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

class CParticleTestCtlCtrl : public COleControl
{
	DECLARE_DYNCREATE(CParticleTestCtlCtrl)

// Constructor
public:
	CParticleTestCtlCtrl();
	CParticleContainer	m_container;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleTestCtlCtrl)
	public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();
	virtual BOOL DestroyWindow();
	//}}AFX_VIRTUAL

// Implementation
protected:
	~CParticleTestCtlCtrl();

	DECLARE_OLECREATE_EX(CParticleTestCtlCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CParticleTestCtlCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CParticleTestCtlCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CParticleTestCtlCtrl)		// Type name and misc status

// Message maps
	//{{AFX_MSG(CParticleTestCtlCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	//{{AFX_DISPATCH(CParticleTestCtlCtrl)
	BOOL m_flickerFlag;
	afx_msg void OnFlickerFlagChanged();
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()

	afx_msg void AboutBox();

// Event maps
	//{{AFX_EVENT(CParticleTestCtlCtrl)
	//}}AFX_EVENT
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	CRect m_bounds;
	void Initialize(const CRect& bounds);
	enum {
	//{{AFX_DISP_ID(CParticleTestCtlCtrl)
	dispidFlickerFlag = 1L,
	//}}AFX_DISP_ID
	};
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLETESTCTLCTL_H__3A4B626F_9EA6_11D3_AB75_00805FC73D05__INCLUDED)
