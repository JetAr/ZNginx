// SettingsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SmsTest.h"
#include "SettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg dialog


CSettingsDlg::CSettingsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CSettingsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSettingsDlg)
    m_strSmsc = _T("");
    //}}AFX_DATA_INIT
}


void CSettingsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSettingsDlg)
    DDX_Control(pDX, IDC_RATE_LIST, m_ctrlRateList);
    DDX_Control(pDX, IDC_COMM_LIST, m_ctrlCommList);
    DDX_Text(pDX, IDC_SMSC, m_strSmsc);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsDlg, CDialog)
    //{{AFX_MSG_MAP(CSettingsDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsDlg message handlers

BOOL CSettingsDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
    CString strComm;
    for(int i=0; i<9; i++)
    {
        strComm.Format("COM%d", i + 1);
        m_ctrlCommList.AddString(strComm);
    }

    m_ctrlRateList.AddString("9600");
    m_ctrlRateList.AddString("14400");
    m_ctrlRateList.AddString("19200");
    m_ctrlRateList.AddString("28800");
    m_ctrlRateList.AddString("38400");
    m_ctrlRateList.AddString("57600");
    m_ctrlRateList.AddString("115200");

    if(m_strPort.IsEmpty()) m_strPort="COM3";
    if(m_strRate.IsEmpty()) m_strRate="57600";

    m_ctrlCommList.SelectString(-1, m_strPort);
    m_ctrlRateList.SelectString(-1, m_strRate);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CSettingsDlg::OnOK()
{
    // TODO: Add extra validation here
    UpdateData();

    int nSel;
    nSel = m_ctrlCommList.GetCurSel();
    if(nSel > -1) m_ctrlCommList.GetLBText(nSel, m_strPort);
    nSel = m_ctrlRateList.GetCurSel();
    if(nSel > -1) m_ctrlRateList.GetLBText(nSel, m_strRate);

    if(m_strPort.IsEmpty() || m_strRate.IsEmpty() || m_strSmsc.IsEmpty())
    {
        AfxMessageBox("请正确设置端口和SMSC!");
        return;
    }

    CDialog::OnOK();
}
