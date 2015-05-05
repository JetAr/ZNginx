// TermDoc.h : interface of the CTermDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERMDOC_H__957DC8EA_654F_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_TERMDOC_H__957DC8EA_654F_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//定义最大数据块、XON字符和XOFF字符
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
	CWinThread* m_pThread;  // 代表辅助线程
	volatile BOOL m_bConnected;  //串口是否联接
	volatile HWND m_hTermWnd;    //保存视图的窗口句柄
	volatile HANDLE m_hPostMsgEvent; // 用于WM_COMMNOTIFY消息的事件对象
	OVERLAPPED m_osRead, m_osWrite; // 用于重叠读/写
	volatile HANDLE m_hCom; // 串行口句柄
	int m_nBaud;     //波特率
	int m_nDataBits; //停止位
	int m_nParity;   //校验位
	CString m_strPort;     //串口号
	int m_nStopBits; //停止位
	int m_nFlowCtrl; //流控制选项
	BOOL m_bEcho;    //是否在本地回显
	BOOL m_bNewLine; //是否自动换行

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
	void CloseConnection();   //关闭联接
	BOOL ConfigConnection(); //配置串口通信参数
	BOOL OpenConnection();    //建立联接
	DWORD ReadComm(char *buf,DWORD dwLength);//读串口
	DWORD WriteComm(char *buf,DWORD dwLength);//写串口

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
