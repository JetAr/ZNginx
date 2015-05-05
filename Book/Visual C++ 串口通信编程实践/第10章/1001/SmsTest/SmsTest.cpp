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
		strError.Format("�޷��򿪶˿�%s! ����������?", m_strPort);
		
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
		AfxMessageBox("�ö˿���û�з���MODEM!");

		return FALSE;
	}

	if (m_strSmsc.IsEmpty())
	{
		AfxMessageBox("������SMSC!");

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
			AfxMessageBox("�˿��������´���������ʱ��Ч");
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
	m_strInfo = _T("    ��������һ��ͨ�������շ�����Ϣ�ķ���������ѧϰ���о�֮�á�\r\n"
		"    �����ֻ�������GSMģ�鱾��߱�MODEM���ܣ�����ֱ��ʹ����չ��AT������п��ơ�"
		"���ڲ��߱��˹��ܵ��ֻ�(��Nokia5110��)���谲װר�ŵ������������MODEM�����ڣ����ܽ������Ʋ�����\r\n"
		"    �������ϲ�Ϊ���յĶ���Ϣ�б���������Ϣ����Զ����ֻ���ɾ�����������²��Ƿ���������档\r\n"
		"    ����������Ϊbhw98��������bug���кõĽ��飬����ϵ��bhw98@sina.com");
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

