// SerialPortTest2Doc.cpp : implementation of the CSerialPortTest2Doc class
//

#include "stdafx.h"
#include "SerialPortTest2.h"

#include "SerialPortTest2Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2Doc

IMPLEMENT_DYNCREATE(CSerialPortTest2Doc, CDocument)

BEGIN_MESSAGE_MAP(CSerialPortTest2Doc, CDocument)
	//{{AFX_MSG_MAP(CSerialPortTest2Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2Doc construction/destruction

CSerialPortTest2Doc::CSerialPortTest2Doc()
{
	// TODO: add one-time construction code here

}

CSerialPortTest2Doc::~CSerialPortTest2Doc()
{
}

BOOL CSerialPortTest2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2Doc serialization

void CSerialPortTest2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2Doc diagnostics

#ifdef _DEBUG
void CSerialPortTest2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSerialPortTest2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSerialPortTest2Doc commands
