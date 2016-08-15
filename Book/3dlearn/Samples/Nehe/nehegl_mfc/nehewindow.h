#if !defined(AFX_MAINWND_H__0D1BEEB0_1AA6_4170_98F4_B73A52B62176__INCLUDED_)
#define AFX_MAINWND_H__0D1BEEB0_1AA6_4170_98F4_B73A52B62176__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNeheWindow window

class CNeheWindow : public CWnd
{
// Construction
public:
	CNeheWindow();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNeheWindow)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNeheWindow();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNeheWindow)
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnToggleFullScreen(WPARAM w,LPARAM l);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINWND_H__0D1BEEB0_1AA6_4170_98F4_B73A52B62176__INCLUDED_)
