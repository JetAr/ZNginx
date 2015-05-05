// SmsTestDoc.h : interface of the CSmsTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SMSTESTDOC_H__D5E62B9F_1602_42FA_8931_4059B5EED5C6__INCLUDED_)
#define AFX_SMSTESTDOC_H__D5E62B9F_1602_42FA_8931_4059B5EED5C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSmsTestDoc : public CDocument
{
protected: // create from serialization only
	CSmsTestDoc();
	DECLARE_DYNCREATE(CSmsTestDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSmsTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSmsTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSmsTestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SMSTESTDOC_H__D5E62B9F_1602_42FA_8931_4059B5EED5C6__INCLUDED_)
