#if !defined(AFX_SETTINGSDLG_H__4FC35B70_CC7D_4FD6_A41D_8F49EDB194E6__INCLUDED_)
#define AFX_SETTINGSDLG_H__4FC35B70_CC7D_4FD6_A41D_8F49EDB194E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SettingsDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog

class CSettingsDlg : public CDialog
{
// Construction
public:
	CSettingsDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSettingsDlg)
	enum { IDD = IDD_SETTINGS };
	CComboBox	m_ctrlRateList;
	CComboBox	m_ctrlCommList;
	CString	m_strSmsc;
	//}}AFX_DATA

	CString m_strPort;
	CString m_strRate;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSettingsDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETTINGSDLG_H__4FC35B70_CC7D_4FD6_A41D_8F49EDB194E6__INCLUDED_)
