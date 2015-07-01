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
// 版权声明
// ---------------
// 本文件所含之代码是《人机博弈程序设计指南》的范例程序中国象棋的一部分
// 您可以免费的使用, 编译 或者作为您应用程序的一部分。 
// 但，您不能在未经作者书面许可的情况下分发此源代码。 
// 如果您的应用程序使用了这些代码，在您的应用程序界面上 
// 放入 e-mail <hidebug@hotmail.com> 是令人欣赏的做法。
// 此代码并不含有任何保证，使用者当自承风险。
// 
// 王小春 <hidebug@hotmail.com>

// chessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chess.h"
#include "chessDlg.h"
#include "newgame.h"
#include "MoveGenerator.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

#define BORDERWIDTH 15 //棋盘(左右)边缘的宽度
#define BORDERHEIGHT 14 //棋盘(上下)边缘的高度
#define GRILLEWIDTH 39  //棋盘上每个格子的高度
#define GRILLEHEIGHT 39 //棋盘上每个格子的宽度
const BYTE InitChessBoard[10][9]=
{
	{B_CAR,   B_HORSE, B_ELEPHANT, B_BISHOP, B_KING,  B_BISHOP, B_ELEPHANT, B_HORSE, B_CAR},
	{NOCHESS, NOCHESS, NOCHESS,    NOCHESS,  NOCHESS, NOCHESS,  NOCHESS,    NOCHESS, NOCHESS},
	{NOCHESS, B_CANON, NOCHESS,    NOCHESS,  NOCHESS, NOCHESS,  NOCHESS,    B_CANON, NOCHESS},
	{B_PAWN,  NOCHESS, B_PAWN,     NOCHESS,  B_PAWN,  NOCHESS,  B_PAWN,     NOCHESS, B_PAWN},
	{NOCHESS, NOCHESS, NOCHESS,    NOCHESS,  NOCHESS, NOCHESS,  NOCHESS,    NOCHESS, NOCHESS},
	
	{NOCHESS, NOCHESS, NOCHESS,    NOCHESS,  NOCHESS, NOCHESS,  NOCHESS,    NOCHESS, NOCHESS},
	{R_PAWN,  NOCHESS, R_PAWN,     NOCHESS,  R_PAWN,  NOCHESS,  R_PAWN,     NOCHESS, R_PAWN},
	{NOCHESS, R_CANON, NOCHESS,    NOCHESS,  NOCHESS, NOCHESS,  NOCHESS,    R_CANON, NOCHESS},
	{NOCHESS, NOCHESS, NOCHESS,    NOCHESS,  NOCHESS, NOCHESS,  NOCHESS,    NOCHESS, NOCHESS},
	{R_CAR,   R_HORSE, R_ELEPHANT, R_BISHOP, R_KING,  R_BISHOP, R_ELEPHANT, R_HORSE, R_CAR}
};

/////////////////////////////////////////////////////////////////////////////
// CChessDlg dialog


////added by Spring Wang this code does not contents in books
int IsGameOver(BYTE position[10][9])
{
	int i,j;
	BOOL RedLive = FALSE, BlackLive=FALSE; 
	for (i = 7; i < 10; i++)
		for (j = 3; j < 6; j++)
		{
			if (position[i][j] == B_KING)
				BlackLive = TRUE;
			if (position[i][j] == R_KING)
				RedLive  = TRUE;
		}

	for (i = 0; i < 3; i++)
		for (j = 3; j < 6; j++)
		{
			if (position[i][j] == B_KING)
				BlackLive = TRUE;
			if (position[i][j] == R_KING)
				RedLive  = TRUE;
		}
	if (RedLive && BlackLive)
		return FALSE;
	else
		return TRUE;
}
////added by Spring Wang this code does not contents in books



CChessDlg::CChessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChessDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChessDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CChessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChessDlg)
	DDX_Control(pDX, IDC_NODECOUNT, m_OutputInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CChessDlg, CDialog)
	//{{AFX_MSG_MAP(CChessDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_NEWGAME, OnNewgame)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChessDlg message handlers

