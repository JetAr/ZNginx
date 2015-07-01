// RenjuDlg.h : header file
//

#if !defined(AFX_RENJUDLG_H__F0F4F824_EA98_45F9_8643_DBA5E66A0A72__INCLUDED_)
#define AFX_RENJUDLG_H__F0F4F824_EA98_45F9_8643_DBA5E66A0A72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRenjuDlg dialog
#include "Define.h"
#include "SearchEngine.h"
#include "NegaScout_TT_HH.h"
#include "NewGameDlg.h"

typedef struct _movestone
{
    BYTE nRenjuID;
    POINT ptMovePoint;
} MOVESTONE;

class CRenjuDlg : public CDialog
{
// Construction
public:
    CRenjuDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
    //{{AFX_DATA(CRenjuDlg)
    enum { IDD = IDD_RENJU_DIALOG };
    CStatic	m_OutputInfo;
    CProgressCtrl	m_ThinkProgress;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRenjuDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    //}}AFX_VIRTUAL

private:
    BYTE m_RenjuBoard[GRID_NUM][GRID_NUM];//棋盘数组，用于显示棋盘
    int m_nUserStoneColor;				  //用户棋子的颜色
    CSearchEngine* m_pSE;				  //搜索引擎指针

protected:
    void InvertRenjuBroad();
// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CRenjuDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnBtnnewgame();
    afx_msg void OnBtneixt();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnDestroy();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENJUDLG_H__F0F4F824_EA98_45F9_8643_DBA5E66A0A72__INCLUDED_)
