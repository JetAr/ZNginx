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
    m_unEditPosition = 0;
    m_unEditVelocity = 0;
    m_unEditVelocity2 = 0;
    m_unEditPosition2 = 0;
    m_strEditDispData2 = _T("");
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_bSerialPortOpened=FALSE; //��ʼ״̬������1û�д�
    m_bSerialPortOpened2=FALSE; //��ʼ״̬������2û�д�
    m_unPort=1;
    m_unPort2=2;
    m_strPortRXData2="";
    m_strChecksum2="";
}

void CSerialPortTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSerialPortTestDlg)
    DDX_Control(pDX, IDC_STATIC_ICONS1, m_ctrlStaticIconS1);
    DDX_Control(pDX, IDC_STATIC_ICONS2, m_ctrlStaticIconS2);
    DDX_Control(pDX, IDC_STATIC_ICONS3, m_ctrlStaticIconS3);
    DDX_Control(pDX, IDC_STATIC_ICONS4, m_ctrlStaticIconS4);
    DDX_Control(pDX, IDC_STATIC_ICONS5, m_ctrlStaticIconS5);
    DDX_Control(pDX, IDC_STATIC_ICONS6, m_ctrlStaticIconS6);
    DDX_Control(pDX, IDC_STATIC_ICONS7, m_ctrlStaticIconS7);
    DDX_Control(pDX, IDC_STATIC_ICONS8, m_ctrlStaticIconS8);
    DDX_Control(pDX, IDC_STATIC_ICONS9, m_ctrlStaticIconS9);
    DDX_Control(pDX, IDC_CHECK_SWITCH1, m_ctrlCheckSwitch1);
    DDX_Control(pDX, IDC_CHECK_SWITCH2, m_ctrlCheckSwitch2);
    DDX_Control(pDX, IDC_CHECK_SWITCH3, m_ctrlCheckSwitch3);
    DDX_Control(pDX, IDC_CHECK_SWITCH4, m_ctrlCheckSwitch4);
    DDX_Control(pDX, IDC_CHECK_SWITCH5, m_ctrlCheckSwitch5);
    DDX_Control(pDX, IDC_CHECK_SWITCH6, m_ctrlCheckSwitch6);
    DDX_Control(pDX, IDC_CHECK_SWITCH7, m_ctrlCheckSwitch7);
    DDX_Control(pDX, IDC_CHECK_SWITCH8, m_ctrlCheckSwitch8);
    DDX_Control(pDX, IDC_CHECK_SWITCH9, m_ctrlCheckSwitch9);
    DDX_Control(pDX, IDC_COMBO_COMPORT2, m_ctrlComboComPort2);
    DDX_Control(pDX, IDC_COMBO_COMPORT, m_ctrlComboComPort);
    DDX_Text(pDX, IDC_EDIT_RECEIVEMSG, m_strEditReceiveMsg);
    DDX_Text(pDX, IDC_EDIT_SENDMSG, m_strEditSendMsg);
    DDX_Text(pDX, IDC_EDIT_POSITION, m_unEditPosition);
    DDV_MinMaxUInt(pDX, m_unEditPosition, 0, 31);
    DDX_Text(pDX, IDC_EDIT_VELOCITY, m_unEditVelocity);
    DDV_MinMaxUInt(pDX, m_unEditVelocity, 0, 120);
    DDX_Text(pDX, IDC_EDIT_VELOCITY2, m_unEditVelocity2);
    DDX_Text(pDX, IDC_EDIT_POSITION2, m_unEditPosition2);
    DDX_Text(pDX, IDC_EDIT_DISPDATA2, m_strEditDispData2);
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
    ON_BN_CLICKED(IDC_BUTTON_OPEN2, OnButtonOpen2)
    ON_BN_CLICKED(IDC_BUTTON_CLOSE2, OnButtonClose2)
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
    m_ctrlComboComPort.SetCurSel(0); //��ʼѡ�񴮿�1
    m_ctrlComboComPort2.SetCurSel(1); //��ʼѡ�񴮿�2

    //��������ֱ����á��򿪴��ڡ������رմ��ڡ�������״̬��ʹ��״̬
    GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
    GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);
    GetDlgItem(IDC_BUTTON_OPEN2)->EnableWindow(!m_bSerialPortOpened2);
    GetDlgItem(IDC_BUTTON_CLOSE2)->EnableWindow(m_bSerialPortOpened2);

    //����ͼ��
    m_hIconRed  = AfxGetApp()->LoadIcon(IDI_ICON_RED);
    m_hIconOff	= AfxGetApp()->LoadIcon(IDI_ICON_OFF);

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

