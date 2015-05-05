// SerialPortVxDDlg.h : header file
//

#if !defined(AFX_SERIALPORTVXDDLG_H__962EE1C8_E5F6_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SERIALPORTVXDDLG_H__962EE1C8_E5F6_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>  //定义CEVent类

#include "CommHook.h"  //设备驱动头文件
#include "MyListCtrl.h"



/////////////////////////////////////////////////////////////////////////////
// CSerialPortVxDDlg dialog


class CSerialPortVxDDlg : public CDialog
{
// Construction
public:
	CSerialPortVxDDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSerialPortVxDDlg)
	enum { IDD = IDD_SERIALPORTVXD_DIALOG };
	CMyListCtrl	m_ctrlListDispData;
	CString	m_strPortName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPortVxDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	bool	m_bRunning;
	HANDLE	m_hVxD;
	_sAccessStats	m_sAccessData;
	CEvent	m_Event;
	DWORD	m_dwEventRing0Handle;
	char	*m_cpTargetPortName;
	CFont	m_fontTrace;
	LOGFONT	m_logfontTrace;
	int		m_nCharWidth;
	int		m_nCharHeight;
	CListRecord *m_pCurRec;


protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSerialPortVxDDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStartstop();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonStatreset();
	afx_msg void OnButtonCleardisp();
	afx_msg void OnButtonSavedata();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTVXDDLG_H__962EE1C8_E5F6_11D8_870F_00E04C3F78CA__INCLUDED_)
