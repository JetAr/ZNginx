// MFC_appDoc.h : interface of the CMFC_appDoc class
//


#pragma once


class CMFC_appDoc : public CDocument
{
protected: // create from serialization only
	CMFC_appDoc();
	DECLARE_DYNCREATE(CMFC_appDoc)

	// Attributes
public:

	// Operations
public:

	// Overrides
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	// Implementation
public:
	virtual ~CMFC_appDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};


