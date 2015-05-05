// RemoteConDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RemoteCon.h"
#include "RemoteConDlg.h"

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
// CRemoteConDlg dialog

CRemoteConDlg::CRemoteConDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CRemoteConDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRemoteConDlg)
	m_bluescore = _T("");
	m_redscore = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CRemoteConDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRemoteConDlg)
	DDX_Control(pDX, IDC_MSCOMM1, m_ctrlComm);
	DDX_Text(pDX, IDC_EDIT_BLUESCORE, m_bluescore);
	DDX_Text(pDX, IDC_EDIT_REDSCORE, m_redscore);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CRemoteConDlg, CDialog)
	//{{AFX_MSG_MAP(CRemoteConDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_REDSEND, OnButtonRedsend)
	ON_BN_CLICKED(IDC_BUTTON_BLUESEND, OnButtonBluesend)
	ON_BN_CLICKED(IDC_BUTTONNEXT, OnButtonnext)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRemoteConDlg message handlers

BOOL CRemoteConDlg::OnInitDialog()
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
	m_ctrlComm.SetCommPort(1); //选择com1
if( !m_ctrlComm.GetPortOpen())
m_ctrlComm.SetPortOpen(TRUE);//打开串口
else
AfxMessageBox("cannot open serial port");

m_ctrlComm.SetSettings("9600,n,8,1"); //波特率9600，无校验，8个数据位，1个停止位 

m_ctrlComm.SetInputMode(1); //1：表示以二进制方式检取数据
m_ctrlComm.SetRThreshold(1); 
//参数1表示每当串口接收缓冲区中有多于或等于1个字符时将引发一个接收数据的OnComm事件
m_ctrlComm.SetInputLen(0); //设置当前接收区数据长度为0
m_ctrlComm.GetInput();


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CRemoteConDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CRemoteConDlg::OnPaint() 
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
HCURSOR CRemoteConDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BEGIN_EVENTSINK_MAP(CRemoteConDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CRemoteConDlg)
	ON_EVENT(CRemoteConDlg, IDC_MSCOMM1, 1 /* OnComm */, OnComm, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CRemoteConDlg::OnComm() 
{
	// TODO: Add your control notification handler code here
	
}

void CRemoteConDlg::OnButtonRedsend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_redscore='r'+m_redscore;
	m_ctrlComm.SetOutput(COleVariant(m_redscore));

}

void CRemoteConDlg::OnButtonBluesend() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	m_bluescore='b'+m_bluescore;
	m_ctrlComm.SetOutput(COleVariant(m_bluescore));
}

void CRemoteConDlg::OnButtonnext() 
{
	// TODO: Add your control notification handler code here
        m_redscore="0";
		m_bluescore="0";
		UpdateData(FALSE);
		m_ctrlComm.SetOutput(COleVariant("n"));
}
