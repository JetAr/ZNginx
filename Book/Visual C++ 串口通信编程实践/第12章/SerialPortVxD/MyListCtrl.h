#if !defined(AFX_MYLISTCTRL_H__1404C920_E65B_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_MYLISTCTRL_H__1404C920_E65B_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyListCtrl.h : header file
//

////////////////////////////////////////////////////////////////////////
// CListRecord：列表数据记录类
class CListRecord
	{
	public:
		CListRecord()
			{
			m_dwLength = 0;
			}
		~CListRecord() {}

		WORD	m_pwData[32];
		DWORD	m_dwLength;
	};



/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl window

class CMyListCtrl : public CListCtrl
{
// Construction
public:
	CMyListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyListCtrl)
	virtual void DrawItem( LPDRAWITEMSTRUCT lpDrawItemStruct );
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMyListCtrl();

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyListCtrl)
	afx_msg void OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYLISTCTRL_H__1404C920_E65B_11D8_870F_00E04C3F78CA__INCLUDED_)
