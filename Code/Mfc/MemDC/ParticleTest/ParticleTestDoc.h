// ParticleTestDoc.h : interface of the CParticleTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PARTICLETESTDOC_H__FE68199D_9DE8_11D3_AB75_00805FC73D05__INCLUDED_)
#define AFX_PARTICLETESTDOC_H__FE68199D_9DE8_11D3_AB75_00805FC73D05__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CParticleTestDoc : public CDocument
{
protected: // create from serialization only
	CParticleTestDoc();
	DECLARE_DYNCREATE(CParticleTestDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CParticleTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CParticleTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CParticleTestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	// Generated OLE dispatch map functions
	//{{AFX_DISPATCH(CParticleTestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DISPATCH
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PARTICLETESTDOC_H__FE68199D_9DE8_11D3_AB75_00805FC73D05__INCLUDED_)
