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
    m_bSerialPortOpened=FALSE; //初始状态：串口1没有打开
    m_bSerialPortOpened2=FALSE; //初始状态：串口2没有打开
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
    m_ctrlComboComPort.SetCurSel(0); //初始选择串口1
    m_ctrlComboComPort2.SetCurSel(1); //初始选择串口2

    //以下两句分别设置“打开串口”、“关闭串口”两个按状态的使能状态
    GetDlgItem(IDC_BUTTON_OPEN)->EnableWindow(!m_bSerialPortOpened);
    GetDlgItem(IDC_BUTTON_CLOSE)->EnableWindow(m_bSerialPortOpened);
    GetDlgItem(IDC_BUTTON_OPEN2)->EnableWindow(!m_bSerialPortOpened2);
    GetDlgItem(IDC_BUTTON_CLOSE2)->EnableWindow(m_bSerialPortOpened2);

    //载入图标
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

#define CR 0X0D    //回车
#define LF 0X0A    //换行
LONG CSerialPortTestDlg::OnComm(WPARAM ch, LPARAM port)
{
    static char checksum=0,checksum1=0;
    static int count1=0;//,count2=0,count3=0;
    static unsigned char buf[20];
    static char c1,c2;    //用于计算半Byte校验
    static int flag;     //用于接收阶段标记
    static int twoflag=0;

    if(port == m_unPort)
    {
        if(ch>127)   //是不是首字节
        {
            count1=0;  //记录接收字符的个数
            buf[count1]=ch;
            checksum1= ch-128;  //开始计算校验值
        }
        else
        {
            count1++;
            buf[count1]=ch;
            checksum1 = checksum1^ch;
            if(count1==3)  //包括校验字节在内的全部接收完毕
            {
                if(checksum1)  //校验错
                {
                    m_strEditReceiveMsg = "接收校验出错";
                    UpdateData(FALSE);
                }
                else
                {
                    CString str;
                    unsigned char * temp=(unsigned char*)buf;
                    m_strEditReceiveMsg ="接收到的简单通信协议字节为：";
                    for(int i=0; i<4; i++)
                    {
                        str.Format("%02X ",*(buf+i));
                        m_strEditReceiveMsg += str;
                    }
                    UpdateData(FALSE);
                }
                if(count1>5)   //防止出错
                    count1=0;
            }
        }
    }

    if(port==m_unPort2)   //串口2的数据处理
    {
        m_strPortRXData2 += (char)ch;
        switch(ch)
        {
        case '$':
            checksum=0;			//开始计算CheckSum
            flag=0;
            break;
        case '*':  //有效数据结束，可以$和*之间数据的半Byte校验值了
            flag=2;
            c2=checksum & 0x0f;
            c1=((checksum >> 4) & 0x0f);
            if (c1 < 10) c1+= '0';
            else c1 += 'A' - 10;
            if (c2 < 10) c2+= '0';
            else c2 += 'A' - 10;
            break;
        case CR:   //这句必须加上，否则会出错的
            break;
        case LF:   //数据包的最后一个字符
            m_strPortRXData2.Empty();
            break;
        default:
            if(flag>0)  //注意：只有在接收到'*'后，flag才大于0
            {
                m_strChecksum2 += ch;
                if(flag==1)
                {
                    CString strCheck="";
                    strCheck.Format("%c%c",c1,c2);
                    if(strCheck!=m_strChecksum2)  //校验计算不正确，说明接收数据出错
                    {
                        m_strPortRXData2.Empty();
                    }
                    else  //校验计算正确则处理数据
                    {
                        CString strSwitchSetData;
                        strSwitchSetData = m_strPortRXData2.Mid(1,9);
                        //以下设置信号灯状态
                        for(int i=0; i<9; i++)
                        {
                            if(strSwitchSetData.Mid(i,1)=="1")
                                SetSwitchStatus(i+1,TRUE);
                            else
                                SetSwitchStatus(i+1,FALSE);
                        }
                        //以下取出位置与速度数据
                        CString strTemp;
                        strTemp = m_strPortRXData2.Mid(10,5);
                        char *temp=(char*)((LPCTSTR)strTemp);
                        char tbuf[4];
                        tbuf[0]=temp[0];
                        tbuf[1]=temp[1];
                        tbuf[2]=0;
                        m_unEditPosition2 = atoi(tbuf);  //得到位置状态值
                        tbuf[0]=temp[2];
                        tbuf[1]=temp[3];
                        tbuf[2]=temp[4];
                        tbuf[3]=0;
                        m_unEditVelocity2 = atoi(tbuf);  //得到速度值
                        //将接收到的数据包内容显示
                        m_strEditDispData2 = "接收到NMEA数据包："+m_strPortRXData2;
                        //下面准备发送"简单自定义通信协议"数据包
                        unsigned char ucChar[4];
                        //首字节
                        ucChar[0]=0x80;   //首字节最高位置1
                        if(strSwitchSetData.Mid(0,1)=="1")  // 开关1状态
                            ucChar[0] |= 0x40;            //0100 0000
                        else
                            ucChar[0] &= 0xBF;	          //1011 1111
                        if(strSwitchSetData.Mid(1,1)=="1")  // 开关2状态
                            ucChar[0] |= 0x20;            //0010 0000
                        else
                            ucChar[0] &= 0xDF;	          //1101 1111
                        if(m_unEditPosition2>31)      //对位置值进行限值
                            m_unEditPosition2=31;
                        ucChar[0] &= 0xE0;    //将首字节的低5位置0
                        ucChar[0] += m_unEditPosition2; //再加上位置值
                        ucChar[3] = ucChar[0];    //同时计算校验值
                        //第二字节
                        unsigned char ucTemp=0x40;
                        for(i=0; i<7; i++)
                        {
                            if(strSwitchSetData.Mid(2+i,1)=="1")
                                ucChar[1] |= ucTemp;
                            else
                                ucChar[1] &= (~ucTemp);
                            ucTemp >>= 1;
                        }
                        ucChar[1] &= 0x7F; //前面做了那么多运算，用这条语句保证一下最高位为0
                        ucChar[3] ^= ucChar[1];   //计算校验值
                        //第三字节
                        if(m_unEditVelocity2>120)
                            m_unEditVelocity2=120;
                        ucChar[2] = m_unEditVelocity2;
                        ucChar[3] ^= ucChar[2];   //计算校验值
                        //第四字节
                        ucChar[3] &= 0x7F;
                        //把"简单自定义通信协议"数据包发送出去
                        if(m_bSerialPortOpened2)
                            m_SerialPort2.WriteToPort(ucChar,4);
                        //同时把发送的内容显示
                        CString strTemp1;
                        strTemp=   _T("发送的内容为: ");
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
                //从'*'后收，flag=2,1次减1操作，正好将数据包的校验值保存在m_strChecksum中
                flag--;
            }
            else
                checksum=checksum^ch;  //当flag<=0时，计算校验值
            break;
        }
    }

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
        m_unPort=nPort;
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
    if(!m_bSerialPortOpened) //检查串口是否打开
    {
        AfxMessageBox("串口没有打开");
        return;
    }
    UpdateData(TRUE); //读入编辑框中的数据
    CString strSend=""; //要发送的NEMA字符串
    //以下读入9个开关的设置状态
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


//该函数用于设置开关的信号灯状态，如开关5开启，
//可设置为 SetSwitchStatus(5,TRUE);
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


//该函数用于得到设置的开关状态值
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
    int nPort=m_ctrlComboComPort2.GetCurSel()+1; //得到串口号
    if(m_SerialPort2.InitPort(this, nPort, 9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))
    {
        m_SerialPort2.StartMonitoring();
        m_bSerialPortOpened2=TRUE;
        m_unPort2=nPort;   //记录打开的串口号
    }
    else
    {
        AfxMessageBox("没有发现此串口或被占用");
        m_bSerialPortOpened2=FALSE;
    }
    GetDlgItem(IDC_BUTTON_OPEN2)->EnableWindow(!m_bSerialPortOpened2);
    GetDlgItem(IDC_BUTTON_CLOSE2)->EnableWindow(m_bSerialPortOpened2);
}

void CSerialPortTestDlg::OnButtonClose2()
{
    // TODO: Add your control notification handler code here
    m_SerialPort2.ClosePort();//关闭串口2
    m_bSerialPortOpened2=FALSE;
    GetDlgItem(IDC_BUTTON_OPEN2)->EnableWindow(!m_bSerialPortOpened2);
    GetDlgItem(IDC_BUTTON_CLOSE2)->EnableWindow(m_bSerialPortOpened2);
}

//用于将字符串打包成NMEA通信协议包
void CSerialPortTestDlg::SendNMEAData(CString &strData)
{
    char checksum=0,cr=13,ln=10;
    char c1,c2; //2个 半Bype 校验值
    for(int i=0; i<strData.GetLength(); i++)
        checksum = checksum^strData[i];
    c2=checksum & 0x0F;
    c1=((checksum >> 4) & 0x0F);
    if (c1 < 10) c1+= '0';
    else c1 += 'A' - 10;
    if (c2 < 10) c2+= '0';
    else c2 += 'A' - 10;
    CString strNMEAData;
    //加上包头，尾和校验值和回车换行符
    strNMEAData='$'+strData+"*"+c1+c2+cr+ln;
    //以下几行程序关不通用，在自己的程序中注意修改
    m_SerialPort.WriteToPort((LPCTSTR)strNMEAData);
    m_strEditSendMsg.Format("发送的数据包为：%s",strNMEAData);
    UpdateData(FALSE);  //在发送显示编辑框中显示发送的数据包
}
