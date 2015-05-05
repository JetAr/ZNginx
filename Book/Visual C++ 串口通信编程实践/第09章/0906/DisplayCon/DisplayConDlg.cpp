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
    COLORREF tx = RGB(  255,  0, 0); //�ı���ɫ
    COLORREF bk = RGB(  0,  0, 0);//����ɫ

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
		m_ctrlComm.SetCommPort(2); //ѡ��com1
if( !m_ctrlComm.GetPortOpen())
      m_ctrlComm.SetPortOpen(TRUE);//�򿪴���
else
AfxMessageBox("cannot open serial port");

m_ctrlComm.SetSettings("9600,n,8,1"); //������9600����У�飬8������λ��1��ֹͣλ 

m_ctrlComm.SetInputMode(1); //1����ʾ�Զ����Ʒ�ʽ��ȡ����
m_ctrlComm.SetRThreshold(1); 
//����1��ʾÿ�����ڽ��ջ��������ж��ڻ����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
m_ctrlComm.SetInputLen(0); //���õ�ǰ���������ݳ���Ϊ0
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
    COleSafeArray safearray_inp;//COleSafeArray����������������ά��������
    LONG len,k;
    BYTE rxdata[2048]; //����BYTE���� An 8-bit integerthat is not signed.
    CString strtemp,m_strRXData1;//,m_strRXData2;

   if(m_ctrlComm.GetCommEvent()==2) //�¼�ֵΪ2��ʾ���ջ����������ַ�
   {             
        
	       variant_inp=m_ctrlComm.GetInput(); //��������
           safearray_inp=variant_inp; //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
           len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���//����һάCOleSafeArray�����е�Ԫ����
           for(k=0;k<len;k++)
            safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
           
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
        UpdateData(FALSE); //���±༭������
	}   
}
