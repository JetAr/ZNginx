// SerialPortTest2View.cpp : implementation of the CSerialPortTest2View class
//

#include "stdafx.h"
#include "SerialPortTest2.h"

#include "SerialPortTest2Doc.h"
#include "SerialPortTest2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2View

IMPLEMENT_DYNCREATE(CSerialPortTest2View, CView)

BEGIN_MESSAGE_MAP(CSerialPortTest2View, CView)
	//{{AFX_MSG_MAP(CSerialPortTest2View)
	ON_MESSAGE(WM_COMM_RXCHAR, OnComm)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2View construction/destruction

CSerialPortTest2View::CSerialPortTest2View()
{
	// TODO: add construction code here
	m_bCOM1Opened=FALSE; //COM1��ʼ״̬û�д�
	m_bCOM2Opened=FALSE; //COM2��ʼ״̬û�д�
}

CSerialPortTest2View::~CSerialPortTest2View()
{
}

BOOL CSerialPortTest2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2View drawing

void CSerialPortTest2View::OnDraw(CDC* pDC)
{
	CSerialPortTest2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2View printing

BOOL CSerialPortTest2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSerialPortTest2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSerialPortTest2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2View diagnostics

#ifdef _DEBUG
void CSerialPortTest2View::AssertValid() const
{
	CView::AssertValid();
}

void CSerialPortTest2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSerialPortTest2Doc* CSerialPortTest2View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSerialPortTest2Doc)));
	return (CSerialPortTest2Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2View message handlers


LONG CSerialPortTest2View::OnComm(WPARAM ch, LPARAM port)
{
	if(port==2)   //COM2���յ�����    
	{
		switch(ch)
		{
		case '$':
			m_strRXDataCOM2=(char)ch;
			break;
		case '*':
			{
			m_strRXDataCOM2+=(char)ch;
			CDC* pDC=GetDC();   //׼��������ʾ
  			pDC->TextOut(10,150,"COM2���յ�"+m_strRXDataCOM2);//��ʾ���յ������� 
			ReleaseDC(pDC);
			m_strRXDataCOM2.Replace('$','Y');
			//COM2����Ӧ����Ϣ
			m_SerialPort[1].WriteToPort((LPCTSTR)m_strRXDataCOM2); 
			}
			break;
		default:
			m_strRXDataCOM2+=(char)ch;
			break;
		}
	}

	if(port==1)	    //COM1���յ�����
	{
		switch(ch)
		{
		case 'Y':
			m_strRXDataCOM1=(char)ch;
			break;
		case '*':
			{
			m_strRXDataCOM1+=(char)ch;
			CDC* pDC=GetDC();   //׼��������ʾ
  			pDC->TextOut(200,100,"COM1���յ���"+m_strRXDataCOM1);//��ʾ���յ������� 
			ReleaseDC(pDC);
			}
			break;
		default:
			m_strRXDataCOM1+=(char)ch;
			break;
		}
	}

	return 0;
}

void CSerialPortTest2View::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	// TODO: Add your specialized code here and/or call the base class
	for(int i=0;i<2;i++)
	{
		if (m_SerialPort[i].InitPort(this,i+1,9600,'N',8,1,EV_RXFLAG | EV_RXCHAR,512))
		{
			m_SerialPort[i].StartMonitoring();   //�������ڼ����߳�
			if(i==0)            //�����COM1�򿪳ɹ�,�����ö�ʱ��
			{
				SetTimer(1,1000,NULL);  //���ö�ʱ����1���������
				m_bCOM1Opened=TRUE;     //COM1��
			}
			else
				m_bCOM2Opened=TRUE;     //COM2��
		}
		else  //��ʼ�����ɹ�������ʾ��ʾ��Ϣ
		{
			CString strTemp;
			strTemp.Format("COM%d û�з��֣��������豸ռ��",i+1);
			AfxMessageBox(strTemp);
		}
	}
}



void CSerialPortTest2View::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	static UINT unCount=0; //���徲̬����
	if(m_bCOM1Opened) //���COM1�Ƿ�ɹ���
	{
		unCount++;
		CString strTemp;
		strTemp.Format("$%06d*",unCount);
		m_SerialPort[0].WriteToPort((LPCTSTR)strTemp); //COM1��������
		CDC* pDC=GetDC();   //׼��������ʾ
  		pDC->TextOut(10,100,"COM1���ͣ�"+strTemp);//��ʾ���յ������� 
		ReleaseDC(pDC);
	}
	CView::OnTimer(nIDEvent);
}