#define CR 0X0D    //�س�
#define LF 0X0A    //����
LONG CSerialPortTestDlg::OnComm(WPARAM ch, LPARAM port)
{
    static char checksum=0,checksum1=0;
    static int count1=0;//,count2=0,count3=0;
    static unsigned char buf[20];
    static char c1,c2;    //���ڼ����ByteУ��
    static int flag;     //���ڽ��ս׶α��
    static int twoflag=0;

    if(port == m_unPort)
    {
        if(ch>127)   //�ǲ������ֽ�
        {
            count1=0;  //��¼�����ַ��ĸ���
            buf[count1]=ch;
            checksum1= ch-128;  //��ʼ����У��ֵ
        }
        else
        {
            count1++;
            buf[count1]=ch;
            checksum1 = checksum1^ch;
            if(count1==3)  //����У���ֽ����ڵ�ȫ���������
            {
                if(checksum1)  //У���
                {
                    m_strEditReceiveMsg = "����У�����";
                    UpdateData(FALSE);
                }
                else
                {
                    CString str;
                    unsigned char * temp=(unsigned char*)buf;
                    m_strEditReceiveMsg ="���յ��ļ�ͨ��Э���ֽ�Ϊ��";
                    for(int i=0; i<4; i++)
                    {
                        str.Format("%02X ",*(buf+i));
                        m_strEditReceiveMsg += str;
                    }
                    UpdateData(FALSE);
                }
                if(count1>5)   //��ֹ����
                    count1=0;
            }
        }
    }

    if(port==m_unPort2)   //����2�����ݴ���
    {
        m_strPortRXData2 += (char)ch;
        switch(ch)
        {
        case '$':
            checksum=0;			//��ʼ����CheckSum
            flag=0;
            break;
        case '*':  //��Ч���ݽ���������$��*֮�����ݵİ�ByteУ��ֵ��
            flag=2;
            c2=checksum & 0x0f;
            c1=((checksum >> 4) & 0x0f);
            if (c1 < 10) c1+= '0';
            else c1 += 'A' - 10;
            if (c2 < 10) c2+= '0';
            else c2 += 'A' - 10;
            break;
        case CR:   //��������ϣ����������
            break;
        case LF:   //���ݰ������һ���ַ�
            m_strPortRXData2.Empty();
            break;
        default:
            if(flag>0)  //ע�⣺ֻ���ڽ��յ�'*'��flag�Ŵ���0
            {
                m_strChecksum2 += ch;
                if(flag==1)
                {
                    CString strCheck="";
                    strCheck.Format("%c%c",c1,c2);
                    if(strCheck!=m_strChecksum2)  //У����㲻��ȷ��˵���������ݳ���
                    {
                        m_strPortRXData2.Empty();
                    }
                    else  //У�������ȷ��������
                    {
                        CString strSwitchSetData;
                        strSwitchSetData = m_strPortRXData2.Mid(1,9);
                        //���������źŵ�״̬
                        for(int i=0; i<9; i++)
                        {
                            if(strSwitchSetData.Mid(i,1)=="1")
                                SetSwitchStatus(i+1,TRUE);
                            else
                                SetSwitchStatus(i+1,FALSE);
                        }
                        //����ȡ��λ�����ٶ�����
                        CString strTemp;
                        strTemp = m_strPortRXData2.Mid(10,5);
                        char *temp=(char*)((LPCTSTR)strTemp);
                        char tbuf[4];
                        tbuf[0]=temp[0];
                        tbuf[1]=temp[1];
                        tbuf[2]=0;
                        m_unEditPosition2 = atoi(tbuf);  //�õ�λ��״ֵ̬
                        tbuf[0]=temp[2];
                        tbuf[1]=temp[3];
                        tbuf[2]=temp[4];
                        tbuf[3]=0;
                        m_unEditVelocity2 = atoi(tbuf);  //�õ��ٶ�ֵ
                        //�����յ������ݰ�������ʾ
                        m_strEditDispData2 = "���յ�NMEA���ݰ���"+m_strPortRXData2;
                        //����׼������"���Զ���ͨ��Э��"���ݰ�
                        unsigned char ucChar[4];
                        //���ֽ�
                        ucChar[0]=0x80;   //���ֽ����λ��1
                        if(strSwitchSetData.Mid(0,1)=="1")  // ����1״̬
                            ucChar[0] |= 0x40;            //0100 0000
                        else
                            ucChar[0] &= 0xBF;	          //1011 1111
                        if(strSwitchSetData.Mid(1,1)=="1")  // ����2״̬
                            ucChar[0] |= 0x20;            //0010 0000
                        else
                            ucChar[0] &= 0xDF;	          //1101 1111
                        if(m_unEditPosition2>31)      //��λ��ֵ������ֵ
                            m_unEditPosition2=31;
                        ucChar[0] &= 0xE0;    //�����ֽڵĵ�5λ��0
                        ucChar[0] += m_unEditPosition2; //�ټ���λ��ֵ
                        ucChar[3] = ucChar[0];    //ͬʱ����У��ֵ
                        //�ڶ��ֽ�
                        unsigned char ucTemp=0x40;
                        for(i=0; i<7; i++)
                        {
                            if(strSwitchSetData.Mid(2+i,1)=="1")
                                ucChar[1] |= ucTemp;
                            else
                                ucChar[1] &= (~ucTemp);
                            ucTemp >>= 1;
                        }
                        ucChar[1] &= 0x7F; //ǰ��������ô�����㣬��������䱣֤һ�����λΪ0
                        ucChar[3] ^= ucChar[1];   //����У��ֵ
                        //�����ֽ�
                        if(m_unEditVelocity2>120)
                            m_unEditVelocity2=120;
                        ucChar[2] = m_unEditVelocity2;
                        ucChar[3] ^= ucChar[2];   //����У��ֵ
                        //�����ֽ�
                        ucChar[3] &= 0x7F;
                        //��"���Զ���ͨ��Э��"���ݰ����ͳ�ȥ
                        if(m_bSerialPortOpened2)
                            m_SerialPort2.WriteToPort(ucChar,4);
                        //ͬʱ�ѷ��͵�������ʾ
                        CString strTemp1;
                        strTemp=   _T("���͵�����Ϊ: ");
                        for(int j=0; j<4; j++)
                        {
                            strTemp1.Format("0x%02X",ucChar[j]);
                            strTemp +=  strTemp1 + ",";
                        }
                        m_strEditDispData2 += strTemp;

                        UpdateData(FALSE);
                    }
                    m_strChecksum2.Empty();
                }
                //��'*'���գ�flag=2,1�μ�1���������ý����ݰ���У��ֵ������m_strChecksum��
                flag--;
            }
            else
                checksum=checksum^ch;  //��flag<=0ʱ������У��ֵ
            break;
        }
    }

    return 0;
}

