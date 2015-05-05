// SerialPortTest2View.h : interface of the CSerialPortTest2View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SERIALPORTTEST2VIEW_H__C0B3B64C_5574_11D8_870F_00E04C3F78CA__INCLUDED_)
#define AFX_SERIALPORTTEST2VIEW_H__C0B3B64C_5574_11D8_870F_00E04C3F78CA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SerialPort.h"  //添加CSerialPort类头文件

class CSerialPortTest2View : public CView
{
protected: // create from serialization only
	CSerialPortTest2View();
	DECLARE_DYNCREATE(CSerialPortTest2View)

// Attributes
public:
	CSerialPort m_SerialPort[2]; //定义2个CSerialPort类对象
	BOOL m_bCOM1Opened; //用于标志COM1是否打开
	BOOL m_bCOM2Opened; //用于标志COM2是否打开
	CString m_strRXDataCOM1;   //COM1接收数据
	CString m_strRXDataCOM2;   //COM2接收数据
	CSerialPortTest2Doc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSerialPortTest2View)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSerialPortTest2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSerialPortTest2View)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnComm(WPARAM ch, LPARAM port);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SerialPortTest2View.cpp
inline CSerialPortTest2Doc* CSerialPortTest2View::GetDocument()
   { return (CSerialPortTest2Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SERIALPORTTEST2VIEW_H__C0B3B64C_5574_11D8_870F_00E04C3F78CA__INCLUDED_)
