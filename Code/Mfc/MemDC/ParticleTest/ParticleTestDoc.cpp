// ParticleTestDoc.cpp : implementation of the CParticleTestDoc class
//

#include "stdafx.h"
#include "ParticleTest.h"

#include "ParticleTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CParticleTestDoc

IMPLEMENT_DYNCREATE(CParticleTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CParticleTestDoc, CDocument)
	//{{AFX_MSG_MAP(CParticleTestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CParticleTestDoc, CDocument)
	//{{AFX_DISPATCH_MAP(CParticleTestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//      DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IParticleTest to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {FE681993-9DE8-11D3-AB75-00805FC73D05}
static const IID IID_IParticleTest =
{ 0xfe681993, 0x9de8, 0x11d3, { 0xab, 0x75, 0x0, 0x80, 0x5f, 0xc7, 0x3d, 0x5 } };

BEGIN_INTERFACE_MAP(CParticleTestDoc, CDocument)
	INTERFACE_PART(CParticleTestDoc, IID_IParticleTest, Dispatch)
END_INTERFACE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CParticleTestDoc construction/destruction

CParticleTestDoc::CParticleTestDoc()
{
	// TODO: add one-time construction code here

	EnableAutomation();

	AfxOleLockApp();
}

CParticleTestDoc::~CParticleTestDoc()
{
	AfxOleUnlockApp();
}

BOOL CParticleTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CParticleTestDoc serialization

void CParticleTestDoc::Serialize(CArchive& ar)
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
// CParticleTestDoc diagnostics

#ifdef _DEBUG
void CParticleTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CParticleTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CParticleTestDoc commands
