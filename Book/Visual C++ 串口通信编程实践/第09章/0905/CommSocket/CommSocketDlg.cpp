// CommSocketDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CommSocket.h"
#include "CommSocketDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CCommSocketApp theApp;
void CALLBACK Listen(SOCKET s, int ServerPort, const char *ClientIP);

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
// CCommSocketDlg dialog

CCommSocketDlg::CCommSocketDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CCommSocketDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CCommSocketDlg)
    m_unEditServerPort = 5050;
    m_strEditConnIP = _T("10.1.37.170");
    m_unEditConnPort = 5050;
    m_strEditRecvData = _T("");
    m_strEditServerData = _T("");
    m_strEditSendData = _T("1234567890ABCDEFG");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_bServer = FALSE;
    m_bConnected = FALSE;
}

void CCommSocketDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCommSocketDlg)
    DDX_Text(pDX, IDC_EDIT_SERVERPORT, m_unEditServerPort);
    DDX_Text(pDX, IDC_EDIT_CONNIP, m_strEditConnIP);
    DDX_Text(pDX, IDC_EDIT_CONNPORT, m_unEditConnPort);
    DDX_Text(pDX, IDC_EDIT_RECVDATA, m_strEditRecvData);
    DDX_Text(pDX, IDC_EDIT_SERVERDATA, m_strEditServerData);
    DDX_Text(pDX, IDC_EDIT_SENDDATA, m_strEditSendData);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCommSocketDlg, CDialog)
    //{{AFX_MSG_MAP(CCommSocketDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_START, OnButtonStart)
    ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
    ON_BN_CLICKED(IDC_BUTTON_CONNECT, OnButtonConnect)
    ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, OnButtonDisconnect)
    ON_BN_CLICKED(IDC_BUTTON_SEND, OnButtonSend)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCommSocketDlg message handlers

BOOL CCommSocketDlg::OnInitDialog()
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
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bServer);
    GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bServer);

    GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(!m_bConnected);
    GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(m_bConnected);

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCommSocketDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCommSocketDlg::OnPaint()
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
HCURSOR CCommSocketDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CCommSocketDlg::OnButtonStart()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    if(m_wskServer.CreateServer(m_unEditServerPort, SOMAXCONN))
    {
        m_bServer = m_wskServer.StartServer(Listen);
    }

    if(!m_bServer)
    {
        CString info;
        info.Format("启动服务失败：%d", m_wskServer.err);
        AfxMessageBox(info);
    }

    GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bServer);
    GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bServer);
}

void CCommSocketDlg::OnButtonStop()
{
    // TODO: Add your control notification handler code here
    m_bServer = !m_wskServer.StopServer();
    if(m_bServer)
    {
        CString info;
        info.Format("停止服务失败：%d", m_wskServer.err);
        AfxMessageBox(info);
    }
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(!m_bServer);
    GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(m_bServer);
}

void CCommSocketDlg::OnButtonConnect()
{
    // TODO: Add your control notification handler code here

    UpdateData(TRUE);
    m_bConnected = m_wskClient.Connection(m_strEditConnIP, m_unEditConnPort);
    if(!m_bConnected)
    {
        CString info;
        info.Format("连接失败：%d", m_wskClient.err);
        AfxMessageBox(info);
    }
    GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(!m_bConnected);
    GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(m_bConnected);
}

void CCommSocketDlg::OnButtonDisconnect()
{
    // TODO: Add your control notification handler code here
    m_wskClient.Close();
    m_bConnected = FALSE;
    GetDlgItem(IDC_BUTTON_CONNECT)->EnableWindow(!m_bConnected);
    GetDlgItem(IDC_BUTTON_DISCONNECT)->EnableWindow(m_bConnected);
}


void CCommSocketDlg::OnButtonSend()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    int nRet = m_wskClient.SendData(m_strEditSendData, m_strEditSendData.GetLength(), 3);
    if(nRet <= 0)
    {
        CString info;
        info.Format("发送失败：%d", m_wskClient.err);
        AfxMessageBox(info);
    }
    else
    {
        TRACE("Begin Read\n");
        char buf[256];
        int nRet = m_wskClient.ReadData(buf, 255, 3);
        if(nRet > 0)
        {
            buf[nRet] = 0;
            m_strEditRecvData.Format("%.255s\r\n", buf);
            GetDlgItem(IDC_EDIT_RECVDATA)->SetWindowText(m_strEditRecvData);
        }
    }
}


void CALLBACK Listen(SOCKET s, int ServerPort, const char *ClientIP)
{
    CString info;
    CString strtmp;

    strtmp.Format("连接上%s:%d socket=0x%X\r\n", ClientIP, ServerPort, s);
    theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)->GetWindowText(info);
    info += strtmp;
    theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)->SetWindowText(info);
    ((CEdit*)(theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)))->LineScroll(999999);

    int nRet;
    char buf[256];
    CWSocket wsk;
    wsk = s;

    while(1)
    {
        info.Empty();
        nRet = wsk.ReadData(buf, 250, 60);
        if(nRet > 0)
        {
            buf[nRet] = 0;
            TRACE("recv: %s\n", buf);
            strtmp.Format("s%X recv: %.255s\r\n", s, buf);
            //theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)->GetWindowText(info);
            //TRACE("GetWindowText\n");
            info += strtmp;
            //theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)->SetWindowText(info);
            //TRACE("SetWindowText\n");
        }
        else
        {
            TRACE("recv error: %d\n", nRet);
            strtmp.Format("s%X recv error: %d\r\n", s, nRet);
            //theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)->GetWindowText(info);
            info += strtmp;
            //theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)->SetWindowText(info);
            break;
        }
        //TRACE("Read finished\n");
        //((CEdit*)(theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)))->LineScroll(999999);

        strtmp.Format("%.250s", buf);
        strtmp.Insert(0, "Re:");
        nRet = strtmp.GetLength();
        memcpy(buf, strtmp, nRet);

        Sleep(50);
        //TRACE("Begin Send\n");
        nRet = wsk.SendData(buf, nRet, 60);
        if(nRet > 0)
        {
            buf[nRet] = 0;
            TRACE("send: %s\n", buf);
            strtmp.Format("s%X send: %.255s\r\n", s, buf);
            //theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)->GetWindowText(info);
            info += strtmp;
            //theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)->SetWindowText(info);
        }
        else
        {
            TRACE("send error: %d\n", nRet);
            strtmp.Format("s%X send error: %d\r\n", s, nRet);
            //theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)->GetWindowText(info);
            info += strtmp;
            //theApp.m_pMainWnd->GetDlgItem(IDC_SERVERDATA)->SetWindowText(info);
            break;
        }

        theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)->GetWindowText(strtmp);
        info = strtmp + info;
        theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)->SetWindowText(info);
        ((CEdit*)(theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)))->LineScroll(999999);
    }

    wsk.Close();

    strtmp.Format("断开%s:%d socket=0x%X\r\n", ClientIP, ServerPort, s);
    theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)->GetWindowText(info);
    info += strtmp;
    theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)->SetWindowText(info);
    ((CEdit*)(theApp.m_pMainWnd->GetDlgItem(IDC_EDIT_SERVERDATA)))->LineScroll(999999);
}