BOOL CChessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	m_Chessman.Create(IDB_CHESSMAN, 36, 14, RGB(0,255,0)); 

	BITMAP BitMap;
	m_BoardBmp.LoadBitmap(IDB_CHESSBOARD);
	m_BoardBmp.GetBitmap(&BitMap);
	m_nBoardWidth =  BitMap.bmWidth; 
	m_nBoardHeight = BitMap.bmHeight;
	m_BoardBmp.DeleteObject();


	memcpy(m_ChessBoard, InitChessBoard, 90);//初始化棋盘
	
	CMoveGenerator *pMG;
	CEveluation *pEvel;

	m_pSE = new CNegamaxEngine;
	pMG = new CMoveGenerator;
	pEvel = new CEveluation;
	
	m_pSE->SetSearchDepth(3);
	m_pSE->SetMoveGenerator(pMG);
	m_pSE->SetEveluator(pEvel);
	m_MoveChess.nChessID = NOCHESS;
	m_bGameOver = FALSE;//this code does not contents in books.

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CChessDlg::OnPaint() 
{
	CPaintDC dc(this);
	CDC MemDC;
	int i, j;
	POINT pt;
	CBitmap *pOldBmp;
	
	MemDC.CreateCompatibleDC( &dc );
	m_BoardBmp.LoadBitmap(IDB_CHESSBOARD);
	pOldBmp = MemDC.SelectObject(&m_BoardBmp);
	for (i = 0; i < 10; i++)
		for (j = 0; j < 9; j++)
		{
			if (m_ChessBoard[i][j] == NOCHESS)
				continue;
			pt.x = j*GRILLEHEIGHT+14;
			pt.y = i*GRILLEWIDTH+15;
			m_Chessman.Draw(&MemDC, m_ChessBoard[i][j]-1, pt, ILD_TRANSPARENT);		
		}
		if (m_MoveChess.nChessID != NOCHESS)
			m_Chessman.Draw(&MemDC, m_MoveChess.nChessID - 1, m_MoveChess.ptMovePoint, ILD_TRANSPARENT);		
		
		dc.BitBlt(0, 0, m_nBoardWidth, m_nBoardHeight, &MemDC, 0, 0, SRCCOPY);
		MemDC.SelectObject(&pOldBmp);
		MemDC.DeleteDC();
		m_BoardBmp.DeleteObject();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CChessDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CChessDlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_bGameOver)
		return;
	memcpy(m_BackupChessBoard, m_ChessBoard, 90);
	if (point.x > 0 && point.y > 0 &&point.x< m_nBoardWidth && point.y< m_nBoardHeight && 
		IsRed(m_ChessBoard[(point.y-14)/GRILLEHEIGHT][(point.x-15)/GRILLEWIDTH]))
	{
		memcpy(m_BackupChessBoard, m_ChessBoard, 90);
		m_ptMoveChess.x = (point.x-15)/GRILLEWIDTH;
		m_ptMoveChess.y = (point.y-14)/GRILLEHEIGHT;
		m_MoveChess.nChessID = m_ChessBoard[m_ptMoveChess.y][m_ptMoveChess.x];
		m_ChessBoard[m_ptMoveChess.y][m_ptMoveChess.x] = NOCHESS;
		point.x -= 18;
		point.y -= 18;
		m_MoveChess.ptMovePoint = point;
		InvalidateRect(NULL,FALSE);
		UpdateWindow();
		SetCapture();
	}
	CDialog::OnLButtonDown(nFlags, point);
}
extern int count;

void CChessDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	BOOL bTurnSide = FALSE; 

	if (m_bGameOver)
		return;

	int timecount;
	
	if (m_MoveChess.nChessID &&
		CMoveGenerator::IsValidMove(m_BackupChessBoard, m_ptMoveChess.x, m_ptMoveChess.y,
		(point.x-15)/GRILLEWIDTH,(point.y-14)/GRILLEHEIGHT))
	{
		m_ChessBoard[(point.y-14)/GRILLEHEIGHT][(point.x-15)/GRILLEWIDTH] = m_MoveChess.nChessID;
		bTurnSide = TRUE;
	}
	else
		memcpy(m_ChessBoard, m_BackupChessBoard, 90);
	
	m_MoveChess.nChessID = NOCHESS;
	InvalidateRect(NULL,FALSE);
	UpdateWindow();
	
	ReleaseCapture();
	if (bTurnSide == TRUE)
	{
		timecount = GetTickCount();
		m_pSE->SearchAGoodMove(m_ChessBoard);	
		
		CString sNodeCount;

		sNodeCount.Format(" Cost %d ms. %d Nodes were eveluated.", GetTickCount() - timecount,count);
		m_OutputInfo.SetWindowText(sNodeCount);
	}
	
	if (IsGameOver(m_ChessBoard))
	{
		m_bGameOver = TRUE;
		MessageBox("Game Over! Press New Game to replay...");
	}
	
	count = 0;
	InvalidateRect(NULL, FALSE);
	UpdateWindow();
	CDialog::OnLButtonUp(nFlags, point);
}

void CChessDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	if (m_MoveChess.nChessID)
	{
		//防止将棋子拖出棋盘
		if( point.x < 15)
			point.x = 15;
		if( point.y < 15)
			point.y = 15;
		if( point.x > m_nBoardWidth - 15)
			point.x = m_nBoardWidth - 15;
		if( point.y > m_nBoardHeight - 15)
			point.y = m_nBoardHeight - 15;

		point.x -= 18;
		point.y -= 18;
		m_MoveChess.ptMovePoint = point;
		InvalidateRect(NULL,FALSE);//刷新窗口
		UpdateWindow();
	}
	
	CDialog::OnMouseMove(nFlags, point);
}

void CChessDlg::OnNewgame() 
{
	// TODO: Add your control notification handler code here
	CMoveGenerator * pMG;
	CEveluation *pEvel;

	CNewGame newGameDlg;
	if (  newGameDlg.DoModal() == IDOK)
	{
		if (m_pSE)
			delete m_pSE;
		switch(newGameDlg.GetSelectedEngine())
		{
		case 0:
			m_pSE = new CNegamaxEngine;
			break;

		case 1:
			m_pSE = new CAlphaBetaEngine;
			break;

		case 2:
			m_pSE = new CFAlphaBetaEngine;
			break;

		case 3:
			m_pSE = new CAspirationSearch;
			break;

		case 4:
			m_pSE = new CPVS_Engine;
			break;

		case 5:
			m_pSE = new CIDAlphabeta;
			break;

		case 6:
			m_pSE = new CAlphaBetaAndTT;
			break;

		case 7:
			m_pSE = new CAlphabeta_HH;
			break;

		case 8:
			m_pSE = new CMTD_f;
			break;

		case 9:
			m_pSE = new CNegaScout_TT_HH;
			break;
		}
		m_pSE->SetSearchDepth(newGameDlg.GetSelectedPly());
		pEvel = new CEveluation;
	}
	else 
		return;

	memcpy(m_ChessBoard, InitChessBoard, 90);//初始化棋盘
	m_MoveChess.nChessID = NOCHESS;

	pMG = new CMoveGenerator;
	m_pSE->SetMoveGenerator(pMG);
	m_pSE->SetEveluator(pEvel);
	
	m_bGameOver = FALSE;//this code does not contents in books.

	InvalidateRect(NULL,FALSE);
	UpdateWindow();
}


