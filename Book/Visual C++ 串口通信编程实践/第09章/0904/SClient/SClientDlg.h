// SClientDlg.h : header file
//

#if !defined(AFX_SCLIENTDLG_H__05807406_3669_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SCLIENTDLG_H__05807406_3669_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CSClientDlg dialog

#include "MySocket.h"   //添加类说明头文件

class CSClientDlg : public CDialog
{
// Construction
public:
	CString m_strRXDataTemp;
	void UpdateRXData();
	BOOL m_bLinked;           //网络是否连接
	CMySocket* m_pMySocket;   //用于建立客户端
	CSClientDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSClientDlg)
	enum { IDD = IDD_SCLIENT_DIALOG };
	CString	m_strEditIPAddr;
	UINT	m_unEditPortNO;
	CString	m_strEditSendData;
	CString	m_strEditRecvData;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSClientDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSClientDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonLink();
	afx_msg void OnButtonUnlink();
	afx_msg void OnButtonSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCLIENTDLG_H__05807406_3669_11D8_870F_00E04C3F78CA__INCLUDED_)
