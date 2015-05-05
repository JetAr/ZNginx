// SServerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SServer.h"
#include "SServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSServerDlg dialog

CSServerDlg::CSServerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSServerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSServerDlg)
	m_unEditPortNO = 5050;
	m_strEditNetMsg = _T("");
	m_strEditComMsg = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAcceptList.RemoveAll();  //初始化接收socket队列
	m_bListened=FALSE;  //没有开启网络服务
	m_bSerialPortOpened=FALSE; //没有打开串口
}

void CSServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSServerDlg)
	DDX_Control(pDX, IDC_CHECK_SENDDATA, m_ctrlCheckSendData);
	DDX_Control(pDX, IDC_COMBO_COMPORT, m_ctrlComboComPort);
	DDX_Text(pDX, IDC_EDIT_PORTNO, m_unEditPortNO);
	DDX_Text(pDX, IDC_EDIT_NETMSG, m_strEditNetMsg);
	DDX_Text(pDX, IDC_EDIT_COMMSG, m_strEditComMsg);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSServerDlg, CDialog)
	//{{AFX_MSG_MAP(CSServerDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
	ON_BN_CLICKED(IDC_CHECK_SENDDATA, OnCheckSenddata)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSServerDlg message handlers

BOOL CSServerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bListened);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bListened);
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);

	m_ctrlComboComPort.SetCurSel(0); //初始选择串口1
	m_ctrlCheckSendData.SetCheck(0); //没有打开串口之前不选中

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CSServerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSServerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//开启网络服务
void CSServerDlg::OnButtonStart() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_pListenSocket=new CListenSocket();  //新建监听Socket
	if(m_pListenSocket->Create(m_unEditPortNO))
	{
		if(!m_pListenSocket->Listen(10))
		{
			AfxMessageBox("设置监听Socket失败",MB_ICONINFORMATION);
			delete m_pListenSocket; //释放指针空间
		}
		else
		{
			m_bListened=TRUE;
		}
	}
	else
	{
		AfxMessageBox("创建监听Socket失败",MB_ICONINFORMATION);
		delete m_pListenSocket; //释放指针空间
	}
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bListened);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bListened);
}


//关闭网络服务
void CSServerDlg::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	if(!m_bListened) return;
	if(m_pListenSocket == NULL) return;
	m_pListenSocket->ShutDown(2);
	Sleep(50);
	m_pListenSocket->Close();
	m_pListenSocket = NULL;
	m_bListened = FALSE;	
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bListened);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bListened);
}

void CSServerDlg::UpdateMsgData()
{
	m_strEditNetMsg=m_strNetMsg;
	UpdateData(FALSE);   //更新网络通信信息
}

//打开串口
void CSServerDlg::OnButtonOpen() 
{
	// TODO: Add your control notification handler code here
	int nPort=m_ctrlComboComPort.GetCurSel()+1; //得到串口号
	if(m_SerialPort.InitPort(this, nPort, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))
	{
		m_SerialPort.StartMonitoring();
		m_bSerialPortOpened=TRUE;
	}
	else
	{
		AfxMessageBox("没有发现此串口或被占用");
		m_bSerialPortOpened=FALSE;
	}
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);
}

//关闭串口
void CSServerDlg::OnButtonClose() 
{
	// TODO: Add your control notification handler code here
	if(m_ctrlCheckSendData.GetCheck())
	{
		AfxMessageBox("请先关掉串口发送功能");
		return;
	}
	m_SerialPort.ClosePort();//关闭串口
    m_bSerialPortOpened=FALSE;
	GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
	GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);
}

//检查串口是否打开，否则不能通过串口发送数据
void CSServerDlg::OnCheckSenddata() 
{
	// TODO: Add your control notification handler code here
	if(m_ctrlCheckSendData.GetCheck())
	{
		if(!m_bSerialPortOpened)
		{
			AfxMessageBox("串口没有打开,请先打开串口");
			m_ctrlCheckSendData.SetCheck(0);
			return;
		}
	}
}

//通过串口发送数据
void CSServerDlg::SerialSendData(CString strSendData)
{
	if(!m_bSerialPortOpened) return;
	if(!m_ctrlCheckSendData.GetCheck()) return;
	m_SerialPort.WriteToPort((LPCTSTR)strSendData);//发送数据
	m_strEditComMsg.Format("向COM%d发送：%s",
		m_ctrlComboComPort.GetCurSel()+1,strSendData);
	UpdateData(FALSE);
}
