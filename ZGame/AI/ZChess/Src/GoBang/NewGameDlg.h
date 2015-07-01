#if !defined(AFX_NEWGAMEDLG_H__A08444D3_48A5_441D_8B41_D214D7CAED97__INCLUDED_)
#define AFX_NEWGAMEDLG_H__A08444D3_48A5_441D_8B41_D214D7CAED97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewGameDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNewGameDlg dialog

class CNewGameDlg : public CDialog
{
// Construction
public:
    CNewGameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CNewGameDlg)
    enum { IDD = IDD_NEWGAME };
    CSpinButtonCtrl	m_SetPly;
    //}}AFX_DATA

public:
    int GetSelectedPly()
    {
        return m_nSelectedPly;
    };//取用户选中的搜索深度
    int GetStoneColor()
    {
        return m_nStoneColor;
    };  //取用户选中的棋子颜色

protected:
    int m_nSelectedPly;//记录用户选择的搜索层数
    int m_nStoneColor; //记录用户选择的棋子颜色

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNewGameDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNewGameDlg)
    virtual void OnOK();
    virtual BOOL OnInitDialog();
    afx_msg void OnBlackstone();
    afx_msg void OnWhitestone();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWGAMEDLG_H__A08444D3_48A5_441D_8B41_D214D7CAED97__INCLUDED_)
