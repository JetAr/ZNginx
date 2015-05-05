// DisplayConDlg.h : header file
//
//{{AFX_INCLUDES()
#include "mscomm.h"
//}}AFX_INCLUDES

#include "editex.hxx"

#if !defined(AFX_DISPLAYCONDLG_H__5C7FC587_A1F1_47C6_8474_3AB57F8EAB9E__INCLUDED_)
#define AFX_DISPLAYCONDLG_H__5C7FC587_A1F1_47C6_8474_3AB57F8EAB9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CDisplayConDlg dialog

class CDisplayConDlg : public CDialog
{
// Construction
public:
	CDisplayConDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDisplayConDlg)
	enum { IDD = IDD_DISPLAYCON_DIALOG };
	CMSComm	m_ctrlComm;
	CString	m_redscore;
	CString	m_bluescore;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplayConDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
public:
	CEditEx   m_edit;
	CEditEx   m_edit2;
	// Generated message map functions
	//{{AFX_MSG(CDisplayConDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnComm();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYCONDLG_H__5C7FC587_A1F1_47C6_8474_3AB57F8EAB9E__INCLUDED_)
