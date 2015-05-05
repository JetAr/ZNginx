// TermDoc.cpp : implementation of the CTermDoc class
//

#include "stdafx.h"
#include "Term.h"
#include "SetupDlg.h"  //��Ӵ������öԻ���ͷ�ļ�
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
	m_bConnected=FALSE;//�Ͽ����Ӳ˵�����Ч
	m_pThread=NULL;

	m_nBaud = 9600;
	m_nDataBits = 8;
	m_bEcho = TRUE;   //��ʼ����Ϊ���ػ���
	m_bNewLine = TRUE;  //��ʼ����Ϊ�Զ�����
	m_nParity = 0;      //����żУ��
	m_strPort = "COM2";  //ѡ��COM2
	m_nStopBits = 0;     //
	m_nFlowCtrl=0;

}

CTermDoc::~CTermDoc()
{
	//�������ʱɾ���̡߳��رմ��ڵĲ���
	if(m_bConnected)
		CloseConnection();

	// ɾ���¼����
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
	// ΪWM_COMMNOTIFY��Ϣ�����¼������ֹ����ã���ʼ��Ϊ���źŵ�
	if((m_hPostMsgEvent=CreateEvent(NULL, TRUE, TRUE, NULL))==NULL)
		return FALSE;

	memset(&m_osRead, 0, sizeof(OVERLAPPED));
	memset(&m_osWrite, 0, sizeof(OVERLAPPED));

	// Ϊ�ص��������¼������ֹ����ã���ʼ��Ϊ���źŵ�
	if((m_osRead.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL))==NULL)
		return FALSE;

	// Ϊ�ص�д�����¼������ֹ����ã���ʼ��Ϊ���źŵ�
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
// �����̣߳�������Ӵ��п�
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
		AfxMessageBox("�޷������¼�����!");
		return (UINT)-1;
	}

	while(pDoc->m_bConnected)
	{
		ClearCommError(pDoc->m_hCom,&dwErrorFlags,&ComStat);

		if(ComStat.cbInQue)
		{
			// ���޵ȴ�WM_COMMNOTIFY��Ϣ��������
			WaitForSingleObject(pDoc->m_hPostMsgEvent, INFINITE);
			ResetEvent(pDoc->m_hPostMsgEvent);

			// ֪ͨ��ͼ
			PostMessage(pDoc->m_hTermWnd, WM_COMMNOTIFY, EV_RXCHAR, 0);

			continue;
		}

		dwMask=0;

		if(!WaitCommEvent(pDoc->m_hCom, &dwMask, &os)) // �ص�����
		{

			if(GetLastError()==ERROR_IO_PENDING)
			// ���޵ȴ��ص��������
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

//��������
void CTermDoc::OnCommConnect() 
{
	// TODO: Add your command handler code here
	if(!OpenConnection())
		AfxMessageBox("�޷�������������");
}

//�Ͽ���������
void CTermDoc::OnCommDisconnect() 
{
	// TODO: Add your command handler code here
	CloseConnection(); 
}

//��������
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
				AfxMessageBox("�޷���ָ���Ĳ������ô���!");
	}	
}

//UPDATE_COMMAND_UI�Ͽ���������������º��������������Ƿ����
void CTermDoc::OnUpdateCommDisconnect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(m_bConnected);
}

//UPDATE_COMMAND_UI��������������º��������������Ƿ����
void CTermDoc::OnUpdateCommConnect(CCmdUI* pCmdUI) 
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable(!m_bConnected);
}


// �򿪲����ô��ڣ����������߳�
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
		FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, // �ص���ʽ
		NULL); 

	if(m_hCom==INVALID_HANDLE_VALUE)
		return FALSE;

	SetupComm(m_hCom,MAXBLOCK,MAXBLOCK);
	SetCommMask(m_hCom, EV_RXCHAR);


	// �Ѽ����ʱ��Ϊ��󣬰��ܳ�ʱ��Ϊ0������ReadFile�������ز���ɲ���
	TimeOuts.ReadIntervalTimeout=MAXDWORD; 
	TimeOuts.ReadTotalTimeoutMultiplier=0; 
	TimeOuts.ReadTotalTimeoutConstant=0; 

	/* ����д��ʱ��ָ��WriteComm��Ա�����е�GetOverlappedResult�����ĵȴ�ʱ��*/
	TimeOuts.WriteTotalTimeoutMultiplier=50;
	TimeOuts.WriteTotalTimeoutConstant=2000;

	SetCommTimeouts(m_hCom, &TimeOuts);

	if(ConfigConnection())
	{

		m_pThread=AfxBeginThread(CommProc, this, THREAD_PRIORITY_NORMAL, 
			0, CREATE_SUSPENDED, NULL); // �����������߳�

		if(m_pThread==NULL)
		{
			CloseHandle(m_hCom);
			return FALSE;
		}
		else

		{
			m_bConnected=TRUE;
			m_pThread->ResumeThread(); // �ָ��߳�����
		}
	}
	else
	{
		CloseHandle(m_hCom);
		return FALSE;
	}
	
	return TRUE;
}

 //���ô�������
BOOL CTermDoc::ConfigConnection()
{
	DCB dcb;

	if(!GetCommState(m_hCom, &dcb))
		return FALSE;

	dcb.fBinary=TRUE;
	dcb.BaudRate = m_nBaud; // ���ݴ�������
	dcb.ByteSize = m_nDataBits; // ÿ�ֽ�λ��
	dcb.fParity = TRUE;

	switch(m_nParity) // У������
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

	switch(m_nStopBits) // ֹͣλ
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

	switch(m_nFlowCtrl)   //������ѡ������
	{
	case 0:
		// Ӳ������������
		dcb.fOutxCtsFlow = FALSE;
		dcb.fRtsControl = FALSE;
		// XON/XOFF����������
		dcb.fInX=dcb.fOutX = FALSE;
		//dcb.XonChar = XON;
		//dcb.XoffChar = XOFF;
		////dcb.XonLim = 50;
		//dcb.XoffLim = 50;
		break;
	case 1:
		// Ӳ������������
		dcb.fOutxCtsFlow = TRUE;
		dcb.fRtsControl = TRUE;

		// XON/XOFF����������
		dcb.fInX=dcb.fOutX = FALSE;
		//dcb.XonChar = XON;
		//dcb.XoffChar = XOFF;
		//dcb.XonLim = 50;
		//dcb.XoffLim = 50;
		break;
	case 2:
		// �������������
		dcb.fOutxCtsFlow = FALSE;
		dcb.fRtsControl = FALSE;

		// XON/XOFF����������
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

//�ر����ӣ��رո����߳�
void CTermDoc::CloseConnection()
{
	if(!m_bConnected)
		return;

	m_bConnected=FALSE;

	//����CommProc�߳���WaitSingleObject�����ĵȴ�
	SetEvent(m_hPostMsgEvent); 

	//����CommProc�߳���WaitCommEvent�ĵȴ�
	SetCommMask(m_hCom, 0); 

	//�ȴ������߳���ֹ
	WaitForSingleObject(m_pThread->m_hThread, INFINITE);
	m_pThread=NULL;
	CloseHandle(m_hCom);
}


// ��ָ���������ַ��Ӵ��п����
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
			GetOverlappedResult(m_hCom,&m_osWrite,&length,TRUE);// �ȴ�
		}
		else
			length=0;
	}

	return length;
}

// �Ӵ��п����뻺�����ж���ָ���������ַ�
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
