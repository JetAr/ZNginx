// SerialPortVxDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SerialPortVxD.h"
#include "SerialPortVxDDlg.h"

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
// CSerialPortVxDDlg dialog

CSerialPortVxDDlg::CSerialPortVxDDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSerialPortVxDDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSerialPortVxDDlg)
    m_strPortName = _T("");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    //变量的初始化
    m_bRunning = false;
    m_cpTargetPortName = NULL;
    m_hVxD = NULL;

    CFont font;
    font.CreatePointFont( 100, _T( "Courier New" ) );
    font.GetLogFont( &m_logfontTrace );
    m_fontTrace.CreateFontIndirect( &m_logfontTrace );

    m_pCurRec = NULL;

}

void CSerialPortVxDDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSerialPortVxDDlg)
    DDX_Control(pDX, IDC_LIST_DISPDATA, m_ctrlListDispData);
    DDX_Text(pDX, IDC_EDIT_PORT, m_strPortName);
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSerialPortVxDDlg, CDialog)
    //{{AFX_MSG_MAP(CSerialPortVxDDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON_STARTSTOP, OnButtonStartstop)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BUTTON_STATRESET, OnButtonStatreset)
    ON_BN_CLICKED(IDC_BUTTON_CLEARDISP, OnButtonCleardisp)
    ON_BN_CLICKED(IDC_BUTTON_SAVEDATA, OnButtonSavedata)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialPortVxDDlg message handlers

BOOL CSerialPortVxDDlg::OnInitDialog()
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

    // 事件传送
    HINSTANCE hKernel32 = LoadLibrary("KERNEL32");
    DWORD (*fpOVH)(HANDLE) =
        (DWORD (*)(HANDLE)) GetProcAddress(hKernel32, "OpenVxDHandle" );

    FreeLibrary(hKernel32);

    CRect rect;
    m_ctrlListDispData.GetClientRect( &rect );
    m_ctrlListDispData.InsertColumn( 0, NULL, LVCFMT_LEFT, rect.right-GetSystemMetrics(SM_CXVSCROLL) );

    //列表中的 WM_MEASUREITEM映射
    m_ctrlListDispData.GetWindowRect( &rect );
    WINDOWPOS wp;
    wp.hwnd = m_ctrlListDispData.m_hWnd;
    wp.cx = rect.Width();
    wp.cy = rect.Height();
    wp.flags = SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
    m_ctrlListDispData.SendMessage( WM_WINDOWPOSCHANGED, 0, (LPARAM)&wp );

    // 准备好第一个记录
    m_pCurRec = new CListRecord;
    int item = m_ctrlListDispData.InsertItem( 0, NULL );
    m_ctrlListDispData.SetItemData( item, (DWORD)m_pCurRec );

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSerialPortVxDDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSerialPortVxDDlg::OnPaint()
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
HCURSOR CSerialPortVxDDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CSerialPortVxDDlg::OnButtonStartstop()
{
    // TODO: Add your control notification handler code here
    m_bRunning = !m_bRunning;
    if ( m_bRunning )
    {
        UpdateData( TRUE );
        if ( m_strPortName.IsEmpty() )
            m_strPortName = "NONE";
        delete[] m_cpTargetPortName;
        m_cpTargetPortName = new char[ m_strPortName.GetLength() + 1 ];
        strcpy( m_cpTargetPortName, m_strPortName );

        // 以下的设置是把串口设备驱动文件CommHook.vxd放在系统文件夹中
        // WINSYSDIR一般在C:\Windows\System
        //CreateFile函数中用"\\\\.\\CommHook.vxd"
        m_hVxD = CreateFile( "\\\\.\\CommHook.vxd",
                             0, 0, NULL, 0,
                             FILE_FLAG_DELETE_ON_CLOSE, NULL );
        if ( m_hVxD == INVALID_HANDLE_VALUE )
        {
            AfxMessageBox( "无法打开虚拟设备驱动文件commhook.VxD -\n\n(请检查是否放在系统文件夹：WINSYSDIR)", MB_OK );
            m_bRunning = false;
            m_hVxD = NULL;
            return;
        }
        GetDlgItem( IDC_BUTTON_STARTSTOP )->SetWindowText( "停止截获串口数据" );

        DWORD result;

        DeviceIoControl(m_hVxD,
                        _CommHook_DIOC_SetTargetPort,
                        m_cpTargetPortName,
                        strlen(m_cpTargetPortName),
                        NULL,
                        0,
                        &result,
                        NULL );
        //如果打开设备驱动文件成功，就启动定时器每200ms查询一次数据
        SetTimer( 1000, 200, NULL );
    }
    else
    {
        KillTimer( 1000 );  //停止定时
        GetDlgItem( IDC_BUTTON_STARTSTOP )->SetWindowText( "开始截获串口数据" );
        CloseHandle( m_hVxD );  //关闭虚拟设备句柄
        m_hVxD = NULL;
    }
}

