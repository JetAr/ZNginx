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

// IDAlphabeta.cpp: implementation of the CIDAlphabeta class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "IDAlphabeta.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIDAlphabeta::CIDAlphabeta()
{

}

CIDAlphabeta::~CIDAlphabeta()
{

}

void CIDAlphabeta::SearchAGoodMove(BYTE position[10][9])
{
    CHESSMOVE backupmove;
    memcpy(CurPosition, position, 90);
    m_nTimeCount = GetTickCount();
    for (m_nMaxDepth = 1; m_nMaxDepth <= m_nSearchDepth; m_nMaxDepth++)
    {
        if (alphabeta(m_nMaxDepth, -20000, 20000) != 66666)
            backupmove = m_cmBestMove;
    }

    MakeMove(&backupmove);
    memcpy(position, CurPosition, 90);
}
int CIDAlphabeta::alphabeta(int depth, int alpha, int beta)
{
    int score;
    int Count,i;
    BYTE type;

    i = IsGameOver(CurPosition, depth);
    if (i != 0)
        return i;

    if (depth <= 0)	//叶子节点取估值
        return m_pEval->Eveluate(CurPosition, (m_nMaxDepth-depth)%2);

    Count = m_pMG->CreatePossibleMove(CurPosition, depth, (m_nMaxDepth-depth)%2);

    if (depth == m_nMaxDepth && m_nMaxDepth > 1)
    {
        for (i=1; i < Count; i++)
        {
            if (m_pMG->m_MoveList[depth][i].From.x == m_cmBestMove.From.x
                    && m_pMG->m_MoveList[depth][i].From.y == m_cmBestMove.From.y
                    && m_pMG->m_MoveList[depth][i].To.x == m_cmBestMove.To.x
                    && m_pMG->m_MoveList[depth][i].To.y == m_cmBestMove.To.y)
            {
                m_pMG->m_MoveList[depth][i] = m_pMG->m_MoveList[depth][0];
                m_pMG->m_MoveList[depth][0] = m_cmBestMove;
            }
        }
    }

    for (i=0; i<Count; i++)
    {
        if (depth == m_nMaxDepth)
        {
            if (GetTickCount() - m_nTimeCount >= 1000000)
                return 66666;
        }
        type = MakeMove(&m_pMG->m_MoveList[depth][i]);
        score = -alphabeta(depth - 1, -beta, -alpha);
        UnMakeMove(&m_pMG->m_MoveList[depth][i],type);

        if (score > alpha)
        {
            alpha = score;
            if(depth == m_nMaxDepth)
                m_cmBestMove = m_pMG->m_MoveList[depth][i];
        }
        if (alpha >= beta)
            break;


    }
    return alpha;
}

