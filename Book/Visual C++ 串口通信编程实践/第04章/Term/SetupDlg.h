#if !defined(AFX_SETUPDLG_H__957DC901_654F_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SETUPDLG_H__957DC901_654F_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog

class CSetupDlg : public CDialog
{
// Construction
public:
	CSetupDlg(CWnd* pParent = NULL);   // standard constructor
	BOOL m_bConnected;  //串口是否联接
	
	int m_nParity;//校验位
	int m_nStopBits;//停止位

// Dialog Data
	//{{AFX_DATA(CSetupDlg)
	enum { IDD = IDD_DIALOG_COMSETTINGS };
	CButton	m_ctrlFlowCtrl;
	CComboBox	m_ctrlStopbits;
	CComboBox	m_ctrlPort;
	CComboBox	m_ctrlParity;
	CComboBox	m_ctrlDatabits;
	CComboBox	m_ctrlBaud;
	int		m_nFlowctrl;
	CString	m_strPort;
	CString	m_strBaud;
	CString	m_strDatabits;
	BOOL	m_bEcho;
	BOOL	m_bNewline;
	// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDLG_H__957DC901_654F_11D8_870F_00E04C3F78CA__INCLUDED_)
