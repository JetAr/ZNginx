// SerialPortTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SerialPortTest.h"
#include "SerialPortTestDlg.h"

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
// CSerialPortTestDlg dialog

CSerialPortTestDlg::CSerialPortTestDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSerialPortTestDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSerialPortTestDlg)
    m_strEditReceiveMsg = _T("");
    m_strEditSendMsg = _T("");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_bSerialPortOpened=FALSE; //初始状态：串口没有打开
}

void CSerialPortTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSerialPortTestDlg)
    DDX_Control(pDX, IDC_COMBO_COMPORT, m_ctrlComboComPort);
    DDX_Text(pDX, IDC_EDIT_RECEIVEMSG, m_strEditReceiveMsg);
    DDX_Text(pDX, IDC_EDIT_SENDMSG, m_strEditSendMsg);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSerialPortTestDlg, CDialog)
    //{{AFX_MSG_MAP(CSerialPortTestDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_MESSAGE(WM_COMM_RXCHAR, OnComm)
    ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE, OnButtonClose)
    ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTestDlg message handlers

BOOL CSerialPortTestDlg::OnInitDialog()
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
    m_ctrlComboComPort.SetCurSel(0); //初始选择串口1
    //以下两句分别设置“打开串口”、“关闭串口”两个按状态的使能状态
    GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
    GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialPortTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSerialPortTestDlg::OnPaint()
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
HCURSOR CSerialPortTestDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

LONG CSerialPortTestDlg::OnComm(WPARAM ch, LPARAM port)
{
    m_strEditReceiveMsg += ch;
    UpdateData(FALSE);  //将接收到的字符显示在接收编辑框中
    return 0;
}

void CSerialPortTestDlg::OnButtonOpen()
{
    // TODO: Add your control notification handler code here
    int nPort=m_ctrlComboComPort.GetCurSel()+1; //得到串口号，想想为什么要加1
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

void CSerialPortTestDlg::OnButtonClose()
{
    // TODO: Add your control notification handler code here
    m_SerialPort.ClosePort();//关闭串口
    m_bSerialPortOpened=FALSE;
    GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
    GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);
}

void CSerialPortTestDlg::OnButtonSend()
{
    // TODO: Add your control notification handler code here
    if(!m_bSerialPortOpened) return; //检查串口是否打开
    UpdateData(TRUE); //读入编辑框中的数据
    m_SerialPort.WriteToPort((LPCTSTR)m_strEditSendMsg);//发送数据
}









