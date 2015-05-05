// SCOMMDlg.h : header file
//

#include "SerialPort.h"
#include "HyperLink.h"
#include "label.h"
#include "PushPin.h"
#include "AnimateDlgIcon.h" 

#if !defined(AFX_SCOMMDLG_H__666127A8_FEE4_40AA_9309_1B3B55EEDAFC__INCLUDED_)
#define AFX_SCOMMDLG_H__666127A8_FEE4_40AA_9309_1B3B55EEDAFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/////////////////////////////////////////////////////////////////////////////
// CSCOMMDlg dialog

class CSCOMMDlg : public CDialog
{
// Construction
public:
	BOOL m_bVisible;   //�����Ƿ��������� ����ͼ����ť����
	BOOL m_bStopDispRXData; //�Ƿ���ʾ�����ַ�
	CString m_strTempSendFilePathName; //�����ļ�·����
	long m_nFileLength;  //�ļ�����
	BOOL m_bSendFile;    //�Ƿ����ļ�
	HICON m_hIconRed;    //���ڴ�ʱ�ĺ��ͼ����
	HICON m_hIconOff;    //���ڹر�ʱ��ָʾͼ����
	HICON m_hIconGreen;

	int m_nBaud;       //������
	int m_nCom;         //���ں�
	char m_cParity;    //У��
	int m_nDatabits;    //����λ
	int m_nStopbits;    //ֹͣλ
	CSerialPort m_Port;  //CSerialPort�����
	CAnimateDlgIcon m_animIcon;  //����ͼ��
	CSCOMMDlg(CWnd* pParent = NULL);	// standard constructor


// Dialog Data
	//{{AFX_DATA(CSCOMMDlg)
	enum { IDD = IDD_SCOMM_DIALOG };
	CButton	m_ctrlHelp;
	CPushPinButton	m_ctrlPushPin;
	CButton	m_ctrlSendFile;
	CEdit	m_ctrlEditSendFile;
	CStatic	m_ctrlTXCount;
	CStatic	m_ctrlPortStatus;
	CStatic	m_ctrlRXCOUNT;
	CEdit	m_ctrlSavePath;
	CButton	m_ctrlManualSend;
	CHyperLink	m_ctrlHyperLink2;
	CButton	m_ctrlClearTXData;
	CStatic	m_ctrlStaticXFS;
	CButton	m_ctrlClose;
	CButton	m_ctrlCounterReset;
	CEdit	m_ctrlEditSend;
	CEdit	m_ctrlReceiveData;
	CButton	m_ctrlAutoClear;
	CStatic	m_ctrlIconOpenoff;
	CHyperLink	m_ctrlHyperLinkWWW;
	CComboBox	m_StopBits;
	CComboBox	m_DataBits;
	CComboBox	m_Parity;
	CButton	m_ctrlAutoSend;
	CButton	m_ctrlHexSend;
	CButton	m_ctrlStopDisp;
	CButton	m_ctrlOpenPort;
	CButton	m_ctrlHexReceieve;
	CComboBox	m_Speed;
	CComboBox	m_Com;
	CString	m_ReceiveData;
	CString	m_strSendData;
	CString	m_strCurPath;
	CString	m_strSendFilePathName;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSCOMMDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	int m_nCycleTime;
	BOOL m_bAutoSend;
	int Str2Hex(CString str, char *data);
	char HexChar(char c);
	DWORD m_dwCommEvents;
	BOOL m_bOpenPort;
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CSCOMMDlg)
	virtual BOOL OnInitDialog();
	afx_msg LONG OnCommunication(WPARAM ch, LPARAM port);
	afx_msg LONG OnFileSendingEnded(WPARAM wParam,LPARAM port);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonClearReciArea();
	afx_msg void OnButtonOpenport();
	afx_msg void OnButtonStopdisp();
	afx_msg void OnButtonManualsend();
	afx_msg void OnCheckAutosend();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnChangeEditCycletime();
	afx_msg void OnChangeEditSend();
	afx_msg void OnButtonClearrecasenda();
	afx_msg void OnSelendokComboComselect();
	afx_msg void OnSelendokComboSpeed();
	afx_msg void OnSelendokComboParity();
	afx_msg void OnSelendokComboDatabits();
	afx_msg void OnSelendokComboStopbits();
	afx_msg void OnButtonSavedata();
	afx_msg void OnButtonDirbrowser();
	afx_msg void OnButtonSendfile();
	afx_msg void OnButtonCountreset();
	afx_msg void OnButtonClose();
	afx_msg void OnButtonFilebrowser();
	afx_msg void OnButtonPushpin();
	afx_msg void OnDestroy();
	afx_msg void OnButtonHelp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	//DECLARE_DYNAMIC_MAP()



private:
	
};


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCOMMDLG_H__666127A8_FEE4_40AA_9309_1B3B55EEDAFC__INCLUDED_)

