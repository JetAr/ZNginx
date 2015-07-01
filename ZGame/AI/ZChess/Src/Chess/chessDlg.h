// COPYRIGHT NOTES
// ---------------
// This source code is a part of chess which is an example of <Game Programing guide>.
// You may use, compile or redistribute it as part of your application
// for free.
// You cannot redistribute sources without the official agreement of the author.
// If distribution of you application which contents code below was occured, place
// e-mail <hidebug@hotmail.com> on it is to be appreciated.
// This code can be used WITHOUT ANY WARRANTIES on your own risk.
//
// Spring Wang <hidebug@hotmail.com>

// ---------------
// ��Ȩ����
// ---------------
// ���ļ�����֮�����ǡ��˻����ĳ������ָ�ϡ��ķ��������й������һ����
// ��������ѵ�ʹ��, ���� ������Ϊ��Ӧ�ó����һ���֡�
// ������������δ������������ɵ�����·ַ���Դ���롣
// �������Ӧ�ó���ʹ������Щ���룬������Ӧ�ó��������
// ���� e-mail <hidebug@hotmail.com> ���������͵�������
// �˴��벢�������κα�֤��ʹ���ߵ��Գз��ա�
//
// ��С�� <hidebug@hotmail.com>

// chessDlg.h : header file
//

#if !defined(AFX_CHESSDLG_H__2B09B234_CA39_11D5_AEC7_5254AB2E22C7__INCLUDED_)
#define AFX_CHESSDLG_H__2B09B234_CA39_11D5_AEC7_5254AB2E22C7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "define.h"
#include "SearchEngine.h"
#include "NegamaxEngine.h"
#include "AlphaBetaEngine.h"
#include "AlphaBetaAndTT.h"
#include "FAlphaBetaEngine.h"
#include "PVS_Engine.h"
#include "NegaScout.h"
#include "IDAlphabeta.h"
#include "AspirationSearch.h"
#include "Alphabeta_HH.h"
#include "mtd_f.h"
#include "NegaScout_TT_HH.h"

typedef struct _movechess
{
    BYTE nChessID;
    POINT ptMovePoint;
} MOVECHESS;
/////////////////////////////////////////////////////////////////////////////
// CChessDlg dialog
class CChessDlg : public CDialog
{
// Construction
public:

    CChessDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
    //{{AFX_DATA(CChessDlg)
    enum { IDD = IDD_CHESS_DIALOG };
    CStatic	m_OutputInfo;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CChessDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(CChessDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    afx_msg void OnNewgame();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
private:
    BOOL m_bGameOver;
    BYTE m_ChessBoard[10][9];
    BYTE m_BackupChessBoard[10][9];
    MOVECHESS m_MoveChess;
    POINT m_ptMoveChess;
    CBitmap m_BoardBmp;
    CImageList m_Chessman;
    int m_nBoardWidth;
    int m_nBoardHeight;
    CSearchEngine *m_pSE;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHESSDLG_H__2B09B234_CA39_11D5_AEC7_5254AB2E22C7__INCLUDED_)
