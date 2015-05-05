// SerialPortTest2Doc.h : interface of the CSerialPortTest2Doc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALPORTTEST2DOC_H__C0B3B64A_5574_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SERIALPORTTEST2DOC_H__C0B3B64A_5574_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CSerialPortTest2Doc : public CDocument
{
protected: // create from serialization only
	CSerialPortTest2Doc();
	DECLARE_DYNCREATE(CSerialPortTest2Doc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPortTest2Doc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSerialPortTest2Doc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSerialPortTest2Doc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTTEST2DOC_H__C0B3B64A_5574_11D8_870F_00E04C3F78CA__INCLUDED_)
