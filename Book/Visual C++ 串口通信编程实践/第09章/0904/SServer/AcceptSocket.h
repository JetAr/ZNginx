#if !defined(AFX_ACCEPTSOCKET_H__C23FFA61_4443_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_ACCEPTSOCKET_H__C23FFA61_4443_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AcceptSocket.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CAcceptSocket command target

class CAcceptSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CAcceptSocket();
	virtual ~CAcceptSocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAcceptSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CAcceptSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACCEPTSOCKET_H__C23FFA61_4443_11D8_870F_00E04C3F78CA__INCLUDED_)
