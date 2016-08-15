// MFC_appDoc.cpp : implementation of the CMFC_appDoc class
//

#include "stdafx.h"
#include "MFC_app.h"

#include "MFC_appDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFC_appDoc

IMPLEMENT_DYNCREATE(CMFC_appDoc, CDocument)

BEGIN_MESSAGE_MAP(CMFC_appDoc, CDocument)
END_MESSAGE_MAP()


// CMFC_appDoc construction/destruction

CMFC_appDoc::CMFC_appDoc()
{
	// TODO: add one-time construction code here

}

CMFC_appDoc::~CMFC_appDoc()
{
}

BOOL CMFC_appDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}




// CMFC_appDoc serialization

void CMFC_appDoc::Serialize(CArchive& ar)
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


// CMFC_appDoc diagnostics

#ifdef _DEBUG
void CMFC_appDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CMFC_appDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CMFC_appDoc commands
