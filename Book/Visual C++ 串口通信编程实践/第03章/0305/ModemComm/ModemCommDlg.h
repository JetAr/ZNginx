// ModemCommDlg.h : header file
//
#include "mscomm.h"/////////added 

#if !defined(AFX_MODEMCOMMDLG_H__548C9AA6_884C_43B0_9505_BC2A41328CB3__INCLUDED_)
#define AFX_MODEMCOMMDLG_H__548C9AA6_884C_43B0_9505_BC2A41328CB3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CModemCommDlg dialog

class CModemCommDlg : public CDialog
{
// Construction
public:
	CModemCommDlg(CWnd* pParent = NULL);	// standard constructor

    //added begin
	void OpenComm(int number);//
	void SendString(CString m_strSend);

	CMSComm	m_ctrlComm;
	BOOL bOpen;//added end
// Dialog Data
	//{{AFX_DATA(CModemCommDlg)
	enum { IDD = IDD_MODEMCOMM_DIALOG };
	CButton	m_ctrlDial;
	CString	m_ReceiveData;
	CString	m_SendData;
	CString	m_TelphoneNo;
	int		m_NumCom;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CModemCommDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CModemCommDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnComm();
	afx_msg void OnRadioCom1();
	afx_msg void OnRadioCom2();
	afx_msg void OnDial();
	afx_msg void OnSend();
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MODEMCOMMDLG_H__548C9AA6_884C_43B0_9505_BC2A41328CB3__INCLUDED_)
