// MSCommMultiPortDlg.h : header file
//
//{{AFX_INCLUDES()
#include "mscomm.h"
//}}AFX_INCLUDES

#if !defined(AFX_MSCOMMMULTIPORTDLG_H__FEFC79A6_5B50_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_MSCOMMMULTIPORTDLG_H__FEFC79A6_5B50_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CMSCommMultiPortDlg dialog

class CMSCommMultiPortDlg : public CDialog
{
// Construction
public:
	CMSCommMultiPortDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CMSCommMultiPortDlg)
	enum { IDD = IDD_MSCOMMMULTIPORT_DIALOG };
	CString	m_strEditCOM1RXData;
	CString	m_strEditCOM1TXData;
	CString	m_strEditCOM2TXData;
	CString	m_strEditCOM2RXData;
	CMSComm	m_ctrlMSCommCOM1;
	CMSComm	m_ctrlMSCommCOM2;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMSCommMultiPortDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CMSCommMultiPortDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonCom1send();
	afx_msg void OnButtonCom2send();
	afx_msg void OnCommCOM1();
	afx_msg void OnCommCOM2();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MSCOMMMULTIPORTDLG_H__FEFC79A6_5B50_11D8_870F_00E04C3F78CA__INCLUDED_)
