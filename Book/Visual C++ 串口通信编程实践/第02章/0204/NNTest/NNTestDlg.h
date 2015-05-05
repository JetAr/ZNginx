// NNTestDlg.h : header file
//

#if !defined(AFX_NNTESTDLG_H__6C0040D2_8D86_4405_B1B2_8A977C095121__INCLUDED_)
#define AFX_NNTESTDLG_H__6C0040D2_8D86_4405_B1B2_8A977C095121__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CNNTestDlg dialog

#include "SerialPort.h"


class CNNTestDlg : public CDialog
{
// Construction
public:
	CNNTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CNNTestDlg)
	enum { IDD = IDD_NNTEST_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNNTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

CSerialPort m_SerialPort;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CNNTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NNTESTDLG_H__6C0040D2_8D86_4405_B1B2_8A977C095121__INCLUDED_)
