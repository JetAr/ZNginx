// SetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Term.h"
#include "SetupDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg dialog


CSetupDlg::CSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetupDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDlg)
	DDX_Control(pDX, IDC_FLOWCTRL, m_ctrlFlowCtrl);
	DDX_Control(pDX, IDC_STOPBITS, m_ctrlStopbits);
	DDX_Control(pDX, IDC_PORT, m_ctrlPort);
	DDX_Control(pDX, IDC_PARITY, m_ctrlParity);
	DDX_Control(pDX, IDC_DATABITS, m_ctrlDatabits);
	DDX_Control(pDX, IDC_BAUD, m_ctrlBaud);
	DDX_Radio(pDX, IDC_FLOWCTRL, m_nFlowctrl);
	DDX_CBString(pDX, IDC_PORT, m_strPort);
	DDX_CBString(pDX, IDC_BAUD, m_strBaud);
	DDX_CBString(pDX, IDC_DATABITS, m_strDatabits);
	DDX_Check(pDX, IDC_ECHO, m_bEcho);
	DDX_Check(pDX, IDC_NEWLINE, m_bNewline);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CSetupDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDlg message handlers

BOOL CSetupDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	//对对话框的显示情况进行初始化
	m_ctrlPort.AddString(_T("COM1"));
	m_ctrlPort.AddString(_T("COM2"));
	m_ctrlPort.AddString(_T("COM3"));
	m_ctrlPort.AddString(_T("COM4"));

	m_ctrlParity.AddString(_T("NONE"));
	m_ctrlParity.AddString(_T("EVEN"));
	m_ctrlParity.AddString(_T("ODD"));
	m_ctrlParity.SetCurSel(m_nParity);

	m_ctrlDatabits.AddString(_T("5"));
	m_ctrlDatabits.AddString(_T("6"));
	m_ctrlDatabits.AddString(_T("7"));
	m_ctrlDatabits.AddString(_T("8"));
	
	m_ctrlBaud.AddString(_T("300"));
	m_ctrlBaud.AddString(_T("600"));
	m_ctrlBaud.AddString(_T("1200"));
	m_ctrlBaud.AddString(_T("2400"));
	m_ctrlBaud.AddString(_T("4800"));
	m_ctrlBaud.AddString(_T("9600"));
	m_ctrlBaud.AddString(_T("14400"));
	m_ctrlBaud.AddString(_T("19200"));
	m_ctrlBaud.AddString(_T("38400"));
	m_ctrlBaud.AddString(_T("57600"));

	m_ctrlStopbits.AddString(_T("1"));
	m_ctrlStopbits.AddString(_T("1.5"));
	m_ctrlStopbits.AddString(_T("2"));
	m_ctrlStopbits.SetCurSel(m_nStopBits);

	GetDlgItem(IDC_PORT)->EnableWindow(!m_bConnected);

	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