void CSerialPortTestDlg::OnButtonOpen()
{
    // TODO: Add your control notification handler code here
    int nPort=m_ctrlComboComPort.GetCurSel()+1; //�õ����ںţ�����ΪʲôҪ��1
    if(m_SerialPort.InitPort(this, nPort, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))
    {
        m_SerialPort.StartMonitoring();
        m_bSerialPortOpened=TRUE;
        m_unPort=nPort;
    }
    else
    {
        AfxMessageBox("û�з��ִ˴��ڻ�ռ��");
        m_bSerialPortOpened=FALSE;
    }
    GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
    GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);
}

void CSerialPortTestDlg::OnButtonClose()
{
    // TODO: Add your control notification handler code here
    m_SerialPort.ClosePort();//�رմ���
    m_bSerialPortOpened=FALSE;
    GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
    GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);
}

void CSerialPortTestDlg::OnButtonSend()
{
    // TODO: Add your control notification handler code here
    if(!m_bSerialPortOpened) //��鴮���Ƿ��
    {
        AfxMessageBox("����û�д�");
        return;
    }
    UpdateData(TRUE); //����༭���е�����
    CString strSend=""; //Ҫ���͵�NEMA�ַ���
    //���¶���9�����ص�����״̬
    for(int i=1; i<=9; i++)
    {
        if(GetSwitchStatus(i))
            strSend+='1';
        else
            strSend+='0';
    }
    CString strTemp;
    strTemp.Format("%02d",m_unEditPosition);
    strSend+=strTemp;
    strTemp.Format("%03d",m_unEditVelocity);
    strSend+=strTemp;
    SendNMEAData(strSend);
}


//�ú����������ÿ��ص��źŵ�״̬���翪��5������
//������Ϊ SetSwitchStatus(5,TRUE);
void CSerialPortTestDlg::SetSwitchStatus(UINT unSwtich,BOOL bStatus)
{
    if(bStatus)
    {
        switch(unSwtich)
        {
        case 1:
            m_ctrlStaticIconS1.SetIcon(m_hIconRed);
            break;
        case 2:
            m_ctrlStaticIconS2.SetIcon(m_hIconRed);
            break;
        case 3:
            m_ctrlStaticIconS3.SetIcon(m_hIconRed);
            break;
        case 4:
            m_ctrlStaticIconS4.SetIcon(m_hIconRed);
            break;
        case 5:
            m_ctrlStaticIconS5.SetIcon(m_hIconRed);
            break;
        case 6:
            m_ctrlStaticIconS6.SetIcon(m_hIconRed);
            break;
        case 7:
            m_ctrlStaticIconS7.SetIcon(m_hIconRed);
            break;
        case 8:
            m_ctrlStaticIconS8.SetIcon(m_hIconRed);
            break;
        case 9:
            m_ctrlStaticIconS9.SetIcon(m_hIconRed);
            break;
        default:
            break;
        }
    }
    else
    {
        switch(unSwtich)
        {
        case 1:
            m_ctrlStaticIconS1.SetIcon(m_hIconOff);
            break;
        case 2:
            m_ctrlStaticIconS2.SetIcon(m_hIconOff);
            break;
        case 3:
            m_ctrlStaticIconS3.SetIcon(m_hIconOff);
            break;
        case 4:
            m_ctrlStaticIconS4.SetIcon(m_hIconOff);
            break;
        case 5:
            m_ctrlStaticIconS5.SetIcon(m_hIconOff);
            break;
        case 6:
            m_ctrlStaticIconS6.SetIcon(m_hIconOff);
            break;
        case 7:
            m_ctrlStaticIconS7.SetIcon(m_hIconOff);
            break;
        case 8:
            m_ctrlStaticIconS8.SetIcon(m_hIconOff);
            break;
        case 9:
            m_ctrlStaticIconS9.SetIcon(m_hIconOff);
            break;
        default:
            break;
        }
    }
}


