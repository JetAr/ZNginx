// SDICommView.cpp : implementation of the CSDICommView class
//

#include "stdafx.h"
#include "SDIComm.h"

#include "SDICommDoc.h"
#include "SDICommView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDICommView

IMPLEMENT_DYNCREATE(CSDICommView, CView)

BEGIN_MESSAGE_MAP(CSDICommView, CView)
	//{{AFX_MSG_MAP(CSDICommView)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()



/////////////////////////////////////////////////////////////////////////////
// CSDICommView construction/destruction

CSDICommView::CSDICommView()
{
	// TODO: add construction code here

}

CSDICommView::~CSDICommView()
{
}

BOOL CSDICommView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSDICommView drawing

void CSDICommView::OnDraw(CDC* pDC)
{
	CSDICommDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSDICommView printing

BOOL CSDICommView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSDICommView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSDICommView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSDICommView diagnostics

#ifdef _DEBUG
void CSDICommView::AssertValid() const
{
	CView::AssertValid();
}

void CSDICommView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSDICommDoc* CSDICommView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSDICommDoc)));
	return (CSDICommDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSDICommView message handlers

int CSDICommView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_MSComm.Create(NULL,0,CRect(0,0,0,0),this,IDC_MSCOMM1);
	if(m_MSComm.GetPortOpen())	//如果串口是打开的，则行关闭串口
		m_MSComm.SetPortOpen(FALSE); 

	m_MSComm.SetCommPort(2);				//选择COM2
	m_MSComm.SetInBufferSize(1024);			//接收缓冲区
	m_MSComm.SetOutBufferSize(1024);		//发送缓冲区
	m_MSComm.SetInputLen(0);				//设置当前接收区数据长度为0,表示全部读取
	m_MSComm.SetInputMode(1);				//以二进制方式读写数据
	m_MSComm.SetRThreshold(5);				//接收缓冲区有5个及5个以上字符时，将引发接收数据的OnComm事件
	m_MSComm.SetSettings("9600,n,8,1");	    //波特率：9600，无检验位，8个数据位，1个停止位

	if(!m_MSComm.GetPortOpen())				//如果串口没有打开则打开
		m_MSComm.SetPortOpen(TRUE);			//打开串口
	else
		AfxMessageBox("Open Serial Port Failure!");
	m_MSComm.GetInput();					//先预读缓冲区以清除残留数据

	
	return 0;
}

/////////////以下为手工添加的代码///////////////////////////////////////////////

BEGIN_EVENTSINK_MAP(CSDICommView, CView)
    //{{AFX_EVENTSINK_MAP(CAboutDlg)
	ON_EVENT(CSDICommView, IDC_MSCOMM1, 1, OnComm, VTS_NONE)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CSDICommView::OnComm() 
{
	// TODO: Add your control notification handler code here
	VARIANT variant_inp;
	COleSafeArray safearray_inp;
	LONG len,k;
	BYTE rxdata[2048]; //设置BYTE数组
	CString strtemp;
	CString strDisp;  //用于显示接收数据

    if(m_MSComm.GetCommEvent()==2)
	{

		variant_inp=m_MSComm.GetInput(); //读缓冲区
		safearray_inp=variant_inp;  //VARIANT型变量转换为ColeSafeArray型变量
		len=safearray_inp.GetOneDimSize(); //得到有效数据长度
		for(k=0;k<len;k++)
			safearray_inp.GetElement(&k,rxdata+k);//转换为BYTE型数组
		for(k=0;k<len;k++)             //将数组转换为Cstring型变量
		{
			BYTE bt=*(char*)(rxdata+k);    //字符型
			strtemp.Format("%c",bt);    //将字符送入临时变量strtemp存放
			strDisp+=strtemp;  //加入接收编辑框对应字符串    
		}

		CDC* pDC=GetDC();   //准备数据显示
 		pDC->TextOut(200,100,"COM2接收到："+strDisp);//显示接收到的数据 
		ReleaseDC(pDC);

		strtemp.Format("OK,'%s' Received ",strDisp);
		m_MSComm.SetOutput(COleVariant(strtemp));//发送数据
	}
}


/////////////手工添加的代码结束////////////////////////////////////////////////////
