// SerialPortTestDlg.h : header file
//

#if !defined(AFX_SERIALPORTTESTDLG_H__556B3D06_5375_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SERIALPORTTESTDLG_H__556B3D06_5375_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialPort.h"  //���CSerailPort���ͷ�ļ�

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTestDlg dialog

class CSerialPortTestDlg : public CDialog
{
// Construction
public:
	void SendNMEAData(CString &strData);
	BOOL GetSwitchStatus(UINT unSwitch);
	void SetSwitchStatus(UINT unSwtich,BOOL bStatus);
	HICON m_hIconRed;    //���ش�ʱ�ĺ��ͼ����
	HICON m_hIconOff;    //���عر�ʱ��ָʾͼ����

	CSerialPort m_SerialPort; //CSerailPort����󣬴���1
	CSerialPort m_SerialPort2; //CSerailPort����󣬴���2
	BOOL m_bSerialPortOpened; //��־����1�Ƿ��
	BOOL m_bSerialPortOpened2; //��־����2�Ƿ��
	UINT m_unPort;          //��¼����1�Ĵ��ں�
	UINT m_unPort2;         //��¼����2�Ĵ��ں�
	CString m_strPortRXData;  //���ڴ��洮��1�յ�������
	CString m_strPortRXData2;  //���ڴ��洮��2�յ�������
	CString m_strChecksum2;   //���ڴ���2�������ݼ���У��ֵ
	CSerialPortTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSerialPortTestDlg)
	enum { IDD = IDD_SERIALPORTTEST_DIALOG };
	CStatic	m_ctrlStaticIconS1;
	CStatic	m_ctrlStaticIconS2;
	CStatic	m_ctrlStaticIconS3;
	CStatic	m_ctrlStaticIconS4;
	CStatic	m_ctrlStaticIconS5;
	CStatic	m_ctrlStaticIconS6;
	CStatic	m_ctrlStaticIconS7;
	CStatic	m_ctrlStaticIconS8;
	CStatic	m_ctrlStaticIconS9;
	CButton	m_ctrlCheckSwitch1;
	CButton	m_ctrlCheckSwitch2;
	CButton	m_ctrlCheckSwitch3;
	CButton	m_ctrlCheckSwitch4;
	CButton	m_ctrlCheckSwitch5;
	CButton	m_ctrlCheckSwitch6;
	CButton	m_ctrlCheckSwitch7;
	CButton	m_ctrlCheckSwitch8;
	CButton	m_ctrlCheckSwitch9;
	CComboBox	m_ctrlComboComPort2;
	CComboBox	m_ctrlComboComPort;
	CString	m_strEditReceiveMsg;
	CString	m_strEditSendMsg;
	UINT	m_unEditPosition;
	UINT	m_unEditVelocity;
	UINT	m_unEditVelocity2;
	UINT	m_unEditPosition2;
	CString	m_strEditDispData2;
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
	afx_msg void OnButtonOpen2();
	afx_msg void OnButtonClose2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTTESTDLG_H__556B3D06_5375_11D8_870F_00E04C3F78CA__INCLUDED_)
