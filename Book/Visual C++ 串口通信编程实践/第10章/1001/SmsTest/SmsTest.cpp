// SmsTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "SmsTest.h"

#include "Comm.h"

#include "MainFrm.h"
#include "SmsTestDoc.h"
#include "SmsTestView.h"

#include "SettingsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmsTestApp

BEGIN_MESSAGE_MAP(CSmsTestApp, CWinApp)
	//{{AFX_MSG_MAP(CSmsTestApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_SETTINGS, OnSettings)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmsTestApp construction

CSmsTestApp::CSmsTestApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSmsTestApp object

CSmsTestApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSmsTestApp initialization

BOOL CSmsTestApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("<<Demo by Kernel Studio>>"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	m_strPort=GetProfileString("Settings", "Port");
	m_strRate=GetProfileString("Settings", "Rate");
	m_strSmsc=GetProfileString("Settings", "SMSC");

	m_pSmsTraffic = NULL;

	if (!CheckSettings()) 	return FALSE;

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSmsTestDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CSmsTestView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The one and only window has been initialized, so show and update it.
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	m_pSmsTraffic = new CSmsTraffic;

	return TRUE;
}

int CSmsTestApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class

	if (m_pSmsTraffic != NULL)
	{
		delete m_pSmsTraffic;
	}

	CloseComm();

	WriteProfileString("Settings", "Port", m_strPort);
	WriteProfileString("Settings", "Rate", m_strRate);
	WriteProfileString("Settings", "SMSC", m_strSmsc);
	
	return CWinApp::ExitInstance();
}

BOOL CSmsTestApp::CheckSettings()
{
	int nRate;

	sscanf(m_strRate, "%d", &nRate);

	while (!::OpenComm(m_strPort, nRate))
	{
		CString strError;
		strError.Format("无法打开端口%s! 现在设置吗?", m_strPort);
		
		if (AfxMessageBox(strError, MB_YESNO) == IDNO) return FALSE;

		CSettingsDlg dlg;
	
		dlg.m_strPort = m_strPort;
		dlg.m_strRate = m_strRate;
		dlg.m_strSmsc = m_strSmsc;

		if (dlg.DoModal() == IDOK)
		{
			m_strPort = dlg.m_strPort;
			m_strRate = dlg.m_strRate;
			m_strSmsc = dlg.m_strSmsc;
		}
		else
		{
			return FALSE;
		}
	}

	if (!gsmInit())
	{
		AfxMessageBox("该端口上没有发现MODEM!");

		return FALSE;
	}

	if (m_strSmsc.IsEmpty())
	{
		AfxMessageBox("请设置SMSC!");

		CSettingsDlg dlg;
	
		dlg.m_strPort = m_strPort;
		dlg.m_strRate = m_strRate;
		dlg.m_strSmsc = m_strSmsc;

		if (dlg.DoModal() == IDOK)
		{
			m_strPort = dlg.m_strPort;
			m_strRate = dlg.m_strRate;
			m_strSmsc = dlg.m_strSmsc;
		}
		else
		{
			return FALSE;
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSmsTestApp message handlers

void CSmsTestApp::OnSettings() 
{
	// TODO: Add your command handler code here
	CSettingsDlg dlg;
	
	dlg.m_strPort = m_strPort;
	dlg.m_strRate = m_strRate;
	dlg.m_strSmsc = m_strSmsc;

	if(dlg.DoModal() == IDOK)
	{
		if(m_strPort != dlg.m_strPort)
		{
			AfxMessageBox("端口设置在下次启动程序时生效");
		}
		m_strPort = dlg.m_strPort;
		m_strRate = dlg.m_strRate;
		m_strSmsc = dlg.m_strSmsc;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strInfo = _T("    本程序是一个通过串口收发端消息的范例。仅供学习和研究之用。\r\n"
		"    多数手机和所有GSM模块本身具备MODEM功能，可以直接使用扩展的AT命令进行控制。"
		"对于不具备此功能的手机(如Nokia5110等)，需安装专门的驱动，虚拟出MODEM及串口，才能进行类似操作。\r\n"
		"    主窗口上部为接收的短消息列表。读出短消息后会自动从手机中删除。主窗口下部是发送输入界面。\r\n"
		"    本程序作者为bhw98。若发现bug或有好的建议，请联系：bhw98@sina.com");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_INFO, m_strInfo);
	DDV_MaxChars(pDX, m_strInfo, 1024);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CSmsTestApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

