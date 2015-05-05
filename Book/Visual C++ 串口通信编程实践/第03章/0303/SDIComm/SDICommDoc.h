// SDICommDoc.h : interface of the CSDICommDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SDICOMMDOC_H__F057A716_4B88_4AD9_9BF6_98DD61525487__INCLUDED_)
#define AFX_SDICOMMDOC_H__F057A716_4B88_4AD9_9BF6_98DD61525487__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSDICommDoc : public CDocument
{
protected: // create from serialization only
	CSDICommDoc();
	DECLARE_DYNCREATE(CSDICommDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSDICommDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSDICommDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSDICommDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SDICOMMDOC_H__F057A716_4B88_4AD9_9BF6_98DD61525487__INCLUDED_)
