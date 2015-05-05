// SServerDlg.h : header file
//

#if !defined(AFX_SSERVERDLG_H__BFEB8166_42F6_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SSERVERDLG_H__BFEB8166_42F6_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialPort.h"    //���CSerialPort��ͷ�ļ�
#include "ListenSocket.h"  //���CListenSocket��ͷ�ļ�
//���CAcceptSocket��ͷ�ļ�
/////////////////////////////////////////////////////////////////////////////
// CSServerDlg dialog

class CSServerDlg : public CDialog
{
// Construction
public:
	void SerialSendData(CString strSendData);
	void UpdateMsgData();  //���ڸ�����������뷢����Ϣ
	CListenSocket* m_pListenSocket;
	BOOL m_bListened;  //���ڱ�־�������Ƿ��ڿ������������
	CPtrList m_pAcceptList; //���ڱ������socket�Ķ���
	CString  m_strNetMsg; //���ڴ��ݶ˿ںźͷ����IP��ַ
	BOOL m_bSerialPortOpened; //���ڱ�־�����Ƿ��
	CSerialPort m_SerialPort; //����CSerialPort�����
//	CString m_strSerialSendData; //����Ҫͨ�����ڷ��͵�����
	CSServerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CSServerDlg)
	enum { IDD = IDD_SSERVER_DIALOG };
	CButton	m_ctrlCheckSendData;
	CComboBox	m_ctrlComboComPort;
	UINT	m_unEditPortNO;
	CString	m_strEditNetMsg;
	CString	m_strEditComMsg;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSServerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSServerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStart();
	afx_msg void OnButtonStop();
	afx_msg void OnButtonOpen();
	afx_msg void OnButtonClose();
	afx_msg void OnCheckSenddata();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SSERVERDLG_H__BFEB8166_42F6_11D8_870F_00E04C3F78CA__INCLUDED_)