//�ú������ڵõ����õĿ���״ֵ̬
BOOL CSerialPortTestDlg::GetSwitchStatus(UINT unSwitch)
{
    BOOL bStatus=FALSE;
    switch(unSwitch)
    {
    case 1:
        bStatus = m_ctrlCheckSwitch1.GetCheck();
        break;
    case 2:
        bStatus = m_ctrlCheckSwitch2.GetCheck();
        break;
    case 3:
        bStatus = m_ctrlCheckSwitch3.GetCheck();
        break;
    case 4:
        bStatus = m_ctrlCheckSwitch4.GetCheck();
        break;
    case 5:
        bStatus = m_ctrlCheckSwitch5.GetCheck();
        break;
    case 6:
        bStatus = m_ctrlCheckSwitch6.GetCheck();
        break;
    case 7:
        bStatus = m_ctrlCheckSwitch7.GetCheck();
        break;
    case 8:
        bStatus = m_ctrlCheckSwitch8.GetCheck();
        break;
    case 9:
        bStatus = m_ctrlCheckSwitch9.GetCheck();
        break;
    default:
        break;
    }
    return bStatus;
}

void CSerialPortTestDlg::OnButtonOpen2()
{
    // TODO: Add your control notification handler code here
    int nPort=m_ctrlComboComPort2.GetCurSel()+1; //�õ����ں�
    if(m_SerialPort2.InitPort(this, nPort, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))
    {
        m_SerialPort2.StartMonitoring();
        m_bSerialPortOpened2=TRUE;
        m_unPort2=nPort;   //��¼�򿪵Ĵ��ں�
    }
    else
    {
        AfxMessageBox("û�з��ִ˴��ڻ�ռ��");
        m_bSerialPortOpened2=FALSE;
    }
    GetDlgItem(IDC_BUTTON_OPEN2)->EnableWindow(!m_bSerialPortOpened2);
    GetDlgItem(IDC_BUTTON_CLOSE2)->EnableWindow(m_bSerialPortOpened2);
}

void CSerialPortTestDlg::OnButtonClose2()
{
    // TODO: Add your control notification handler code here
    m_SerialPort2.ClosePort();//�رմ���2
    m_bSerialPortOpened2=FALSE;
    GetDlgItem(IDC_BUTTON_OPEN2)->EnableWindow(!m_bSerialPortOpened2);
    GetDlgItem(IDC_BUTTON_CLOSE2)->EnableWindow(m_bSerialPortOpened2);
}

//���ڽ��ַ��������NMEAͨ��Э���
void CSerialPortTestDlg::SendNMEAData(CString &strData)
{
    char checksum=0,cr=13,ln=10;
    char c1,c2; //2�� ��Bype У��ֵ
    for(int i=0; i<strData.GetLength(); i++)
        checksum = checksum^strData[i];
    c2=checksum & 0x0F;
    c1=((checksum >> 4) & 0x0F);
    if (c1 < 10) c1+= '0';
    else c1 += 'A' - 10;
    if (c2 < 10) c2+= '0';
    else c2 += 'A' - 10;
    CString strNMEAData;
    //���ϰ�ͷ��β��У��ֵ�ͻس����з�
    strNMEAData='$'+strData+"*"+c1+c2+cr+ln;
    //���¼��г���ز�ͨ�ã����Լ��ĳ�����ע���޸�
    m_SerialPort.WriteToPort((LPCTSTR)strNMEAData);
    m_strEditSendMsg.Format("���͵����ݰ�Ϊ��%s",strNMEAData);
    UpdateData(FALSE);  //�ڷ�����ʾ�༭������ʾ���͵����ݰ�
}
