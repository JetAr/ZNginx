// MFC_appView.h : interface of the CMFC_appView class
//


#pragma once


class CMFC_appView : public CView
{
protected: // create from serialization only
	CMFC_appView();
	DECLARE_DYNCREATE(CMFC_appView)

	// Attributes
public:
	CMFC_appDoc* GetDocument() const;

	// Operations
public:

	// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

	// Implementation
public:
	virtual ~CMFC_appView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
};

#ifndef _DEBUG  // debug version in MFC_appView.cpp
inline CMFC_appDoc* CMFC_appView::GetDocument() const
{ return reinterpret_cast<CMFC_appDoc*>(m_pDocument); }
#endif

