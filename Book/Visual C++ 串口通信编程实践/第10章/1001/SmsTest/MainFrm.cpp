// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SmsTest.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_SEND, OnSend)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	if (!m_wndDialogBar.Create(this, IDD_SEND_SM,
      CBRS_BOTTOM|CBRS_TOOLTIPS|CBRS_FLYBY, IDD_SEND_SM))
	{
		TRACE0("Failed to create dialog bar\n");
		return -1;      // fail to create
	}

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndDialogBar.EnableDocking(CBRS_ALIGN_TOP|CBRS_ALIGN_BOTTOM);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
	DockControlBar(&m_wndDialogBar);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnSend() 
{
	// TODO: Add your control notification handler code here
	CComboBox* pNumberWnd=(CComboBox*)m_wndDialogBar.GetDlgItem(IDC_NUMBER);
	CComboBox* pContentWnd=(CComboBox*)m_wndDialogBar.GetDlgItem(IDC_CONTENT);

	CString strSmsc;
	CString strNumber;
	CString strContent;

	strSmsc = theApp.m_strSmsc;
	pNumberWnd->GetWindowText(strNumber);
	pContentWnd->GetWindowText(strContent);

	// 检查号码
	if(strNumber.GetLength() < 11)
	{
		AfxMessageBox("请输入正确的号码！");
		pNumberWnd->SetFocus();
		pNumberWnd->SetEditSel(-1, 0);
		return;
	}

	// 检查短消息内容是否空，或者超长
	CString strUnicode;
	WCHAR wchar[1024];
	int nCount = ::MultiByteToWideChar(CP_ACP, 0, strContent, -1, wchar, 1024);
	if(nCount <= 1)
	{
		AfxMessageBox("请输入消息内容！");
		pContentWnd->SetFocus();
		pContentWnd->SetEditSel(-1, 0);
		return;
	}
	else if(nCount > 70)		// 我们决定全部用UCS2编码，最大70个字符(半角/全角)
	{
		AfxMessageBox("消息内容太长，无法发送！");
		pContentWnd->SetFocus();
		pContentWnd->SetEditSel(-1, 0);
		return;
	}

	if(AfxMessageBox("确定发送吗？", MB_YESNO) == IDYES)
	{
		SM_PARAM SmParam;

		memset(&SmParam, 0, sizeof(SM_PARAM));

		// 去掉号码前的"+"
		if(strSmsc[0] == '+')  strSmsc = strSmsc.Mid(1);
		if(strNumber[0] == '+')  strNumber = strNumber.Mid(1);

		// 在号码前加"86"
		if(strSmsc.Left(2) != "86")  strSmsc = "86" + strSmsc;
		if(strNumber.Left(2) != "86")  strNumber = "86" + strNumber;

		// 填充短消息结构
		strcpy(SmParam.SCA, strSmsc);
		strcpy(SmParam.TPA, strNumber);
		strcpy(SmParam.TP_UD, strContent);
		SmParam.TP_PID = 0;
		SmParam.TP_DCS = GSM_UCS2;

		// 发送短消息
		theApp.m_pSmsTraffic->PutSendMessage(&SmParam);

		// 列表中加入新串
		if(pNumberWnd->FindStringExact(-1, strNumber)<0)  pNumberWnd->InsertString(0, strNumber);
		if(pContentWnd->FindStringExact(-1, strContent)<0)  pContentWnd->InsertString(0, strContent);
	}
	
	pContentWnd->SetFocus();
	pContentWnd->SetEditSel(-1, 0);	
}

