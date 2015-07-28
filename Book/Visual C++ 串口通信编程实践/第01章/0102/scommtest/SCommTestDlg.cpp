// SCommTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SCommTest.h"
#include "SCommTestDlg.h"

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
// CSCommTestDlg dialog

CSCommTestDlg::CSCommTestDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSCommTestDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSCommTestDlg)
    m_strEditRXData = _T("");
    m_strEditTXData = _T("");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSCommTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSCommTestDlg)
    DDX_Control(pDX, IDC_MSCOMM1, m_ctrlComm);
    DDX_Text(pDX, IDC_EDIT_RXDATA, m_strEditRXData);
    DDX_Text(pDX, IDC_EDIT_TXDATA, m_strEditTXData);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSCommTestDlg, CDialog)
    //{{AFX_MSG_MAP(CSCommTestDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_MANUALSEND, OnButtonManualsend)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSCommTestDlg message handlers

BOOL CSCommTestDlg::OnInitDialog()
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
    m_ctrlComm.SetCommPort(1);   //选择COM1
    m_ctrlComm.SetInputMode(1); //输入方式为二进制方式
    m_ctrlComm.SetInBufferSize(1024); //设置输入缓冲区大小
    m_ctrlComm.SetOutBufferSize(512); //设置输出缓冲区大小
    //波特率9600，无校验，8个数据位，1个停止位
    m_ctrlComm.SetSettings("9600,n,8,1");
    if(!m_ctrlComm.GetPortOpen())
        m_ctrlComm.SetPortOpen(TRUE);//打开串口
    //参数1表示每当串口接收缓冲区中有多于
    //或等于1个字符时将引发一个接收数据的OnComm事件
    m_ctrlComm.SetRThreshold(1);
    m_ctrlComm.SetInputLen(0);  //设置当前接收区数据长度为0
    m_ctrlComm.GetInput();    //先预读缓冲区以清除残留数据

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSCommTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSCommTestDlg::OnPaint()
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
HCURSOR CSCommTestDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

BEGIN_EVENTSINK_MAP(CSCommTestDlg, CDialog)
//{{AFX_EVENTSINK_MAP(CSCommTestDlg)
ON_EVENT(CSCommTestDlg, IDC_MSCOMM1, 1 /* OnComm */, OnComm, VTS_NONE)
//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CSCommTestDlg::OnComm()
{
    // TODO: Add your control notification handler code here
    VARIANT variant_inp;
    COleSafeArray safearray_inp;
    LONG len,k;
    BYTE rxdata[2048]; //设置BYTE数组
    CString strtemp;
    if(m_ctrlComm.GetCommEvent()==2) //事件值为2表示接收缓冲区内有字符
    {
        variant_inp=m_ctrlComm.GetInput(); //读缓冲区
        safearray_inp=variant_inp;  //VARIANT型变量转换为ColeSafeArray型变量
        len=safearray_inp.GetOneDimSize(); //得到有效数据长度
        for(k=0; k<len; k++)
            safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
        for(k=0; k<len; k++)           //将数组转换为Cstring型变量
        {
            BYTE bt=*(char*)(rxdata+k);    //字符型
            strtemp.Format("%c",bt);    //将字符送入临时变量strtemp存放
            m_strEditRXData+=strtemp;  //加入接收编辑框对应字符串
        }
    }
    UpdateData(FALSE);  //更新编辑框内容
}


void CSCommTestDlg::OnButtonManualsend()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE); //读取编辑框内容
    m_ctrlComm.SetOutput(COleVariant(m_strEditTXData));//发送数据

    /*	char TxData[100];
        int Count = m_strEditTXData.GetLength();
    	for(int i = 0; i < Count; i++)
    		TxData[i] = m_strEditTXData.GetAt(i);
        CByteArray array;
        array.RemoveAll();
        array.SetSize(Count);
        for(i=0;i<Count;i++)
    	    array.SetAt(i, TxData[i]);
        m_ctrlComm.SetOutput(COleVariant(array)); // 发送数据
    */

////////////////注：以下代码用于第3章第3.2.3节
/////////////////如何发送接收ASCII值为0和大于128的字符？的测试代码
    /*	unsigned char chData[8];
    	chData[0]=0;    chData[1]=1;    chData[2]=13;
    	chData[3]=12;   chData[4]=0;    chData[5]=10;
    	chData[6]=156;   //156的16进制值为0X9C
    	chData[7]=255;   //255的16进制值为0XFF

    	CByteArray binData;
    	binData.RemoveAll(); //清空binData
    	for(int i=0;i<8;i++)
    		binData.Add(chData[i]);
    	COleVariant var(binData); //将binData转化为Variant数据类型
    	m_ctrlComm.SetOutput(var);
    */
}



