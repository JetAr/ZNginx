// RemoteConDlg.h : header file
//
//{{AFX_INCLUDES()
#include "mscomm.h"
//}}AFX_INCLUDES

#if !defined(AFX_REMOTECONDLG_H__38CC5ED8_8844_42DF_A580_E6A976F8DAD7__INCLUDED_)
#define AFX_REMOTECONDLG_H__38CC5ED8_8844_42DF_A580_E6A976F8DAD7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRemoteConDlg dialog

class CRemoteConDlg : public CDialog
{
// Construction
public:
	CRemoteConDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRemoteConDlg)
	enum { IDD = IDD_REMOTECON_DIALOG };
	CMSComm	m_ctrlComm;
	CString	m_bluescore;
	CString	m_redscore;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRemoteConDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRemoteConDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnComm();
	afx_msg void OnButtonRedsend();
	afx_msg void OnButtonBluesend();
	afx_msg void OnButtonnext();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REMOTECONDLG_H__38CC5ED8_8844_42DF_A580_E6A976F8DAD7__INCLUDED_)
