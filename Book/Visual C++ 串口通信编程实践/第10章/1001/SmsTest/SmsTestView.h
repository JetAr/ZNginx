// SmsTestView.h : interface of the CSmsTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMSTESTVIEW_H__761E3290_6C42_4F91_98C7_B20C5F7E9DA4__INCLUDED_)
#define AFX_SMSTESTVIEW_H__761E3290_6C42_4F91_98C7_B20C5F7E9DA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSmsTestView : public CListView
{
protected: // create from serialization only
	CSmsTestView();
	DECLARE_DYNCREATE(CSmsTestView)

// Attributes
public:
	CSmsTestDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmsTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL DestroyWindow();
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSmsTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSmsTestView)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SmsTestView.cpp
inline CSmsTestDoc* CSmsTestView::GetDocument()
   { return (CSmsTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSTESTVIEW_H__761E3290_6C42_4F91_98C7_B20C5F7E9DA4__INCLUDED_)
