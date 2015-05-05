// ModemCommDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ModemComm.h"
#include "ModemCommDlg.h"

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
// CModemCommDlg dialog

CModemCommDlg::CModemCommDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CModemCommDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CModemCommDlg)
	m_ReceiveData = _T("");
	m_SendData = _T("");
	m_TelphoneNo = _T("");
	m_NumCom = -1;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CModemCommDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CModemCommDlg)
	DDX_Control(pDX, IDC_BUTTON_DIAL, m_ctrlDial);
	DDX_Text(pDX, IDC_EDIT_RECEIVE, m_ReceiveData);
	DDX_Text(pDX, IDC_EDIT_SEND, m_SendData);
	DDX_Text(pDX, IDC_EDIT_TELPHONENO, m_TelphoneNo);
	DDX_Radio(pDX, IDC_RADIO_COM1, m_NumCom);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CModemCommDlg, CDialog)
	//{{AFX_MSG_MAP(CModemCommDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_RADIO_COM1, OnRadioCom1)
	ON_BN_CLICKED(IDC_RADIO_COM2, OnRadioCom2)
	ON_BN_CLICKED(IDC_BUTTON_DIAL, OnDial)
	ON_BN_CLICKED(IDC_BUTTON_SEND, OnSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CModemCommDlg message handlers

BOOL CModemCommDlg::OnInitDialog()
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
	DWORD style=WS_VISIBLE|WS_CHILD;
	if (!m_ctrlComm.Create(NULL,style,CRect(0,0,0,0),this,IDC_MSCOMM1))
	{
		TRACE0("Failed to create OLE Communications Control\n");
	    return -1; //fail to create　　
	}
	
	// TODO: Add extra initialization here
	m_ctrlComm.SetCommPort(1); //选择COM1
	m_ctrlComm.SetInBufferSize(1024); //设置输入缓冲区的大小，Bytes
	m_ctrlComm.SetOutBufferSize(512); //设置输出缓冲区的大小，Bytes

	if(!m_ctrlComm.GetPortOpen()) //打开串口   
	{
		m_ctrlComm.SetPortOpen(TRUE);	
		SendString("ATS0=1\n");//Modem自动等待连接
	}
	else
		AfxMessageBox("串口1已被占用，请选择其他串口");
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CModemCommDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CModemCommDlg::OnPaint() 
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
HCURSOR CModemCommDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BEGIN_EVENTSINK_MAP(CModemCommDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CModemCommDlg)
	ON_EVENT(CModemCommDlg, IDC_MSCOMM1, 1 /* OnComm */, OnComm, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CModemCommDlg::OnComm() 
{
	// TODO: Add your control notification handler code here
	VARIANT vResponse;
	char *str;
	char *str1;
    int k, nEvent, i;
	
    nEvent = m_ctrlComm.GetCommEvent();

    switch(nEvent)
	{
	case 2:  //收到大于RTHresshold个字符
		k = m_ctrlComm.GetInBufferCount(); //接收到的字符数目
		
		if(k > 0)
		{
			vResponse=m_ctrlComm.GetInput(); //read
			//对数据进行其他处理 
            str = (char*)(unsigned char*) vResponse.parray->pvData;
		}    
		// 接收到字符，MSComm控件发送事件
		i = 0;
		str1 = str;
		while (i < k)
		{
			i++;
			str1++;
		}
		*str1 = '\0';
		m_ReceiveData += (const char *)str;
		//清除字符串中的不必要字符
		break;
	case 3:  //CTS线状态发生了变化
		break;
	case 4:  //DSR线状态发生了变化
		break;
	case 5:  //CD线状态发生了变化
		break;
	case 6:  //Ring Indicator发生变化
		break;
    }    

	UpdateData(FALSE);
}

void CModemCommDlg::OnRadioCom1() 
{
	// TODO: Add your control notification handler code here
	OpenComm(1);
}

void CModemCommDlg::OnRadioCom2() 
{
	// TODO: Add your control notification handler code here
	OpenComm(2);
}

void CModemCommDlg::OnDial() 
{
	// TODO: Add your control notification handler code here
	CString strTemp;
	int Count = m_TelphoneNo.GetLength();
	if(!bOpen)
	{
		if(Count < 7)
			AfxMessageBox("电话号码位数错误");
		else
		{
			bOpen = TRUE;
			
			//向Modem发送指令
			strTemp = "ATDT" + m_TelphoneNo + "\n"; 
			SendString(strTemp);
			m_ctrlDial.SetWindowText("挂断");
		}
	}
	else
	{
		SendString("ATH0");
		bOpen = FALSE;
		m_ctrlDial.SetWindowText("拨号");
	}
}

void CModemCommDlg::OnSend() 
{
	// TODO: Add your control notification handler code here
	CString strTemp = m_ReceiveData; 
	SendString(strTemp);
}

void CModemCommDlg::SendString(CString m_strSend)
{
	char TxData[100];
	int Count = m_SendData.GetLength();

	for(int i = 0; i < Count; i++)
		TxData[i] = m_SendData.GetAt(i);

	CByteArray array;
	array.RemoveAll();
	array.SetSize(Count);

	for(i = 0; i < Count; i++)
		array.SetAt(i, TxData[i]);
	m_ctrlComm.SetOutput(COleVariant(array));
}

void CModemCommDlg::OpenComm(int number)
{
	m_ctrlComm.SetCommPort(number); //选择COM
	
	if(!m_ctrlComm.GetPortOpen()) //打开串口   
	{
		m_ctrlComm.SetPortOpen(TRUE);//Modem自动等待连接	
		SendString("ATS0=1\n");

		bOpen = FALSE;
		m_ctrlDial.SetWindowText("拨号");
	}
	else
		AfxMessageBox("串口1已被占用，请选择其他串口");	
}