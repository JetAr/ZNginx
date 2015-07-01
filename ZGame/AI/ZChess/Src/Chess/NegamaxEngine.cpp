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

// NegamaxEngine.cpp: implementation of the CNegamaxEngine class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "chess.h"
#include "NegamaxEngine.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CNegamaxEngine::CNegamaxEngine()
{

}

CNegamaxEngine::~CNegamaxEngine()
{

}
void CNegamaxEngine::SearchAGoodMove(BYTE position[10][9])
{
    m_nMaxDepth = m_nSearchDepth;

    memcpy(CurPosition, position, 90);

    NegaMax(m_nMaxDepth);

    MakeMove(&m_cmBestMove);
    memcpy(position, CurPosition, 90);
}
extern int count;

int CNegamaxEngine::NegaMax(int depth)
{
    int current = -20000 ;
    int score;
    int Count,i;
    BYTE type;

    i = IsGameOver(CurPosition, depth);
    if (i != 0)
        return i;

    if (depth <= 0)	//叶子节点取估值
        return m_pEval->Eveluate(CurPosition, (m_nMaxDepth-depth)%2);

    Count = m_pMG->CreatePossibleMove(CurPosition, depth, (m_nMaxDepth-depth)%2);

    for (i=0; i<Count; i++)
    {

        type = MakeMove(&m_pMG->m_MoveList[depth][i]);
        score = -NegaMax(depth - 1);
        UnMakeMove(&m_pMG->m_MoveList[depth][i],type);

        if (score > current)
        {
            current = score;
            if(depth == m_nMaxDepth)
            {
                m_cmBestMove = m_pMG->m_MoveList[depth][i];
            }
        }

    }

    return current;
}

