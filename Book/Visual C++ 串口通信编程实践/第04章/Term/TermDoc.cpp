// TermDoc.cpp : implementation of the CTermDoc class
//

#include "stdafx.h"
#include "Term.h"
#include "SetupDlg.h"  //添加串口设置对话框头文件
#include "TermDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTermDoc

IMPLEMENT_DYNCREATE(CTermDoc, CDocument)

BEGIN_MESSAGE_MAP(CTermDoc, CDocument)
	//{{AFX_MSG_MAP(CTermDoc)
	ON_COMMAND(ID_COMM_CONNECT, OnCommConnect)
	ON_COMMAND(ID_COMM_DISCONNECT, OnCommDisconnect)
	ON_COMMAND(ID_COMM_SETTINGS, OnCommSettings)
	ON_UPDATE_COMMAND_UI(ID_COMM_DISCONNECT, OnUpdateCommDisconnect)
	ON_UPDATE_COMMAND_UI(ID_COMM_CONNECT, OnUpdateCommConnect)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTermDoc construction/destruction

CTermDoc::CTermDoc()
{
	// TODO: add one-time construction code here
	m_bConnected=FALSE;//断开连接菜单项无效
	m_pThread=NULL;

	m_nBaud = 9600;
	m_nDataBits = 8;
	m_bEcho = TRUE;   //初始设置为本地回显
	m_bNewLine = TRUE;  //初始设置为自动换行
	m_nParity = 0;      //无奇偶校验
	m_strPort = "COM2";  //选择COM2
	m_nStopBits = 0;     //
	m_nFlowCtrl=0;

}

CTermDoc::~CTermDoc()
{
	//程序结束时删除线程、关闭串口的操作
	if(m_bConnected)
		CloseConnection();

	// 删除事件句柄
	if(m_hPostMsgEvent)
		CloseHandle(m_hPostMsgEvent);

	if(m_osRead.hEvent)
		CloseHandle(m_osRead.hEvent);

	if(m_osWrite.hEvent)
		CloseHandle(m_osWrite.hEvent);

}

BOOL CTermDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	((CEditView*)m_viewList.GetHead())->SetWindowText(NULL);

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	// 为WM_COMMNOTIFY消息创建事件对象，手工重置，初始化为有信号的
	if((m_hPostMsgEvent=CreateEvent(NULL, TRUE, TRUE, NULL))==NULL)
		return FALSE;

	memset(&m_osRead, 0, sizeof(OVERLAPPED));
	memset(&m_osWrite, 0, sizeof(OVERLAPPED));

	// 为重叠读创建事件对象，手工重置，初始化为无信号的
	if((m_osRead.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL))==NULL)
		return FALSE;

	// 为重叠写创建事件对象，手工重置，初始化为无信号的
	if((m_osWrite.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL))==NULL)
		return FALSE;


	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTermDoc serialization

void CTermDoc::Serialize(CArchive& ar)
{
	// CEditView contains an edit control which handles all serialization
	((CEditView*)m_viewList.GetHead())->SerializeRaw(ar);
}

/////////////////////////////////////////////////////////////////////////////
// CTermDoc diagnostics

#ifdef _DEBUG
void CTermDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTermDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTermDoc commands

BOOL CTermDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CDocument::CanCloseFrame(pFrame);
}

///////////////////////////////////////////////////////////////
// 辅助线程，负责监视串行口
UINT CommProc(LPVOID pParam)
{
	OVERLAPPED os;
	DWORD dwMask, dwTrans;
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	CTermDoc *pDoc=(CTermDoc*)pParam;

	memset(&os, 0, sizeof(OVERLAPPED));
	os.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);

	if(os.hEvent==NULL)
	{
		AfxMessageBox("无法创建事件对象!");
		return (UINT)-1;
	}

	while(pDoc->m_bConnected)
	{
		ClearCommError(pDoc->m_hCom,&dwErrorFlags,&ComStat);

		if(ComStat.cbInQue)
		{
			// 无限等待WM_COMMNOTIFY消息被处理完
			WaitForSingleObject(pDoc->m_hPostMsgEvent, INFINITE);
			ResetEvent(pDoc->m_hPostMsgEvent);

			// 通知视图
			PostMessage(pDoc->m_hTermWnd, WM_COMMNOTIFY, EV_RXCHAR, 0);

			continue;
		}

		dwMask=0;

		if(!WaitCommEvent(pDoc->m_hCom, &dwMask, &os)) // 重叠操作
		{

			if(GetLastError()==ERROR_IO_PENDING)
			// 无限等待重叠操作结果
				GetOverlappedResult(pDoc->m_hCom, &os, &dwTrans, TRUE);
			else
			{
				CloseHandle(os.hEvent);
				return (UINT)-1;
			}
		}
	}

	CloseHandle(os.hEvent);
	return 0;
}




