#if !defined(AFX_DISPLAYSETTINGSDLG_H__B5A250BB_ED3B_4B4D_90D6_5E6EC87522A3__INCLUDED_)
#define AFX_DISPLAYSETTINGSDLG_H__B5A250BB_ED3B_4B4D_90D6_5E6EC87522A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DisplaySettingsDlg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CDisplaySettingsDlg dialog

class CDisplaySettingsDlg : public CDialog
{
// Construction
public:
	BOOL ModeExists( DEVMODE *dm );
	CString WordToString( UINT uValue );
	void LoadSettings( );
	void SaveSettings( );

	void FillCombo();
	void EnumDisplays();
	CDisplaySettingsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDisplaySettingsDlg();

	CPtrArray	m_DisplayModes;
	CString		m_strIniFile;
// Dialog Data
	//{{AFX_DATA(CDisplaySettingsDlg)
	enum { IDD = IDD_DISPLAY_SETTINGS };
	CComboBox	m_cboResolutions;
	BOOL	m_bDontAskAgain;
	BOOL	m_bFullscreen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDisplaySettingsDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDisplaySettingsDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DISPLAYSETTINGSDLG_H__B5A250BB_ED3B_4B4D_90D6_5E6EC87522A3__INCLUDED_)
