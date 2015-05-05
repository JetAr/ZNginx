// DisplayConDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DisplayCon.h"
#include "DisplayConDlg.h"

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
// CDisplayConDlg dialog

CDisplayConDlg::CDisplayConDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDisplayConDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDisplayConDlg)
	m_redscore = _T("");
	m_bluescore = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CDisplayConDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDisplayConDlg)
	DDX_Control(pDX, IDC_MSCOMM1, m_ctrlComm);
	DDX_Text(pDX, IDC_EDIT1, m_redscore);
	DDX_Text(pDX, IDC_EDIT2, m_bluescore);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDisplayConDlg, CDialog)
	//{{AFX_MSG_MAP(CDisplayConDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDisplayConDlg message handlers

BOOL CDisplayConDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
//////////////////
    COLORREF tx = RGB(  255,  0, 0); //文本着色
    COLORREF bk = RGB(  0,  0, 0);//背景色

	m_edit.SubclassDlgItem( IDC_EDIT_REDSCORE, this );  
    m_edit.bkColor( bk );
    m_edit.textColor( tx );
    m_edit.setFont( -80 );

	m_edit2.SubclassDlgItem( IDC_EDIT_BLUESCORE, this );   
    m_edit2.bkColor( bk );
    m_edit2.textColor( tx );
    m_edit2.setFont( -80 );
	////////////////////////////////////

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
		m_ctrlComm.SetCommPort(2); //选择com1
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

void CDisplayConDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CDisplayConDlg::OnPaint() 
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
HCURSOR CDisplayConDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

BEGIN_EVENTSINK_MAP(CDisplayConDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CDisplayConDlg)
	ON_EVENT(CDisplayConDlg, IDC_MSCOMM1, 1 /* OnComm */, OnComm, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CDisplayConDlg::OnComm() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
    COleSafeArray safearray_inp;//COleSafeArray操作任意类型任意维数的数组
    LONG len,k;
    BYTE rxdata[2048]; //设置BYTE数组 An 8-bit integerthat is not signed.
    CString strtemp,m_strRXData1;//,m_strRXData2;

   if(m_ctrlComm.GetCommEvent()==2) //事件值为2表示接收缓冲区内有字符
   {             
        
	       variant_inp=m_ctrlComm.GetInput(); //读缓冲区
           safearray_inp=variant_inp; //VARIANT型变量转换为ColeSafeArray型变量
           len=safearray_inp.GetOneDimSize(); //得到有效数据长度//返回一维COleSafeArray对象中的元素数
           for(k=0;k<len;k++)
            safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
           
		   m_strRXData1.Format("%c%c%c",rxdata[0],rxdata[1],rxdata[2]);
		char* temp=(char*)((LPCTSTR)m_strRXData1);
        char tbuf[10];
        tbuf[0]=temp[1]; tbuf[1]=temp[2];  tbuf[2]=0; 
		if(temp[0]=='r')
			m_redscore=tbuf;
		if(temp[0]=='b')
			m_bluescore=tbuf;
        if(temp[0]=='n')
		{
			m_bluescore="0";
            m_redscore="0";
		}
        UpdateData(FALSE); //更新编辑框内容
	}   
}
