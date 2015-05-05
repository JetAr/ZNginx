// SDICommDoc.cpp : implementation of the CSDICommDoc class
//

#include "stdafx.h"
#include "SDIComm.h"

#include "SDICommDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDICommDoc

IMPLEMENT_DYNCREATE(CSDICommDoc, CDocument)

BEGIN_MESSAGE_MAP(CSDICommDoc, CDocument)
	//{{AFX_MSG_MAP(CSDICommDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSDICommDoc construction/destruction

CSDICommDoc::CSDICommDoc()
{
	// TODO: add one-time construction code here

}

CSDICommDoc::~CSDICommDoc()
{
}

BOOL CSDICommDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSDICommDoc serialization

void CSDICommDoc::Serialize(CArchive& ar)
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
// CSDICommDoc diagnostics

#ifdef _DEBUG
void CSDICommDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CSDICommDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSDICommDoc commands
