// MSCommMultiPortDlg.cpp : implementation file
//

#include "stdafx.h"
#include "MSCommMultiPort.h"
#include "MSCommMultiPortDlg.h"

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
// CMSCommMultiPortDlg dialog

CMSCommMultiPortDlg::CMSCommMultiPortDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMSCommMultiPortDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMSCommMultiPortDlg)
	m_strEditCOM1RXData = _T("");
	m_strEditCOM1TXData = _T("");
	m_strEditCOM2TXData = _T("");
	m_strEditCOM2RXData = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMSCommMultiPortDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMSCommMultiPortDlg)
	DDX_Text(pDX, IDC_EDIT_COM1RXDATA, m_strEditCOM1RXData);
	DDX_Text(pDX, IDC_EDIT_COM1TXDATA, m_strEditCOM1TXData);
	DDX_Text(pDX, IDC_EDIT_COM2TXDATA, m_strEditCOM2TXData);
	DDX_Text(pDX, IDC_EDIT_COM2RXDATA, m_strEditCOM2RXData);
	DDX_Control(pDX, IDC_MSCOMM_COM1, m_ctrlMSCommCOM1);
	DDX_Control(pDX, IDC_MSCOMM_COM2, m_ctrlMSCommCOM2);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMSCommMultiPortDlg, CDialog)
	//{{AFX_MSG_MAP(CMSCommMultiPortDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_COM1SEND, OnButtonCom1send)
	ON_BN_CLICKED(IDC_BUTTON_COM2SEND, OnButtonCom2send)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMSCommMultiPortDlg message handlers

BOOL CMSCommMultiPortDlg::OnInitDialog()
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
	///////////////////��ʼ��COM1///////////////////
	m_ctrlMSCommCOM1.SetCommPort(1);   //ѡ��COM1
	m_ctrlMSCommCOM1.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_ctrlMSCommCOM1.SetInBufferSize(1024); //�������뻺������С
	m_ctrlMSCommCOM1.SetOutBufferSize(512); //���������������С
	//������9600����У�飬8������λ��1��ֹͣλ 
	m_ctrlMSCommCOM1.SetSettings("9600,n,8,1"); 
	//����1��ʾÿ�����ڽ��ջ��������ж���
	//�����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_ctrlMSCommCOM1.SetRThreshold(1); 
	if(!m_ctrlMSCommCOM1.GetPortOpen())
		m_ctrlMSCommCOM1.SetPortOpen(TRUE);//�򿪴���
	m_ctrlMSCommCOM1.SetInputLen(0);  //���õ�ǰ���������ݳ���Ϊ0
	m_ctrlMSCommCOM1.GetInput();    //��Ԥ���������������������
    
	///////////////////��ʼ��COM2///////////////////
	m_ctrlMSCommCOM2.SetCommPort(2);   //ѡ��COM2
	m_ctrlMSCommCOM2.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
	m_ctrlMSCommCOM2.SetInBufferSize(1024); //�������뻺������С
	m_ctrlMSCommCOM2.SetOutBufferSize(512); //���������������С
	//������9600����У�飬8������λ��1��ֹͣλ 
	m_ctrlMSCommCOM2.SetSettings("9600,n,8,1"); 
	//����1��ʾÿ�����ڽ��ջ��������ж���
	//�����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
	m_ctrlMSCommCOM2.SetRThreshold(1); 
	if(!m_ctrlMSCommCOM2.GetPortOpen())
		m_ctrlMSCommCOM2.SetPortOpen(TRUE);//�򿪴���
	m_ctrlMSCommCOM2.SetInputLen(0);  //���õ�ǰ���������ݳ���Ϊ0
	m_ctrlMSCommCOM2.GetInput();    //��Ԥ���������������������

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMSCommMultiPortDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMSCommMultiPortDlg::OnPaint() 
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
HCURSOR CMSCommMultiPortDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMSCommMultiPortDlg::OnButtonCom1send() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); //��ȡ�༭������
	m_ctrlMSCommCOM1.SetOutput(COleVariant(m_strEditCOM1TXData));//��������
}

void CMSCommMultiPortDlg::OnButtonCom2send() 
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE); //��ȡ�༭������
	m_ctrlMSCommCOM2.SetOutput(COleVariant(m_strEditCOM2TXData));//��������	
}

BEGIN_EVENTSINK_MAP(CMSCommMultiPortDlg, CDialog)
    //{{AFX_EVENTSINK_MAP(CMSCommMultiPortDlg)
	ON_EVENT(CMSCommMultiPortDlg, IDC_MSCOMM_COM1, 1 /* OnComm */, OnCommCOM1, VTS_NONE)
	ON_EVENT(CMSCommMultiPortDlg, IDC_MSCOMM_COM2, 1 /* OnComm */, OnCommCOM2, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CMSCommMultiPortDlg::OnCommCOM1() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //����BYTE����
	CString strtemp;
	if(m_ctrlMSCommCOM1.GetCommEvent()==2) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{
		variant_inp=m_ctrlMSCommCOM1.GetInput(); //��������
		safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for(k=0;k<len;k++)
			safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		for(k=0;k<len;k++)             //������ת��ΪCstring�ͱ���
		{
			BYTE bt=*(char*)(rxdata+k);    //�ַ���
			strtemp.Format("%c",bt);    //���ַ�������ʱ����strtemp���
			m_strEditCOM1RXData+=strtemp;  //������ձ༭���Ӧ�ַ���    
		}
	}
	UpdateData(FALSE);  //���±༭������	
}

void CMSCommMultiPortDlg::OnCommCOM2() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //����BYTE����
	CString strtemp;
	if(m_ctrlMSCommCOM2.GetCommEvent()==2) //�¼�ֵΪ2��ʾ���ջ����������ַ�
	{
		variant_inp=m_ctrlMSCommCOM2.GetInput(); //��������
		safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for(k=0;k<len;k++)
			safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		for(k=0;k<len;k++)             //������ת��ΪCstring�ͱ���
		{
			BYTE bt=*(char*)(rxdata+k);    //�ַ���
			strtemp.Format("%c",bt);    //���ַ�������ʱ����strtemp���
			m_strEditCOM2RXData+=strtemp;  //������ձ༭���Ӧ�ַ���
		}
	}
	UpdateData(FALSE);  //���±༭������	
}
