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
    m_ctrlComm.SetCommPort(1);   //ѡ��COM1
    m_ctrlComm.SetInputMode(1); //���뷽ʽΪ�����Ʒ�ʽ
    m_ctrlComm.SetInBufferSize(1024); //�������뻺������С
    m_ctrlComm.SetOutBufferSize(512); //���������������С
    //������9600����У�飬8������λ��1��ֹͣλ
    m_ctrlComm.SetSettings("9600,n,8,1");
    if(!m_ctrlComm.GetPortOpen())
        m_ctrlComm.SetPortOpen(TRUE);//�򿪴���
    //����1��ʾÿ�����ڽ��ջ��������ж���
    //�����1���ַ�ʱ������һ���������ݵ�OnComm�¼�
    m_ctrlComm.SetRThreshold(1);
    m_ctrlComm.SetInputLen(0);  //���õ�ǰ���������ݳ���Ϊ0
    m_ctrlComm.GetInput();    //��Ԥ���������������������

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
    BYTE rxdata[2048]; //����BYTE����
    CString strtemp;
    if(m_ctrlComm.GetCommEvent()==2) //�¼�ֵΪ2��ʾ���ջ����������ַ�
    {
        variant_inp=m_ctrlComm.GetInput(); //��������
        safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
        len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
        for(k=0; k<len; k++)
            safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
        for(k=0; k<len; k++)           //������ת��ΪCstring�ͱ���
        {
            BYTE bt=*(char*)(rxdata+k);    //�ַ���
            strtemp.Format("%c",bt);    //���ַ�������ʱ����strtemp���
            m_strEditRXData+=strtemp;  //������ձ༭���Ӧ�ַ���
        }
    }
    UpdateData(FALSE);  //���±༭������
}


void CSCommTestDlg::OnButtonManualsend()
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE); //��ȡ�༭������
    m_ctrlComm.SetOutput(COleVariant(m_strEditTXData));//��������

    /*	char TxData[100];
        int Count = m_strEditTXData.GetLength();
    	for(int i = 0; i < Count; i++)
    		TxData[i] = m_strEditTXData.GetAt(i);
        CByteArray array;
        array.RemoveAll();
        array.SetSize(Count);
        for(i=0;i<Count;i++)
    	    array.SetAt(i, TxData[i]);
        m_ctrlComm.SetOutput(COleVariant(array)); // ��������
    */

////////////////ע�����´������ڵ�3�µ�3.2.3��
/////////////////��η��ͽ���ASCIIֵΪ0�ʹ���128���ַ����Ĳ��Դ���
    /*	unsigned char chData[8];
    	chData[0]=0;    chData[1]=1;    chData[2]=13;
    	chData[3]=12;   chData[4]=0;    chData[5]=10;
    	chData[6]=156;   //156��16����ֵΪ0X9C
    	chData[7]=255;   //255��16����ֵΪ0XFF

    	CByteArray binData;
    	binData.RemoveAll(); //���binData
    	for(int i=0;i<8;i++)
    		binData.Add(chData[i]);
    	COleVariant var(binData); //��binDataת��ΪVariant��������
    	m_ctrlComm.SetOutput(var);
    */
}