void CSerialPortVxDDlg::OnTimer(UINT nIDEvent)
{
    // TODO: Add your message handler code here and/or call default
    BOOL status;
    DWORD nBytes;

    status = DeviceIoControl(	m_hVxD,
                                _CommHook_DIOC_AccessStats,
                                NULL,
                                0,
                                &m_sAccessData,
                                sizeof(m_sAccessData),
                                &nBytes,
                                NULL );

    // 得到串口设备中的数据量的大小（字节数）.
    status = DeviceIoControl(	m_hVxD,
                                _CommHook_DIOC_ReadTraceData,
                                NULL,
                                0,
                                NULL,
                                0,
                                &nBytes,
                                NULL );
    if ( nBytes )
    {
        ASSERT( nBytes < 10000 );
        DWORD request = nBytes;
        // 这里每个字节占一个字的容量，高字节为Tx/Rx（发送/接收）标志
        unsigned short int *pData = new unsigned short int[ request ];
        status = DeviceIoControl(	m_hVxD,
                                    _CommHook_DIOC_ReadTraceData,
                                    NULL,
                                    0,
                                    pData,
                                    request,
                                    &nBytes,
                                    NULL );
        ASSERT( request == nBytes );

        for ( int i=0; i<(int)nBytes; i++ )
        {
            m_pCurRec->m_pwData[m_pCurRec->m_dwLength++] = pData[i];

            if ( m_pCurRec->m_dwLength == 32 )
            {
                int n = m_ctrlListDispData.GetItemCount();
                m_ctrlListDispData.RedrawItems( n-1, n-1 );

                m_pCurRec = new CListRecord;
                m_ctrlListDispData.InsertItem( n, NULL );
                m_ctrlListDispData.SetItemData( n, (DWORD)m_pCurRec );
            }
        }
        int n = m_ctrlListDispData.GetItemCount();
        m_ctrlListDispData.RedrawItems( n-1, n-1 );

        for ( i=m_ctrlListDispData.GetItemCount(); i>1000; i-- )
            m_ctrlListDispData.DeleteItem(0);
        m_ctrlListDispData.EnsureVisible( m_ctrlListDispData.GetItemCount()-1, FALSE );

        delete[] pData;
    }

    if ( status )
    {
        CString str;
        str.Format( "%ld", m_sAccessData.dwReadCount );
        GetDlgItem( IDC_EDIT_READS )->SetWindowText( str );
        str.Format( "%ld", m_sAccessData.dwWriteCount );
        GetDlgItem( IDC_EDIT_WRITES )->SetWindowText( str );
        str.Format( "%ld", m_sAccessData.dwReadBytes );
        GetDlgItem( IDC_EDIT_READBYTES)->SetWindowText( str );
        str.Format( "%ld", m_sAccessData.dwWriteBytes );
        GetDlgItem( IDC_EDIT_WRITEBYTES )->SetWindowText( str );
    }

    CDialog::OnTimer(nIDEvent);
}

void CSerialPortVxDDlg::OnButtonStatreset()
{
    // TODO: Add your control notification handler code here
    if ( m_hVxD != NULL )
    {
        DWORD result;
        DeviceIoControl(m_hVxD,
                        _CommHook_DIOC_ClearStats,
                        NULL,
                        0,
                        NULL,
                        0,
                        &result,
                        NULL);
    }

    GetDlgItem( IDC_EDIT_READS )->SetWindowText( "0" );
    GetDlgItem( IDC_EDIT_WRITES )->SetWindowText( "0" );
    GetDlgItem( IDC_EDIT_READBYTES )->SetWindowText( "0" );
    GetDlgItem( IDC_EDIT_WRITEBYTES )->SetWindowText( "0" );
}

void CSerialPortVxDDlg::OnButtonCleardisp()
{
    // TODO: Add your control notification handler code here
    m_ctrlListDispData.DeleteAllItems();
    m_pCurRec = new CListRecord;
    int item = m_ctrlListDispData.InsertItem( 0, NULL );
    m_ctrlListDispData.SetItemData( item, (DWORD)m_pCurRec );
}


void CSerialPortVxDDlg::OnButtonSavedata()
{
    // TODO: Add your control notification handler code here
    FILE *fp = fopen( "CHT.log", "wb" );
    int count = m_ctrlListDispData.GetItemCount();
    WORD status = 10;
    int cols = 0;
    bool getLen = true;
    WORD len=0;

    for ( int i=0; i<count; i++ )
    {
        CListRecord *pRec = (CListRecord *)m_ctrlListDispData.GetItemData( i );

        for ( int j=0; j<(int)pRec->m_dwLength; j++ )
        {
            WORD w = pRec->m_pwData[j];

            WORD type = w >> 8;

            if ( type != status || !len)
            {
                fprintf( fp, "\x0D\x0A" );
                status = type;
                if ( status == 0 )	// receive
                    fprintf( fp, "Rx:" );
                else
                    fprintf( fp, "Tx:" );
                cols = 0;
                getLen = true;
            }

            ++cols;
            fprintf( fp, " %2.2X", w & 0xFF );
            len --;

            if ( getLen && cols == 9 )
            {
                len = (w & 0xFF) + 1;
                getLen = false;
            }
        }
    }

    fclose( fp );
}
