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
	if(m_MSComm.GetPortOpen())	//��������Ǵ򿪵ģ����йرմ���
		m_MSComm.SetPortOpen(FALSE); 

	m_MSComm.SetCommPort(2);				//ѡ��COM2
	m_MSComm.SetInBufferSize(1024);			//���ջ�����
	m_MSComm.SetOutBufferSize(1024);		//���ͻ�����
	m_MSComm.SetInputLen(0);				//���õ�ǰ���������ݳ���Ϊ0,��ʾȫ����ȡ
	m_MSComm.SetInputMode(1);				//�Զ����Ʒ�ʽ��д����
	m_MSComm.SetRThreshold(5);				//���ջ�������5����5�������ַ�ʱ���������������ݵ�OnComm�¼�
	m_MSComm.SetSettings("9600,n,8,1");	    //�����ʣ�9600���޼���λ��8������λ��1��ֹͣλ

	if(!m_MSComm.GetPortOpen())				//�������û�д����
		m_MSComm.SetPortOpen(TRUE);			//�򿪴���
	else
		AfxMessageBox("Open Serial Port Failure!");
	m_MSComm.GetInput();					//��Ԥ���������������������

	
	return 0;
}

/////////////����Ϊ�ֹ���ӵĴ���///////////////////////////////////////////////

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
	BYTE rxdata[2048]; //����BYTE����
	CString strtemp;
	CString strDisp;  //������ʾ��������

    if(m_MSComm.GetCommEvent()==2)
	{

		variant_inp=m_MSComm.GetInput(); //��������
		safearray_inp=variant_inp;  //VARIANT�ͱ���ת��ΪColeSafeArray�ͱ���
		len=safearray_inp.GetOneDimSize(); //�õ���Ч���ݳ���
		for(k=0;k<len;k++)
			safearray_inp.GetElement(&k,rxdata+k);//ת��ΪBYTE������
		for(k=0;k<len;k++)             //������ת��ΪCstring�ͱ���
		{
			BYTE bt=*(char*)(rxdata+k);    //�ַ���
			strtemp.Format("%c",bt);    //���ַ�������ʱ����strtemp���
			strDisp+=strtemp;  //������ձ༭���Ӧ�ַ���    
		}

		CDC* pDC=GetDC();   //׼��������ʾ
 		pDC->TextOut(200,100,"COM2���յ���"+strDisp);//��ʾ���յ������� 
		ReleaseDC(pDC);

		strtemp.Format("OK,'%s' Received ",strDisp);
		m_MSComm.SetOutput(COleVariant(strtemp));//��������
	}
}


/////////////�ֹ���ӵĴ������////////////////////////////////////////////////////
