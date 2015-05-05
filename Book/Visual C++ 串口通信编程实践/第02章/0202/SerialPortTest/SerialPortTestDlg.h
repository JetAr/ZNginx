// SerialPortTestDlg.h : header file
//

#if !defined(AFX_SERIALPORTTESTDLG_H__556B3D06_5375_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SERIALPORTTESTDLG_H__556B3D06_5375_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialPort.h"  //添加CSerailPort类的头文件

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTestDlg dialog

class CSerialPortTestDlg : public CDialog
{
// Construction
public:
	CSerialPort m_SerialPort; //CSerailPort类对象
	BOOL m_bSerialPortOpened; //标志串口是否打开
	CSerialPortTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSerialPortTestDlg)
	enum { IDD = IDD_SERIALPORTTEST_DIALOG };
	CComboBox	m_ctrlComboComPort;
	CString	m_strEditReceiveMsg;
	CString	m_strEditSendMsg;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPortTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSerialPortTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LONG OnComm(WPARAM ch, LPARAM port);
	afx_msg void OnButtonOpen();
	afx_msg void OnButtonClose();
	afx_msg void OnButtonSend();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTTESTDLG_H__556B3D06_5375_11D8_870F_00E04C3F78CA__INCLUDED_)
