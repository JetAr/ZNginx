// SmsTestView.cpp : implementation of the CSmsTestView class
//

#include "stdafx.h"
#include "SmsTest.h"

#include "SmsTestDoc.h"
#include "SmsTestView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmsTestView

IMPLEMENT_DYNCREATE(CSmsTestView, CListView)

BEGIN_MESSAGE_MAP(CSmsTestView, CListView)
	//{{AFX_MSG_MAP(CSmsTestView)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CListView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CListView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmsTestView construction/destruction

CSmsTestView::CSmsTestView()
{
	// TODO: add construction code here

}

CSmsTestView::~CSmsTestView()
{
}

BOOL CSmsTestView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style |= LVS_SHOWSELALWAYS | LVS_REPORT;

	return CListView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSmsTestView drawing

void CSmsTestView::OnDraw(CDC* pDC)
{
	CSmsTestDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

void CSmsTestView::OnInitialUpdate()
{
	CListView::OnInitialUpdate();

	// TODO: You may populate your ListView with items by directly accessing
	//  its list control through a call to GetListCtrl().

	CListCtrl& ListCtrl = GetListCtrl();

	ListCtrl.InsertColumn(0, "号码", LVCFMT_LEFT, 100);
	ListCtrl.InsertColumn(1, "时间", LVCFMT_LEFT, 140);
	ListCtrl.InsertColumn(2, "消息内容", LVCFMT_LEFT, 500);

	SetTimer(1, 1000, NULL);
}

/////////////////////////////////////////////////////////////////////////////
// CSmsTestView printing

BOOL CSmsTestView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSmsTestView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSmsTestView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSmsTestView diagnostics

#ifdef _DEBUG
void CSmsTestView::AssertValid() const
{
	CListView::AssertValid();
}

void CSmsTestView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}

CSmsTestDoc* CSmsTestView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSmsTestDoc)));
	return (CSmsTestDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSmsTestView message handlers

void CSmsTestView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if(nIDEvent == 1)
	{
		SM_PARAM SmParam;
		CString strTime;
		CString strNumber;
		CString strContent;

		CListCtrl& ListCtrl = GetListCtrl();

		// 取接收到的短消息
		if(theApp.m_pSmsTraffic->GetRecvMessage(&SmParam))
		{
			// 取短消息信息
			strNumber = SmParam.TPA;
			strContent = SmParam.TP_UD;
			strTime = "20" + CString(&SmParam.TP_SCTS[0],2) 
				+ "-" + CString(&SmParam.TP_SCTS[2],2) 
				+ "-" + CString(&SmParam.TP_SCTS[4],2)
				+ " " + CString(&SmParam.TP_SCTS[6],2) 
				+ ":" + CString(&SmParam.TP_SCTS[8],2) 
				+ ":" + CString(&SmParam.TP_SCTS[10],2);

			// 去掉号码前的"86"
			if(strNumber.Left(2) == "86")  strNumber = strNumber.Mid(2);
			
			// 最多保留200条
			int nItemCount = ListCtrl.GetItemCount();
			if(nItemCount >= 200)
			{
				ListCtrl.DeleteItem(0);
				nItemCount--;
			}
			
			// 插入新消息
			ListCtrl.InsertItem(nItemCount, strNumber);
			ListCtrl.SetItemText(nItemCount, 1, strTime);
			ListCtrl.SetItemText(nItemCount, 2, strContent);
			
			ListCtrl.EnsureVisible(nItemCount, FALSE);
		}
	}
	else
	{
		// other timers
		CListView::OnTimer(nIDEvent);
	}
}

BOOL CSmsTestView::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	KillTimer(1);

	return CListView::DestroyWindow();
}
