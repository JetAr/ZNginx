// SClientDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SClient.h"
#include "SClientDlg.h"

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
// CSClientDlg dialog

CSClientDlg::CSClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSClientDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSClientDlg)
	m_strEditIPAddr = _T("10.1.37.185"); //地址读者可以更改成自己的IP
	m_unEditPortNO = 5050;//注意了：如果安装了病毒防火墙，要把相应端口打开
	m_strEditSendData = _T("");
	m_strEditRecvData = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_bLinked=FALSE;   //网络是否连接，初始状态置为断
}

void CSClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSClientDlg)
	DDX_Text(pDX, IDC_EDIT_IPADDR, m_strEditIPAddr);
	DDX_Text(pDX, IDC_EDIT_PORTNO, m_unEditPortNO);
	DDX_Text(pDX, IDC_EDIT_SENDDATA, m_strEditSendData);
	DDX_Text(pDX, IDC_EDIT_RECVDATA, m_strEditRecvData);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSClientDlg, CDialog)
	//{{AFX_MSG_MAP(CSClientDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_LINK, OnButtonLink)
	ON_BN_CLICKED(IDC_BUTTON_UNLINK, OnButtonUnlink)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSClientDlg message handlers

BOOL CSClientDlg::OnInitDialog()
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
	GetDlgItem(IDC_BUTTON_LINK)->EnableWindow(!m_bLinked);
	GetDlgItem(IDC_BUTTON_UNLINK)->EnableWindow(m_bLinked);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSClientDlg::OnPaint() 
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
HCURSOR CSClientDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSClientDlg::OnButtonLink() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_pMySocket=new CMySocket;
	m_pMySocket->Create();
	m_bLinked=m_pMySocket->Connect(m_strEditIPAddr, m_unEditPortNO);
	if(!m_bLinked)
	{
		AfxMessageBox("连接服务器失败");
	}
	GetDlgItem(IDC_BUTTON_LINK)->EnableWindow(!m_bLinked);
	GetDlgItem(IDC_BUTTON_UNLINK)->EnableWindow(m_bLinked);
}



void CSClientDlg::OnButtonUnlink() 
{
	// TODO: Add your control notification handler code here
	if(!m_bLinked) return; //如果没有连接，就返回
	if(m_pMySocket->ShutDown(2))//关闭接收与发送
	{
		m_bLinked=FALSE;
		Sleep(50);
		m_pMySocket->Close();
		if(m_pMySocket) delete m_pMySocket;
	}
	GetDlgItem(IDC_BUTTON_LINK)->EnableWindow(!m_bLinked);
	GetDlgItem(IDC_BUTTON_UNLINK)->EnableWindow(m_bLinked);

}

void CSClientDlg::OnButtonSend() 
{
	// TODO: Add your control notification handler code here
	if(!m_bLinked)
	{
		AfxMessageBox("没有连接网络");
		return;
	}
	UpdateData(TRUE);
	m_pMySocket->Send(m_strEditSendData,m_strEditSendData.GetLength(),0);
}

void CSClientDlg::UpdateRXData()
{
	m_strEditRecvData=m_strRXDataTemp;
	UpdateData(FALSE);
}
