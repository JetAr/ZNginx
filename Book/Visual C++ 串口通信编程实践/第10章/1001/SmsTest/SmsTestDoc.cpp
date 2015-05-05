// SmsTestDoc.cpp : implementation of the CSmsTestDoc class
//

#include "stdafx.h"
#include "SmsTest.h"

#include "SmsTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSmsTestDoc

IMPLEMENT_DYNCREATE(CSmsTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CSmsTestDoc, CDocument)
	//{{AFX_MSG_MAP(CSmsTestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSmsTestDoc construction/destruction

CSmsTestDoc::CSmsTestDoc()
{
	// TODO: add one-time construction code here

}

CSmsTestDoc::~CSmsTestDoc()
{
}

BOOL CSmsTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSmsTestDoc serialization

void CSmsTestDoc::Serialize(CArchive& ar)
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
// CSmsTestDoc diagnostics

#ifdef _DEBUG
void CSmsTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSmsTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSmsTestDoc commands
