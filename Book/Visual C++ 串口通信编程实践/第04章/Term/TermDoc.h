// TermDoc.h : interface of the CTermDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMDOC_H__957DC8EA_654F_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_TERMDOC_H__957DC8EA_654F_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//����������ݿ顢XON�ַ���XOFF�ַ�
#define MAXBLOCK 4096
#define XON 0x11
#define XOFF 0x13

class CTermDoc : public CDocument
{
protected: // create from serialization only
	CTermDoc();
	DECLARE_DYNCREATE(CTermDoc)

// Attributes
public:
	CWinThread* m_pThread;  // �������߳�
	volatile BOOL m_bConnected;  //�����Ƿ�����
	volatile HWND m_hTermWnd;    //������ͼ�Ĵ��ھ��
	volatile HANDLE m_hPostMsgEvent; // ����WM_COMMNOTIFY��Ϣ���¼�����
	OVERLAPPED m_osRead, m_osWrite; // �����ص���/д
	volatile HANDLE m_hCom; // ���пھ��
	int m_nBaud;     //������
	int m_nDataBits; //ֹͣλ
	int m_nParity;   //У��λ
	CString m_strPort;     //���ں�
	int m_nStopBits; //ֹͣλ
	int m_nFlowCtrl; //������ѡ��
	BOOL m_bEcho;    //�Ƿ��ڱ��ػ���
	BOOL m_bNewLine; //�Ƿ��Զ�����

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTermDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL CanCloseFrame(CFrameWnd* pFrame);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTermDoc();
	void CloseConnection();   //�ر�����
	BOOL ConfigConnection(); //���ô���ͨ�Ų���
	BOOL OpenConnection();    //��������
	DWORD ReadComm(char *buf,DWORD dwLength);//������
	DWORD WriteComm(char *buf,DWORD dwLength);//д����

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTermDoc)
	afx_msg void OnCommConnect();
	afx_msg void OnCommDisconnect();
	afx_msg void OnCommSettings();
	afx_msg void OnUpdateCommDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateCommConnect(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TERMDOC_H__957DC8EA_654F_11D8_870F_00E04C3F78CA__INCLUDED_)