/////////////////////////////////////////////////////////////////////////////
// CTermDoc commands

//串口联接
void CTermDoc::OnCommConnect() 
{
	// TODO: Add your command handler code here
	if(!OpenConnection())
		AfxMessageBox("无法建立串口联接");
}

//断开串口联接
void CTermDoc::OnCommDisconnect() 
{
	// TODO: Add your command handler code here
	CloseConnection(); 
}

//串口设置
void CTermDoc::OnCommSettings() 
{
	// TODO: Add your command handler code here
	CSetupDlg dlg;
	CString str;

	dlg.m_bConnected = m_bConnected;

	dlg.m_strPort = m_strPort;

	str.Format("%d", m_nBaud);
	dlg.m_strBaud = str;

	str.Format("%d", m_nDataBits);
	dlg.m_strDatabits = str;

	dlg.m_nParity = m_nParity;
	
	dlg.m_nStopBits = m_nStopBits;

	dlg.m_nFlowctrl = m_nFlowCtrl;
	
	dlg.m_bEcho = m_bEcho;
	dlg.m_bNewline = m_bNewLine;

	if(dlg.DoModal() == IDOK)
	{
		m_strPort = dlg.m_strPort;
		m_nBaud = atoi(dlg.m_strBaud);
		m_nDataBits = atoi(dlg.m_strDatabits);
		m_nParity = dlg.m_nParity;
		m_nStopBits = dlg.m_nStopBits;

		m_nFlowCtrl = dlg.m_nFlowctrl;
		
		m_bEcho=dlg.m_bEcho;
		m_bNewLine=dlg.m_bNewline; 

	//	CString str;
	//	str.Format("m_nFlowCtrl=%d",m_nFlowCtrl);
	//	AfxMessageBox(str);

		if(m_bConnected)
			if(!ConfigConnection())
				AfxMessageBox("无法按指定的参数设置串口!");
	}	
}

//UPDATE_COMMAND_UI断开串口联接命令更新函数，设置命令是否可用
void CTermDoc::OnUpdateCommDisconnect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bConnected);
}

//UPDATE_COMMAND_UI串口联接命令更新函数，设置命令是否可用
void CTermDoc::OnUpdateCommConnect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_bConnected);
}


// 打开并配置串口，建立辅助线程
BOOL CTermDoc::OpenConnection()
{
	COMMTIMEOUTS TimeOuts;
	POSITION firstViewPos;
	CView *pView;

	firstViewPos=GetFirstViewPosition();
	pView=GetNextView(firstViewPos);

	m_hTermWnd=pView->GetSafeHwnd();

	if(m_bConnected)
		return FALSE;

	m_hCom=CreateFile(m_strPort, 
		GENERIC_READ | GENERIC_WRITE, 
		0, 
		NULL,
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // 重叠方式
		NULL); 

	if(m_hCom==INVALID_HANDLE_VALUE)
		return FALSE;

	SetupComm(m_hCom,MAXBLOCK,MAXBLOCK);
	SetCommMask(m_hCom, EV_RXCHAR);


	// 把间隔超时设为最大，把总超时设为0将导致ReadFile立即返回并完成操作
	TimeOuts.ReadIntervalTimeout=MAXDWORD; 
	TimeOuts.ReadTotalTimeoutMultiplier=0; 
	TimeOuts.ReadTotalTimeoutConstant=0; 

	/* 设置写超时以指定WriteComm成员函数中的GetOverlappedResult函数的等待时间*/
	TimeOuts.WriteTotalTimeoutMultiplier=50;
	TimeOuts.WriteTotalTimeoutConstant=2000;

	SetCommTimeouts(m_hCom, &TimeOuts);

	if(ConfigConnection())
	{

		m_pThread=AfxBeginThread(CommProc, this, THREAD_PRIORITY_NORMAL, 
			0, CREATE_SUSPENDED, NULL); // 创建并挂起线程

		if(m_pThread==NULL)
		{
			CloseHandle(m_hCom);
			return FALSE;
		}
		else

		{
			m_bConnected=TRUE;
			m_pThread->ResumeThread(); // 恢复线程运行
		}
	}
	else
	{
		CloseHandle(m_hCom);
		return FALSE;
	}
	
	return TRUE;
}

 //配置串口连接
