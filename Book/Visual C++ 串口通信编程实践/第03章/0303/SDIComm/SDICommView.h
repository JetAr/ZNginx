// SDICommView.h : interface of the CSDICommView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICOMMVIEW_H__C97BA3D3_0F5F_4DD7_8237_5227C645AF90__INCLUDED_)
#define AFX_SDICOMMVIEW_H__C97BA3D3_0F5F_4DD7_8237_5227C645AF90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "mscomm.h"

class CSDICommView : public CView
{
protected: // create from serialization only
	CSDICommView();
	DECLARE_DYNCREATE(CSDICommView)

// Attributes
public:
	CSDICommDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDICommView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	CMSComm m_MSComm;   //MSComm类对象
	virtual ~CSDICommView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSDICommView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnComm();     //事件处理函数
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SDICommView.cpp
inline CSDICommDoc* CSDICommView::GetDocument()
   { return (CSDICommDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICOMMVIEW_H__C97BA3D3_0F5F_4DD7_8237_5227C645AF90__INCLUDED_)
