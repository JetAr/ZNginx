// CommSocketDlg.h : header file
//

#if !defined(AFX_COMMSOCKETDLG_H__8BF5C184_8AC4_4104_94AB_7142D1A6C80E__INCLUDED_)
#define AFX_COMMSOCKETDLG_H__8BF5C184_8AC4_4104_94AB_7142D1A6C80E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CCommSocketDlg dialog

#include "WSocket.h"

class CCommSocketDlg : public CDialog
{
// Construction
public:
	CWSocket m_wskServer;
	CWSocket m_wskClient;

	CCommSocketDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CCommSocketDlg)
	enum { IDD = IDD_COMMSOCKET_DIALOG };
	UINT	m_unEditServerPort;
	CString	m_strEditConnIP;
	UINT	m_unEditConnPort;
	CString	m_strEditRecvData;
	CString	m_strEditServerData;
	CString	m_strEditSendData;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCommSocketDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL m_bServer;
	BOOL m_bConnected;

	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCommSocketDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonConnect();
	afx_msg void OnButtonDisconnect();
	afx_msg void OnButtonSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COMMSOCKETDLG_H__8BF5C184_8AC4_4104_94AB_7142D1A6C80E__INCLUDED_)