BOOL CTermDoc::ConfigConnection()
{
	DCB dcb;

	if(!GetCommState(m_hCom, &dcb))
		return FALSE;

	dcb.fBinary=TRUE;
	dcb.BaudRate = m_nBaud; // 数据传输速率
	dcb.ByteSize = m_nDataBits; // 每字节位数
	dcb.fParity = TRUE;

	switch(m_nParity) // 校验设置
	{
	case 0: 
		dcb.Parity=NOPARITY;
		break;

	case 1: 
		dcb.Parity=EVENPARITY;
		break;

	case 2: 
		dcb.Parity=ODDPARITY;
		break;

	default:;
	}

	switch(m_nStopBits) // 停止位
	{
	case 0: 
		dcb.StopBits=ONESTOPBIT;
		break;

	case 1: 
		dcb.StopBits=ONE5STOPBITS;
		break;

	case 2:
		dcb.StopBits=TWOSTOPBITS;
		break;

	default:;
	}

	switch(m_nFlowCtrl)   //流控制选项设置
	{
	case 0:
		// 硬件流控制设置
		dcb.fOutxCtsFlow = FALSE;
		dcb.fRtsControl = FALSE;
		// XON/XOFF流控制设置
		dcb.fInX=dcb.fOutX = FALSE;
		//dcb.XonChar = XON;
		//dcb.XoffChar = XOFF;
		////dcb.XonLim = 50;
		//dcb.XoffLim = 50;
		break;
	case 1:
		// 硬件流控制设置
		dcb.fOutxCtsFlow = TRUE;
		dcb.fRtsControl = TRUE;

		// XON/XOFF流控制设置
		dcb.fInX=dcb.fOutX = FALSE;
		//dcb.XonChar = XON;
		//dcb.XoffChar = XOFF;
		//dcb.XonLim = 50;
		//dcb.XoffLim = 50;
		break;
	case 2:
		// 软件流控制设置
		dcb.fOutxCtsFlow = FALSE;
		dcb.fRtsControl = FALSE;

		// XON/XOFF流控制设置
		dcb.fInX=dcb.fOutX = TRUE;
		dcb.XonChar = XON;
		dcb.XoffChar = XOFF;
		dcb.XonLim = 50;
		dcb.XoffLim = 50;
		break;
	default:
		break;
	}

	return SetCommState(m_hCom, &dcb);
}

//关闭连接，关闭辅助线程
void CTermDoc::CloseConnection()
{
	if(!m_bConnected)
		return;

	m_bConnected=FALSE;

	//结束CommProc线程中WaitSingleObject函数的等待
	SetEvent(m_hPostMsgEvent); 

	//结束CommProc线程中WaitCommEvent的等待
	SetCommMask(m_hCom, 0); 

	//等待辅助线程终止
	WaitForSingleObject(m_pThread->m_hThread, INFINITE);
	m_pThread=NULL;
	CloseHandle(m_hCom);
}


// 将指定数量的字符从串行口输出
DWORD CTermDoc::WriteComm(char *buf, DWORD dwLength)
{
	BOOL fState;
	DWORD length=dwLength;
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	ClearCommError(m_hCom,&dwErrorFlags,&ComStat);
	fState=WriteFile(m_hCom,buf,length,&length,&m_osWrite);

	if(!fState)
	{

		if(GetLastError()==ERROR_IO_PENDING)
		{
			GetOverlappedResult(m_hCom,&m_osWrite,&length,TRUE);// 等待
		}
		else
			length=0;
	}

	return length;
}

// 从串行口输入缓冲区中读入指定数量的字符
DWORD CTermDoc::ReadComm(char *buf, DWORD dwLength)
{
	DWORD length=0;
	COMSTAT ComStat;
	DWORD dwErrorFlags;

	ClearCommError(m_hCom,&dwErrorFlags,&ComStat);
	length=min(dwLength, ComStat.cbInQue);
	ReadFile(m_hCom,buf,length,&length,&m_osRead);

	return length;
}
